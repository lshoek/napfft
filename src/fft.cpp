#include "fft.h"

// External includes
#include <mathutils.h>

// nap::FFTBuffer run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::FFTBuffer)
RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////


namespace nap
{
	FFTBuffer::FFTBuffer(uint dataSize) : mDataSize(dataSize)
	{
		mBinCount = dataSize / 2 + 1;

		mFFTBuffer.resize(mBinCount);
		mSampleBuffer.reserve(mDataSize);

		kiss_fft_cfg cfg = kiss_fft_alloc(mDataSize, 0, 0, 0);
		//mFFTConfig = std::make_unique<kiss_fft_cfg>(kiss_fft_alloc(mDataSize, 0, 0, 0));
	}


	void FFTBuffer::supplySamples(const std::vector<float>& samples)
	{

	}
}
