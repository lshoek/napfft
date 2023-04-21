
#include "fft.h"

// External includes
#include <kiss_fft.h>

// nap::FFTAnalysis run time class definition 
RTTI_BEGIN_CLASS(nap::FFTAnalysis)

RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////


namespace nap
{
	FFTAnalysis::~FFTAnalysis() { }


	bool FFTAnalysis::init(utility::ErrorState& errorState)
	{
		return true;
	}
}
