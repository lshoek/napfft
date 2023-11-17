/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

// Local includes
#include "fftaudiocomponent.h"

// Nap includes
#include <component.h>
#include <parameternumeric.h>
#include <smoothdamp.h>

namespace nap
{
	class OnsetDetectionComponentInstance;
			
	/**
	 * Component to measure onsets of the audio signal from an @AudioComponentBase.
	 */
	class NAPAPI OnsetDetectionComponent : public Component
	{
		RTTI_ENABLE(Component)
		DECLARE_COMPONENT(OnsetDetectionComponent, OnsetDetectionComponentInstance)	
	public:
		/**
		 * FilterParameterItem
		 */
		class NAPAPI FilterParameterItem : public Resource
		{
			RTTI_ENABLE(Resource)
		public:
			bool init(utility::ErrorState& errorState) override { return true; }

			rtti::ObjectPtr<ParameterFloat> mParameter;
			rtti::ObjectPtr<ParameterFloat> mMultiplier;
			rtti::ObjectPtr<ParameterFloat> mThresholdDecay;
			uint mMinBin = 0;
			uint mMaxBin = 1;
			float mSmoothTime = 0.05f;
		};

		// Constructor
		OnsetDetectionComponent() :
			Component() { }

		void getDependentComponents(std::vector<rtti::TypeInfo>& components) const override;

		std::vector<rtti::ObjectPtr<FilterParameterItem>> mParameters;
		bool mEnable = true;
	};
		
		
	/**
	 * Instance of component to measure onsets of the audio signal from an @AudioComponentBase.
	 * A specific frequency band to be measured can be specified.
	 */
	class NAPAPI OnsetDetectionComponentInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		/**
		 * OnsetData
		 */
		class OnsetData
		{
		public:
			OnsetData(const OnsetDetectionComponent::FilterParameterItem& item) :
				mParameter(item.mParameter.get()), mMultiplier(item.mMultiplier.get()), mThresholdDecay(item.mThresholdDecay.get()), mMinMaxBins({ item.mMinBin, item.mMaxBin })
			{}

			ParameterFloat* mParameter = nullptr;
			ParameterFloat* mMultiplier = nullptr;
			ParameterFloat* mThresholdDecay = nullptr;
			glm::uvec2 mMinMaxBins = { 0, 255 };
			float mOnsetValue = 0.0f;
			math::FloatSmoothOperator mOnsetSmoother{ 0.0f, 0.05f };
		};

		// Constructor
		OnsetDetectionComponentInstance(EntityInstance& entity, Component& resource) :
			ComponentInstance(entity, resource) {}

		// Initialize the component
		bool init(utility::ErrorState& errorState) override;

		/**
		 * Update this component
		 * @param deltaTime the time in between cooks in seconds
		 */
		virtual void update(double deltaTime) override;

		/**
		 * 
		 */
		const std::vector<rtti::ObjectPtr<OnsetDetectionComponent::FilterParameterItem>>& getParameterItems() const { return mResource->mParameters; }

	private:
		OnsetDetectionComponent* mResource = nullptr;
		FFTAudioComponentInstance* mFFTAudioComponent = nullptr;

		std::vector<OnsetData> mOnsetList;

		FFTBuffer::AmplitudeSpectrum mPreviousBuffer;
		float mElapsedTime = 0.0f;
	};
}
