#include "QPulseAudioDeviceModel.hpp"
#include <QIcon>
#include <QtDebug>
#include <QMessageBox>

QPulseAudioDeviceModel::QPulseAudioDeviceModel(const QHash<int, QString> & _devices, QObject * parent = 0):QAbstractItemModel(parent), devices(_devices) {


}

QModelIndex QPulseAudioDeviceModel::index(int row, int col, const QModelIndex & parent = QModelIndex()) const {
	return this->createIndex(row,col);
}

QModelIndex QPulseAudioDeviceModel::parent(const QModelIndex & parent) const {

	qDebug() << parent.row();
	return parent.parent();
}

void QPulseAudioDeviceModel::updateItemHighlights()
{
	if ( rowCount() == 0 )
		return;

	emit ( dataChanged ( this->index ( 0,0 ), this->index ( rowCount()-1,columnCount()-1 ) ) )
	;
}


QVariant QPulseAudioDeviceModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
{
		
	if (index.column() > 0)
		return QVariant();
	
	QHash<int, QString>::const_iterator pos;
	qDebug() << "role: " << role;
	switch ( role )
	{
		case Qt::DisplayRole:
			qDebug() << "role2: " << role;
			abort();		
			pos = devices.begin() + index.row();						
			return *pos;
		
		case Qt::ToolTip:
			return QString("These never show up.");
		case Qt::DecorationRole:
				return QVariant();
//		case Qt::BackgroundRole:
//				return Qt::red;
//				return Qt::green;

//			return Qt::white;
		default:
			return QVariant();
	}
}

int QPulseAudioDeviceModel::rowCount ( const QModelIndex & parent ) const
{

	return devices.size();
}


int QPulseAudioDeviceModel::columnCount ( const QModelIndex & parent ) const
{
	if ( rowCount() > 0 )
		return 1;
	else
		return 0;
}



