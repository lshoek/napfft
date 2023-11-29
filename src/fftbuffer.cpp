/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "fftbuffer.h"

// NAP includes
#include <nap/core.h>
#include <mathutils.h>
#include <glm/gtc/constants.hpp>
#include <nap/assert.h>
#include <nap/logger.h>

// External includes
#include <kiss_fftr.h>

// Overlap enum
RTTI_BEGIN_ENUM(nap::FFTBuffer::EOverlap)
	RTTI_ENUM_VALUE(nap::FFTBuffer::EOverlap::One,		"One"),
	RTTI_ENUM_VALUE(nap::FFTBuffer::EOverlap::Three,	"Three"),
	RTTI_ENUM_VALUE(nap::FFTBuffer::EOverlap::Five,		"Five"),
	RTTI_ENUM_VALUE(nap::FFTBuffer::EOverlap::Seven,	"Seven"),
	RTTI_ENUM_VALUE(nap::FFTBuffer::EOverlap::Nine,		"Nine"),
	RTTI_ENUM_VALUE(nap::FFTBuffer::EOverlap::Eleven,	"Eleven")
RTTI_END_ENUM

// nap::FFTBuffer run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::FFTBuffer)
	RTTI_CONSTRUCTOR(nap::uint)
RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////


namespace nap
{
	//////////////////////////////////////////////////////////////////////////
	// Kiss Context
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Safe kissfft context wrapper
	 */
	class FFTBuffer::KissContext
	{
	public:
		// Convert to fast size (factors 2, 3, or 5) for optimal Kiss FFT performance
		KissContext(uint dataSize) :
			mSize(kiss_fftr_next_fast_size_real(static_cast<int>(dataSize)))
		{
			mForwardConfig = kiss_fftr_alloc(mSize, 0, NULL, NULL);
			mInverseConfig = kiss_fftr_alloc(mSize, 1, NULL, NULL);
		}

		~KissContext()
		{
			kiss_fftr_free(mForwardConfig);
			kiss_fftr_free(mInverseConfig);
		}

		int getSize() const { return mSize; }

		kiss_fftr_cfg mForwardConfig = nullptr;
		kiss_fftr_cfg mInverseConfig = nullptr;
		const int mSize;
	};


	void FFTBuffer::KissContextDeleter::operator()(FFTBuffer::KissContext* ctx) const
	{
		delete ctx;
	}


	//////////////////////////////////////////////////////////////////////////
	// FFT Buffer
	//////////////////////////////////////////////////////////////////////////

	FFTBuffer::FFTBuffer(uint dataSize, EOverlap overlap)
	{
		// Create kiss context
		mContext = std::unique_ptr<FFTBuffer::KissContext, FFTBuffer::KissContextDeleter>(new FFTBuffer::KissContext(dataSize));
		const uint data_size = mContext->getSize();

		// Compute hop size
		mOverlap = overlap;
		mHopSize = data_size / static_cast<uint>(mOverlap);

		// Data size required to perform sliding DFT
		uint full_buffer_size = (mHopSize > 1) ? data_size * 2 : data_size;;

		// Create sample buffer
		mSampleBuffer.resize(data_size * 2);
		mSampleBufferWindowed.resize(data_size);
		mSampleBufferHalfPtr = mSampleBuffer.data() + mSampleBuffer.size() / 2;

		// Compute hamming window
		mForwardHammingWindow.resize(data_size);
		for (uint i = 0; i < data_size; ++i)
		{
			mForwardHammingWindow[i] = 0.54f - 0.46f * std::cos(2.0f * glm::pi<float>() * (i / static_cast<float>(data_size)));
			mHammingWindowSum += mForwardHammingWindow[i];
		}
		mNormalizationFactor = 2.0f / mHammingWindowSum;

		// Bins
		mBinCount = data_size / 2 + 1;

		// Create FFT buffers
		mComplexOut.resize(mBinCount);
		mComplexOutAverage.resize(mBinCount);
		mAmplitude.resize(mBinCount);
		mPhase.resize(mBinCount);
	}

	FFTBuffer::~FFTBuffer() {}


	void FFTBuffer::supply(const std::vector<float>& samples)
	{
		const uint data_size = mContext->getSize();
		const uint data_bytes = data_size * sizeof(float);

		{
			std::lock_guard<std::mutex> lock(mSampleBufferMutex);

			// Copy second half to first half
			std::memcpy(mSampleBuffer.data(), mSampleBufferHalfPtr, data_bytes);

			// Copy new samples to second half
			if (samples.size() == data_size)
			{
				std::memcpy(mSampleBufferHalfPtr, samples.data(), data_bytes);
			}
			else if (samples.size() > data_size)
			{
				// Zero-padding
				mSampleBuffer.clear();
				std::memcpy(mSampleBufferHalfPtr, samples.data(), data_bytes);
			}
			else
			{
				NAP_ASSERT_MSG(false, "Specified sample buffer size too small");
				return;
			}
		}
		mDirty = true;
	}


	void FFTBuffer::transform()
	{
		if (mDirty)
		{
			// Copy data to windowed array
			const uint data_size = mContext->getSize();
			const uint hop_count = static_cast<uint>(mOverlap);
			std::fill(mComplexOutAverage.begin(), mComplexOutAverage.end(), 0.0f);

			{
				std::lock_guard<std::mutex> lock(mSampleBufferMutex);

				// Perform FFT
				std::memcpy(mSampleBufferWindowed.data(), mSampleBufferHalfPtr, sizeof(float) * data_size);

				for (uint h = 0; h < hop_count; h++)
				{
					const uint start = (h+1) * mHopSize;

					// Apply hamming window
					for (uint i = 0; i < data_size; ++i)
						mSampleBufferWindowed[i] = mSampleBuffer[start + i] * mForwardHammingWindow[i];

					// Scales amplitudes by nfft/2
					kiss_fftr(mContext->mForwardConfig, static_cast<const float*>(mSampleBufferWindowed.data()), reinterpret_cast<kiss_fft_cpx*>(mComplexOut.data()));

					// Add complex out to average
					for (uint i = 0; i < mComplexOut.size(); ++i)
						mComplexOutAverage[i] += mComplexOut[i];
				}
			}

			// Average windowed buffer
			if (hop_count > 1)
			{
				const float divisor = 1.0f / static_cast<float>(hop_count);
				for (auto& c : mComplexOutAverage)
					c *= divisor;
			}

			// Compute amplitudes and phase angles
			for (uint i = 0; i < mBinCount; i++)
			{
				const auto& cpx = mComplexOut[i];
				mAmplitude[i] = std::abs(cpx) * mNormalizationFactor;
				mPhase[i] = std::arg(cpx);
			}
			mDirty = false;
		}
	}


	const std::vector<float>& FFTBuffer::getAmplitudeSpectrum()
	{
		if (mDirty)
			transform();

		return mAmplitude;
	}


	const std::vector<float>& FFTBuffer::getPhaseSpectrum()
	{
		if (mDirty)
			transform();

		return mPhase;
	}


	uint FFTBuffer::getDataSize()
	{
		return mContext->getSize();
	}
}
