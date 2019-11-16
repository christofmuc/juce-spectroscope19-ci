/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#include "ExampleWidget.h"

ExampleWidget::ExampleWidget() :
	audioIOSelector_(deviceManager, 1, 2, 0, 0, false, false, true, true),
	spectrogram_([this]() {
	MessageManager::callAsync([this]() {
		if (spectroscope2D_) spectroscope2D_->refreshData();
	});
})
{
	formatManager_.registerBasicFormats();
	setAudioChannels(2, 2);

	showIOSelectorButton_.setButtonText("IO");
	showIOSelectorButton_.addListener(this);
	addAndMakeVisible(&showIOSelectorButton_);

	startStopButton_.setButtonText("Start/Stop");
	startStopButton_.addListener(this);
	addAndMakeVisible(startStopButton_);

	logXButton_.setButtonText("Log X");
	logXButton_.addListener(this);
	logXButton_.setClickingTogglesState(true);
	logXButton_.setToggleState(true, dontSendNotification);
	addAndMakeVisible(logXButton_);

	// These two share the same space, I just toggle between visibility of them
	addChildComponent(audioIOSelector_);
	audioIOSelector_.setVisible(true);
}

ExampleWidget::~ExampleWidget()
{
	shutdownAudio();
}

void ExampleWidget::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	audioTransportSource_.prepareToPlay(samplesPerBlockExpected, sampleRate);

	spectroscope2D_ = std::make_unique<SpectogramWidget>(spectrogram_);
	addChildComponent(spectroscope2D_.get());
}

void ExampleWidget::releaseResources()
{
	if (spectroscope2D_) {
		spectroscope2D_->setContinuousRedrawing(false);
		removeChildComponent(spectroscope2D_.get());
		spectroscope2D_.reset();
	}
	audioTransportSource_.releaseResources();
}

void ExampleWidget::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	// Write to Ring Buffer
	spectrogram_.newData(bufferToFill);
}

void ExampleWidget::resized()
{
	auto area = getLocalBounds();
	auto buttonRow = area.removeFromBottom(50).reduced(10);
	startStopButton_.setBounds(buttonRow.removeFromLeft(80));
	logXButton_.setBounds(buttonRow.removeFromLeft(90).withTrimmedLeft(10));
	showIOSelectorButton_.setBounds(area.removeFromRight(50).reduced(10));
	area = area.reduced(10);

	audioIOSelector_.setBounds(area);
	if (spectroscope2D_) spectroscope2D_->setBounds(area.reduced(8));
}

void ExampleWidget::buttonClicked(Button* button)
{
	if (button == &showIOSelectorButton_) {
		showIOSelectorButtonClicked();
	}
	if (button == &startStopButton_ && spectroscope2D_) {
		spectroscope2D_->setContinuousRedrawing(!spectroscope2D_->isRunning());
	}
	else if (button == &logXButton_ && spectroscope2D_) {
		spectroscope2D_->setXAxis(button->getToggleState());
	}
}

void ExampleWidget::showIOSelectorButtonClicked()
{
	// Toggle button switches between Audio IO configuration and oscilloscope
	bool audioIOShouldBeVisibile = !audioIOSelector_.isVisible();
	audioIOSelector_.setVisible(audioIOShouldBeVisibile);

	if (spectroscope2D_) {
		if (audioIOShouldBeVisibile)
		{
			spectroscope2D_->setVisible(false);
			spectroscope2D_->setContinuousRedrawing(false);
			resized();
		}
		else
		{
			spectroscope2D_->setVisible(true);
			spectroscope2D_->setContinuousRedrawing(true);
			resized();
		}
	}
}
