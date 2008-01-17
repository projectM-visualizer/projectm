#include "QPulseAudioDeviceModel.hpp"
#include <QIcon>
#include <QtDebug>
#include <QMessageBox>

QPulseAudioDeviceModel::QPulseAudioDeviceModel(const QHash<int, QString> & _devices, QObject * parent = 0):
 devices(_devices) {}

void QPulseAudioDeviceModel::updateItemHighlights()
{
	if ( rowCount() == 0 )
		return;

	emit ( dataChanged ( this->index ( 0,0 ), this->index ( rowCount()-1) ) )
	;
}


QVariant QPulseAudioDeviceModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
{
     if (!index.isValid())
         return QVariant();
		
	if (index.row() >= rowCount())
		return QVariant();

	QHash<int, QString>::const_iterator pos;
	qDebug() << "role: " << role;
	switch ( role )
	{
		case Qt::DisplayRole:
			pos = devices.begin() + index.row();			
			return *pos;
		
		case Qt::ToolTip:
			return QString("These never show up.");
		case Qt::DecorationRole:				
				return QVariant();
		case Qt::BackgroundRole:
//				return Qt::red;
//				return Qt::green;
			return Qt::white;
		default:			
			return QVariant();
	}
}

int QPulseAudioDeviceModel::rowCount ( const QModelIndex & parent) const
{	
		return devices.count();
}



