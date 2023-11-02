/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

// Local includes
#include "fftbuffer.h"

// Nap includes
#include <component.h>

namespace nap
{
	class FFTAudioComponentInstance;
			
	/**
	 * Component to measure the fft of the audio signal from an @AudioComponentBase.
	 */
	class NAPAPI FFTAudioComponent : public Component
	{
		RTTI_ENABLE(Component)
		DECLARE_COMPONENT(FFTAudioComponent, FFTAudioComponentInstance)	
	public:		
		FFTBuffer::EOverlap mOverlaps = FFTBuffer::EOverlap::One;	///< Property: 'Overlaps' Number of overlaps, more increases fft precision in excahange for performance
	};
	
		
	/**
	 * Instance of component to measure the amplitude level of the audio signal from an @AudioComponentBase.
	 * A specific frequency band to be measured can be specified.
	 */
	class NAPAPI FFTAudioComponentInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		FFTAudioComponentInstance(EntityInstance& entity, Component& resource) :
			ComponentInstance(entity, resource) {}

		/**
		 * 
		 */
		virtual const FFTBuffer& getFFTBuffer() const = 0;

		/**
		 *
		 */
		virtual FFTBuffer& getFFTBuffer() = 0;
	};
}
