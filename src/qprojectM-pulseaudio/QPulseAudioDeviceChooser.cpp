#include "QPulseAudioDeviceChooser.hpp"
#include <QSettings>
#include <QtDebug>

void QPulseAudioDeviceChooser::writeSettings()
{

	QSettings settings ( "projectM", "qprojectM-pulseaudio" );
	settings.setValue ( "tryFirstAvailablePlaybackMonitor",
	                    this->tryFirstPlayBackMonitorCheckBox->checkState() == Qt::Checked );
	//settings.setValue ("pulseAudioSourceDevice", this->listView.data(
}


void QPulseAudioDeviceChooser::readSettings()
{

	QSettings settings ( "projectM", "qprojectM-pulseaudio" );

	bool tryFirst = settings.value
	                ( "tryFirstAvailablePlaybackMonitor", true ).toBool() ;

	this->tryFirstPlayBackMonitorCheckBox->setCheckState
	( tryFirst ? Qt::Checked : Qt::Unchecked );

	if ( tryFirst )
	{	
		this->devicesListView->setEnabled(false);
	} else {
		this->devicesListView->setEnabled(true);
	}
	

}


void QPulseAudioDeviceChooser::updateDevicesListViewLock(int state) {	
		devicesListView->setEnabled(state != Qt::Checked);
	
}


QPulseAudioDeviceChooser::QPulseAudioDeviceChooser ( QPulseAudioThread * qpulseAudioThread, QWidget * parent = 0, Qt::WindowFlags f ) : QDialog ( parent, f ), _qpulseAudioDeviceModel ( qpulseAudioThread->devices(), this ), _qpulseAudioThread ( qpulseAudioThread )
{

	setupUi ( this );
	readSettings();
	qDebug() << "setting model";
	this->devicesListView->setModel ( &_qpulseAudioDeviceModel );

// void QAbstractItemView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )   [virtual protected slot]

	connect ( tryFirstPlayBackMonitorCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateDevicesListViewLock(int)));

	/// @bug wrong! should be based on HASH index, not display index
	connect ( devicesListView, SIGNAL ( doubleClicked ( const QModelIndex& ) ), _qpulseAudioThread, SLOT ( connectDevice ( const QModelIndex& ) ) );
	//connect(buttonBox, SIGNAL(accepted()), _qpulseAudioThread, SLOT(connectDevice(device));
}

void QPulseAudioDeviceChooser::open()
{

	this->show();

}

