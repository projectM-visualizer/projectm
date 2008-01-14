#include "QPulseAudioDevChooser.hpp"
#include <QSettings>

void QPulseAudioDevChooser::writeSettings() {
	
	QSettings settings ( "projectM", "qprojectM-pulseaudio");
	settings.setValue ( "tryFirstAvailablePlaybackMonitor", 
		this->tryFirstPlayBackMonitorCheckBox->checkState() == Qt::Checked);	
	
}


void QPulseAudioDevChooser::readSettings() {
	
QSettings settings ( "projectM", "qprojectM-pulseaudio");

this->tryFirstPlayBackMonitorCheckBox->setCheckState
	(settings.value("tryFirstAvailablePlaybackMonitor", true).toBool() ? Qt::Checked : Qt::Unchecked);


}

void QPulseAudioDevChooser::updateDevice(int index, const QString & name) {
	this->deviceListWidget->item(index)->setText(name);
}

void QPulseAudioDevChooser::removeDevice(int index) {
		
this->deviceListWidget->clear();
}


void QPulseAudioDevChooser::refreshDevices
	(SourceContainer::const_iterator beginPos, SourceContainer::const_iterator endPos) {

	SourceContainer::const_iterator pos = beginPos;

	this->deviceListWidget->clear();

	while (pos != endPos) {
		this->deviceListWidget->addItem(*pos);
		++pos;
	}
}
