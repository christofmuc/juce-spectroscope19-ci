/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "JuceHeader.h"

#include "Spectrogram.h"
#include "SpectrogramWidget.h"

class ExampleWidget : public AudioAppComponent, public Button::Listener
{
public:
	ExampleWidget();

	~ExampleWidget();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void releaseResources() override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

	void resized() override;

	void buttonClicked(Button* button) override;

private:
	void showIOSelectorButtonClicked();

	AudioFormatManager formatManager_;
	AudioTransportSource audioTransportSource_;

	// Subcomponents
	TextButton showIOSelectorButton_;
	TextButton startStopButton_;
	ToggleButton logXButton_;
	AudioDeviceSelectorComponent audioIOSelector_;

	std::unique_ptr<SpectogramWidget> spectroscope2D_;
	Spectrogram spectrogram_;
};

