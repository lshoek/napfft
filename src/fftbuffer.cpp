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


	//////////////////////////////////////////////////////////////////////////
	// Kiss Context delete
	//////////////////////////////////////////////////////////////////////////

	void FFTBuffer::KissContextDeleter::operator()(FFTBuffer::KissContext* ctx) const
	{
		delete ctx;
	}


	//////////////////////////////////////////////////////////////////////////
	// FFT Buffer
	//////////////////////////////////////////////////////////////////////////

	FFTBuffer::FFTBuffer(uint dataSize)
	{
		// Create kiss context
		mContext = std::unique_ptr<FFTBuffer::KissContext, FFTBuffer::KissContextDeleter>(new FFTBuffer::KissContext(dataSize));
		const uint data_size = mContext->getSize();

		// Create sample buffer
		mSampleBuffer.resize(data_size);
		mSampleBufferWindowed.resize(data_size);

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
		mAmplitude.resize(mBinCount);
		mPhase.resize(mBinCount);
	}

	FFTBuffer::~FFTBuffer() {}


	void FFTBuffer::supply(const std::vector<float>& samples)
	{
		{
			std::lock_guard<std::mutex> lock(mSampleBufferMutex);

			if (samples.size() == mSampleBuffer.size())
			{
				mSampleBuffer = samples;
			}
			else if (samples.size() > mSampleBuffer.size())
			{
				// Zero-padding
				mSampleBuffer.clear();
				std::memcpy(mSampleBuffer.data(), samples.data(), mSampleBuffer.size() * sizeof(float));
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
			{
				// Perform FFT
				std::lock_guard<std::mutex> lock(mSampleBufferMutex);

				// Copy data to windowed array
				const uint data_size = mContext->getSize();
				std::memcpy(mSampleBufferWindowed.data(), mSampleBuffer.data(), sizeof(float) * data_size);

				// Apply hamming window
				for (int32_t i = 0; i < data_size; ++i)
					mSampleBufferWindowed[i] = mSampleBuffer[i] * mForwardHammingWindow[i];

				// Scales amplitudes by nfft/2
				kiss_fftr(mContext->mForwardConfig, static_cast<const float*>(mSampleBufferWindowed.data()), reinterpret_cast<kiss_fft_cpx*>(mComplexOut.data()));
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


	const std::vector<float>& FFTBuffer::getAmplitudes()
	{
		if (mDirty)
			transform();

		return mAmplitude;
	}


	const std::vector<float>& FFTBuffer::getPhases()
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
