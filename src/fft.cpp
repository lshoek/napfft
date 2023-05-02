#include "fft.h"

// External includes
#include <kissfft.hh>
#include <mathutils.h>

// nap::FFTBuffer run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::FFTBuffer)
	RTTI_PROPERTY("RequestedFFTSize", &nap::FFTBuffer::mRequestedFFTSize, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::FFTBufferFloat)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::FFTBufferDouble)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::FFTBufferInt)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::FFTBufferShort)
RTTI_END_CLASS


//////////////////////////////////////////////////////////////////////////


namespace nap
{
	template <typename T>
	class KissFFTState
	{
	public:
		KissFFTState::KissFFTState(size_t fftSize) :
			mFFT(fftSize, false) { }

		kissfft<T> mFFT;
	};


	template <typename T>
	TypedFFTBuffer<T>::TypedFFTBuffer() :
		FFTBuffer()
	{ }


	template <typename T>
	bool TypedFFTBuffer<T>::init(utility::ErrorState& errorState)
	{
		// Determine optimal number of frequency bins
		mFFTState = std::make_unique<KissFFTState<T>>(static_cast<size_t>(mRequestedFFTSize));

		std::vector<T> test_samples;
		test_samples.reserve(2048);
		for (uint i = 0; i < 2048; i++)
			test_samples.emplace_back(static_cast<T>(math::random(0.0f, 32.0f)));

		std::vector<std::complex<T>> result(test_samples.size());
		fft(test_samples, result, errorState);

		return true;
	}


	template <typename T>
	bool TypedFFTBuffer<T>::fft(const std::vector<T>& sampleBuffer, std::vector<std::complex<T>>& outBuffer, utility::ErrorState& errorState)
	{		
		outBuffer.reserve(mRequestedFFTSize);
		mFFTState->mFFT.transform_real(sampleBuffer.data(), outBuffer.data());
		return true;
	}
}
