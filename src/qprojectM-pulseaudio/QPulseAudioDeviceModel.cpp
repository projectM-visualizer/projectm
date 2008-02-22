#include "QPulseAudioDeviceModel.hpp"
#include <QIcon>
#include <QtDebug>
#include <QMessageBox>
#include <QHash>

     QPulseAudioDeviceModel::QPulseAudioDeviceModel(const QHash<int, QString> & devices, const QHash<int,QString>::const_iterator & devicePosition, QObject * parent): _devices(devices), _devicePosition(devicePosition) {
	
     }
     
     void QPulseAudioDeviceModel::updateItemHighlights()
     {
	     if ( rowCount() == 0 )
		     return;

	     emit ( dataChanged ( this->index (0), this->index ( rowCount()-1 ) ));			     
     }

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
			pos = _devices.begin() + index.row();			
			return *pos;
		case Qt::DecorationRole:
		{
			QIcon icon(":/check.png");
			return icon;		
			break;
		
		}
		
		case Qt::ToolTipRole:
			pos = _devices.begin() + index.row();
			if (_devicePosition == _devices.begin() + index.row())
 				return *pos + " (active)";			
			else
				return *pos + " (inactive)";
						
			
		case Qt::BackgroundRole:
			if (_devicePosition == _devices.begin() + index.row()) {
				return Qt::green;										
			} else {
				return Qt::white;
			}						
		default:			
			
			return QVariant();
	}
}

int QPulseAudioDeviceModel::rowCount ( const QModelIndex & parent) const
{
		
		return _devices.count();
}



