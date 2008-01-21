#include "QPulseAudioDeviceModel.hpp"
#include <QIcon>
#include <QtDebug>
#include <QMessageBox>

QPulseAudioDeviceModel::QPulseAudioDeviceModel(const QHash<int, QString> & _devices, QObject * parent = 0):
 devices(_devices) {}

QVariant QPulseAudioDeviceModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
{
	if (!index.isValid())
         return QVariant();

	if (index.row() >= rowCount())
		return QVariant();

	QHash<int, QString>::const_iterator pos;
	switch ( role )
	{
		case Qt::DisplayRole:
			pos = devices.begin() + index.row();			
			return *pos;
		case Qt::DecorationRole:
		{
			QIcon icon(":/check.png");
			return icon;		
			break;
		
		}
		case Qt::BackgroundRole:
									
		default:			
			
			return QVariant();
	}
}

int QPulseAudioDeviceModel::rowCount ( const QModelIndex & parent) const
{
		
		return devices.count();
}



