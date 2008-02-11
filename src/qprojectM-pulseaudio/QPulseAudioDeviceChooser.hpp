#ifndef QPULSEAUDIO_DEV_CHOOSER_HPP
#define QPULSEAUDIO_DEV_CHOOSER_HPP
#include "ui_PulseDeviceChooserDialog.h"
#include "QPulseAudioDeviceModel.hpp"
#include "QPulseAudioThread.hpp"

class QDialog;


class QPulseAudioDeviceChooser : public QDialog, public Ui::pulseDeviceChooserDialog {
	
	Q_OBJECT

	public:
		QPulseAudioDeviceChooser(QPulseAudioThread * pulseAudioThread, QWidget * parent, Qt::WindowFlags f=0);

	typedef QHash<int, QString> SourceContainer;
	public slots:
		void updateDevicesListViewLock(int state);
		void open();		
		void writeSettings();
	private slots:
		void readSettings();

	private:
		QPulseAudioDeviceModel _qpulseAudioDeviceModel;
		QPulseAudioThread * _qpulseAudioThread;
};
#endif
