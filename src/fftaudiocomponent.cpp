/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "fftaudiocomponent.h"
 
// Nap includes
#include <entity.h>
#include <nap/core.h>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::FFTAudioComponent)
	RTTI_PROPERTY("Overlaps", &nap::FFTAudioComponent::mOverlaps, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::FFTAudioComponentInstance)
RTTI_END_CLASS
