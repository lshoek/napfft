#pragma once

// External Includes
#include <nap/resource.h>

namespace nap
{
	/**
	 * FFTAnalysis
	 */
	class NAPAPI FFTAnalysis : public Resource
	{
		RTTI_ENABLE(Resource)
	public:
		virtual ~FFTAnalysis();

		/**
		* Initialize this object after de-serialization
		* @param errorState contains the error message when initialization fails
		*/
		virtual bool init(utility::ErrorState& errorState) override;
	};
}
