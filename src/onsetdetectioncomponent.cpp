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
	RTTI_PROPERTY("SmoothTime", &nap::OnsetDetectionComponent::FilterParameterItem::mSmoothTime, nap::rtti::EPropertyMetaData::Default)
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

		const float delta_time = static_cast<float>(deltaTime);
		mElapsedTime += delta_time;

		// Fetch amplitudes
		const auto& amps = mFFTAudioComponent->getFFTBuffer().getAmplitudeSpectrum();

		for (auto& entry : mOnsetList)
		{
			float decay = (entry.mThresholdDecay != nullptr) ? entry.mThresholdDecay->mValue : 0.0001f;

			static const float maximum_decay = 0.95f; // Ensure a decreasing gradient
			float value_inverse = 1.0f - std::clamp(entry.mOnsetValue, 0.0f, maximum_decay);
			float decrement = std::abs(value_inverse - value_inverse * std::pow(decay, delta_time));

			float mult = (entry.mMultiplier != nullptr) ? entry.mMultiplier->mValue : 1.0f;
			float flux = utility::flux(amps, mPreviousBuffer, entry.mMinMaxBins.x, entry.mMinMaxBins.y) * mult;

			entry.mOnsetValue = std::clamp(std::max(flux, entry.mOnsetValue - decrement), 0.0f, 1.0f);
			float value_smoothed = entry.mOnsetSmoother.update(entry.mOnsetValue, delta_time);
			entry.mParameter->setValue(value_smoothed * entry.mParameter->mMaximum);
		}

		// Copy
		mPreviousBuffer = amps;
	}
}
