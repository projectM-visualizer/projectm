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


#ifndef QPULSEAUDIODEVICEMODEL_HPP
#define QPULSEAUDIODEVICEMODEL_HPP

#include <cassert>
#include "projectM.hpp"
#include <QAbstractListModel>
#include <QHash>

 class QPulseAudioDeviceModel : public QAbstractListModel
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

	 public slots:
		 void updateItemHighlights();

 public:
     QPulseAudioDeviceModel(const QHash<int, QString> & devices, const QHash<int,QString>::const_iterator & devicePosition , QObject * parent);
     ~QPulseAudioDeviceModel() { }
     
     

QVariant data ( const QModelIndex & index, int role) const;
int rowCount ( const QModelIndex & parent = QModelIndex()) const ;

 private:
	const QHash<int, QString> & _devices;
 	const QHash<int, QString>::const_iterator & _devicePosition;
};
#endif
