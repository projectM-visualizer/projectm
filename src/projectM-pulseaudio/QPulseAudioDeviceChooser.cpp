/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include "QPulseAudioDeviceChooser.hpp"
#include <QSettings>
#include <QtDebug>
#include <QHBoxLayout>

void QPulseAudioDeviceChooser::writeSettings()
{

	QSettings settings ( "projectM", "qprojectM-pulseaudio" );
	settings.setValue ( "tryFirstAvailablePlaybackMonitor",
	                    this->tryFirstPlayBackMonitorCheckBox->checkState() == Qt::Checked );
			    
	if ( _qpulseAudioThread != 0 && (_qpulseAudioThread->devices().size() > 0) && _qpulseAudioThread->sourcePosition() != _qpulseAudioThread->devices().end())
		settings.setValue("pulseAudioDeviceName", *_qpulseAudioThread->sourcePosition());
	
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
		
	if (state == Qt::Checked) {
	    if (_qpulseAudioThread != 0)
		_qpulseAudioThread->connectDevice();
	}
}


QPulseAudioDeviceChooser::QPulseAudioDeviceChooser ( QPulseAudioThread * qpulseAudioThread, QWidget * parent = 0, Qt::WindowFlags f ) : QDialog ( parent, f ), _qpulseAudioDeviceModel ( qpulseAudioThread->devices(), qpulseAudioThread->sourcePosition(), this), _qpulseAudioThread ( qpulseAudioThread )
{

	setupUi ( this );
	readSettings();	
	this->devicesListView->setModel ( &_qpulseAudioDeviceModel );

		
	QHBoxLayout * hboxLayout = new QHBoxLayout();
	
	hboxLayout->addWidget(this->layoutWidget);
	this->setLayout(hboxLayout);
	
	connect ( tryFirstPlayBackMonitorCheckBox, 
		  SIGNAL(stateChanged(int)), this, SLOT(updateDevicesListViewLock(int)));

	// Using the display index is fine as as we are getting the text associated with it
	// and passing the raw string to pulse audio.
	connect ( devicesListView, SIGNAL ( doubleClicked ( const QModelIndex& ) ),
		  _qpulseAudioThread, SLOT ( connectDevice ( const QModelIndex& ) ) );
	
	connect ( _qpulseAudioThread, SIGNAL ( deviceChanged()),
		  &_qpulseAudioDeviceModel, SLOT (updateItemHighlights()));
	
}

void QPulseAudioDeviceChooser::open()
{
	this->show();
}

