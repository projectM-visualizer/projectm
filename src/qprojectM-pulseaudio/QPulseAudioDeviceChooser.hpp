#ifndef QPULSEAUDIO_DEV_CHOOSER_HPP
#define QPULSEAUDIO_DEV_CHOOSER_HPP
#include "ui_PulseDeviceChooserDialog.h"
#include "QPulseAudioDeviceModel.hpp"
class QDialog;


class QPulseAudioDeviceChooser : public QDialog, public Ui::pulseDeviceChooserDialog {

	Q_OBJECT

	public:
		QPulseAudioDeviceChooser(const QHash<int, QString> & devices, QWidget * parent, Qt::WindowFlags f=0);

	typedef QHash<int, QString> SourceContainer;
//		QPulseAudioDeviceChooser(QWidget * parent = 0, Qt::WindowFlags f = 0 ) ;

	public slots:
		void open();		
	private slots:
		void readSettings();
		void writeSettings();
	private:
		QPulseAudioDeviceModel m_qpulseAudioDeviceModel;
};
#endif
