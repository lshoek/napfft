#pragma once

// External Includes
#include <nap/resource.h>
#include <nap/numeric.h>
#include <complex>

namespace nap
{
	// Forward declares
	template <typename T>
	class KissFFTState;

	/**
	 * FFTBuffer
	 */
	class NAPAPI FFTBuffer : public Resource
	{
		RTTI_ENABLE(Resource)
	public:
		virtual ~FFTBuffer() {};
		uint mRequestedFFTSize = 1024U;
	};


	template <typename T>
	class NAPAPI TypedFFTBuffer : public FFTBuffer
	{
		RTTI_ENABLE(FFTBuffer)
	public:
		TypedFFTBuffer();
		virtual ~TypedFFTBuffer() {}

		/**
		* Initialize this object after de-serialization
		* @param errorState contains the error message when initialization fails
		*/
		virtual bool init(utility::ErrorState& errorState) override;

		/**
		 * 
		 */
		bool fft(const std::vector<T>& sampleBuffer, std::vector<std::complex<T>>& outBuffer, utility::ErrorState& errorState);

		std::unique_ptr<KissFFTState<T>> mFFTState;
	};

	using FFTBufferFloat = TypedFFTBuffer<float>;
	using FFTBufferDouble = TypedFFTBuffer<double>;
	using FFTBufferInt = TypedFFTBuffer<int>;
	using FFTBufferShort = TypedFFTBuffer<int16>;
}
