/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "onsetdetectioncomponent.h"
#include "fftutils.h"

// Nap includes
#include <entity.h>
#include <nap/core.h>

RTTI_BEGIN_CLASS(nap::OnsetDetectionComponent::FilterParameterItem)
	RTTI_PROPERTY("Parameter", &nap::OnsetDetectionComponent::FilterParameterItem::mParameter, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("Multiplier", &nap::OnsetDetectionComponent::FilterParameterItem::mMultiplier, nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("ThresholdDecay", &nap::OnsetDetectionComponent::FilterParameterItem::mThresholdDecay, nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("MinimumBin", &nap::OnsetDetectionComponent::FilterParameterItem::mMinBin, nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("MaximumBin", &nap::OnsetDetectionComponent::FilterParameterItem::mMaxBin, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::OnsetDetectionComponent)
	RTTI_PROPERTY("Parameters", &nap::OnsetDetectionComponent::mParameters, nap::rtti::EPropertyMetaData::Default | nap::rtti::EPropertyMetaData::Embedded)
	RTTI_PROPERTY("Enable", &nap::OnsetDetectionComponent::mEnable, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::OnsetDetectionComponentInstance)
		RTTI_CONSTRUCTOR(nap::EntityInstance &, nap::Component &)
RTTI_END_CLASS

namespace nap
{
	//////////////////////////////////////////////////////////////////////////
	// OnsetDetectionComponent
	//////////////////////////////////////////////////////////////////////////

	void OnsetDetectionComponent::getDependentComponents(std::vector<rtti::TypeInfo>& components) const
	{
		components.emplace_back(RTTI_OF(FFTAudioComponent));
	}


	//////////////////////////////////////////////////////////////////////////
	// OnsetDetectionComponentInstance
	//////////////////////////////////////////////////////////////////////////

	bool OnsetDetectionComponentInstance::init(utility::ErrorState& errorState)
	{
		// Fetch resource
		mResource = getComponent<OnsetDetectionComponent>();

		// Ensure FFTAudioComponentInstance is available
		mFFTAudioComponent = &getEntityInstance()->getComponent<FFTAudioComponentInstance>();
		if (!errorState.check(mFFTAudioComponent != nullptr, "Missing nap::FFTAudioComponentInstance under entity"))
			return false;

		const uint bin_count = mFFTAudioComponent->getFFTBuffer().getBinCount();
		mOnsetList.reserve(mResource->mParameters.size());
		for (const auto& entry : mResource->mParameters)
		{
			if (!errorState.check(entry->mMaxBin < bin_count, "MaxBin of '%s' (%d) exceeds FFT buffer bin count of %d:", entry->mID.c_str(), entry->mMaxBin, bin_count))
				return false;

			mOnsetList.emplace_back(*entry);
		}
		mPreviousBuffer.resize(bin_count);
		return true;
	}


	void OnsetDetectionComponentInstance::update(double deltaTime)
	{
		if (!mResource->mEnable)
			return;

		const auto& amps = mFFTAudioComponent->getFFTBuffer().getAmplitudeSpectrum();
		for (const auto& entry : mOnsetList)
		{
			float mult = (entry.mMultiplier != nullptr) ? entry.mMultiplier->mValue : 1.0f;
			float decay = (entry.mThresholdDecay != nullptr) ? entry.mThresholdDecay->mValue : 0.0005f;
			float value = std::min(entry.mParameter->mValue, 1.0f);
			float value_inverse = 1.0f - value;
			float decrement = std::abs(value_inverse - value_inverse * std::pow(decay, static_cast<float>(deltaTime)));

			float flux = utility::flux(amps, mPreviousBuffer, entry.mMinMaxBins.x, entry.mMinMaxBins.y) * mult;
			entry.mParameter->setValue(std::max<float>(value - decrement, flux));
		}

		// Copy
		mPreviousBuffer = amps;
	}
}
