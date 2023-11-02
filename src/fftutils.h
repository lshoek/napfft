/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

// Local Includes
#include "fftbuffer.h"

// External Includes
#include <utility/dllexport.h>

namespace nap
{
	namespace utility
	{
		/**
		 * @param binCount the number of frequency bins
		 * @param nyqist nyqist frequency in Hz (e.g. 44100 Hz)
		 * @return frequency interval in Hz
		 */
		float NAPAPI interval(uint binCount, float nyqist);

		/**
		 * @param bin the bin to retrieve the frequency from
		 * @param binCount the number of frequency bins
		 * @param nyqist nyqist frequency in Hz (e.g. 44100 Hz)
		 * @return frequency at bin in Hz
		 */
		float NAPAPI freq(uint bin, uint binCount, float nyqist);

		/**
		 * Cutoff frequency
		 * @param inAmps
		 * @param outAmps
		 * @param minBin minimum bin to cutoff
		 * @param maxBin maximum bin to cutoff
		 * @return
		 */
		void NAPAPI cutoff(const FFTBuffer::AmplitudeSpectrum& inAmps, FFTBuffer::AmplitudeSpectrum& outAmps, uint minBin, uint maxBin);

		/**
		 * Spectral average
		 * @return the spectrum average
		 */
		float NAPAPI average(const FFTBuffer::AmplitudeSpectrum& amps, uint minBin = 0, uint maxBin = 0);

		/**
		 * Spectral centroid
		 * @return the normalized spectral centroid
		 */
		float NAPAPI centroid(const FFTBuffer::AmplitudeSpectrum& amps);

		/**
		 * Spectral flux
		 * @return the spectral flux
		 */
		float NAPAPI flux(const FFTBuffer::AmplitudeSpectrum& current, const FFTBuffer::AmplitudeSpectrum& previous, uint minBin, uint maxBin);
	}
}
