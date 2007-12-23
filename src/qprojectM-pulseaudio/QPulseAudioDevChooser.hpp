#ifndef QPULSEAUDIO_DEV_CHOOSER_HPP
#define QPULSEAUDIO_DEV_CHOOSER_HPP
#include "ui_PulseDeviceChooserDialog.h"
class QDialog;

class QPulseAudioDevChooser : public QDialog, public Ui::pulseDeviceChooserDialog {

	Q_OBJECT

	public:
//		QPulseAudioDevChooser(QWidget * parent = 0, Qt::WindowFlags f = 0 ) ;
//QPulseAudioDevChooser::QPulseAudioDevChooser(QWidget * parent = 0, Qt::WindowFlags f = 0 ) : QDialog(parent, f) {}

	public slots:
		//void updateDevice(int index, QString name);
		//void removeDevice(int index);
		//void refreshDevices();

	private slots:
		void saveConfig();




};
#endif
