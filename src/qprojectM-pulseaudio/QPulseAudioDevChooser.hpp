#ifndef QPULSEAUDIO_DEV_CHOOSER_HPP
#define QPULSEAUDIO_DEV_CHOOSER_HPP
#include "ui_PulseDeviceChooserDialog.h"
class QDialog;


class QPulseAudioDevChooser : public QDialog, public Ui::pulseDeviceChooserDialog {

	Q_OBJECT

	public:
	typedef QVector<QString> SourceContainer;
//		QPulseAudioDevChooser(QWidget * parent = 0, Qt::WindowFlags f = 0 ) ;
//QPulseAudioDevChooser::QPulseAudioDevChooser(QWidget * parent = 0, Qt::WindowFlags f = 0 ) : QDialog(parent, f) {}

	public slots:
		void updateDevice(int index, const QString & name);
		void removeDevice(int index);
		void refreshDevices(SourceContainer::const_iterator beginPos,SourceContainer::const_iterator endPos);

	private slots:
		void readSettings();
		void writeSettings();
};
#endif
