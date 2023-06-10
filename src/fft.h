#pragma once

// External Includes
#include <nap/resource.h>
#include <nap/numeric.h>
#include <complex>

#include <kiss_fft.h>

namespace nap
{
	/**
	 * FFTBuffer
	 */
	class NAPAPI FFTBuffer
	{
		RTTI_ENABLE()
	public:
		FFTBuffer(uint dataSize);
		virtual ~FFTBuffer() {}

		/**
		 *
		 */
		void supplySamples(const std::vector<float>& samples);

		/**
		 *
		 */
		const std::vector<std::complex<float>>& getFFT()		{ return mFFTBuffer; }

		/**
		 *
		 */
		bool isDirty() const									{ return mDirty; }

	private:
		uint mDataSize;
		uint mBinCount;

		std::unique_ptr<kiss_fft_cfg> mFFTConfig;
		std::unique_ptr<kiss_fft_cfg> mInverseFFTConfig;

		std::vector<float> mSampleBuffer;
		std::vector<std::complex<float>> mFFTBuffer;
		bool mDirty = false;
	};
}
