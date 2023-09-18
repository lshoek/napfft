/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

// External Includes
#include <nap/resource.h>
#include <nap/numeric.h>
#include <complex>
#include <mutex>

namespace nap
{
	/**
	 * FFTBuffer
	 */
	class NAPAPI FFTBuffer
	{
		RTTI_ENABLE()
	public:
		// Constructor
		FFTBuffer(uint dataSize);
		~FFTBuffer();

		/**
		 * Update the internal sample buffer to perform FFT on. This funtion is thread-safe.
		 */
		void supply(const std::vector<float>& samples);

		/**
		 * Performs FFT and updates amplitudes and phases. This funtion is thread-safe.
		 */
		void transform();

		/**
		 * @return the number of FFT bins
		 */
		uint getBinCount() const { return mBinCount; }

		/**
		 * @return the number of sample buffer data elements
		 */
		uint getDataSize();

		/**
		 * @return normalized magnitudes (rho)
		 */
		const std::vector<float>& getAmplitudes();

		/**
		 * @return normalized phase angles (theta)
		 */
		const std::vector<float>& getPhases();

	private:
		class KissContext;
		struct KissContextDeleter { void operator()(KissContext* ctx) const; };
		std::unique_ptr<KissContext, KissContextDeleter> mContext;

		std::vector<std::complex<float>> mComplexOut;
		std::vector<float> mAmplitude;					//< magnitude (rho)
		std::vector<float> mPhase;						//< phase angle (theta)

		std::vector<float> mForwardHammingWindow;		//< preprocess samples for fft window
		float mHammingWindowSum = 0.0f;
		float mNormalizationFactor;

		// The sample buffer is accessed on both the audio and main thread. Use mutex for read/write.
		std::vector<float> mSampleBuffer;
		std::mutex mSampleBufferMutex;

		std::vector<float> mSampleBufferWindowed;

		uint mBinCount;
		bool mDirty = false;
	};
}
