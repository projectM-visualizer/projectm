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
		
	QHash<int, QString>::const_iterator pos;
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
/*
QVariant QPulseAudioDeviceModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{

	return QAbstractItemModel::headerData ( section, orientation, role );
}
*/
int QPulseAudioDeviceModel::rowCount ( const QModelIndex & parent ) const
{

	return devices.size();
}


int QPulseAudioDeviceModel::columnCount ( const QModelIndex & parent ) const
{

	// eventually add ratings here so size should be 2
	if ( rowCount() > 0 )
		return 2;
	else
		return 0;
}

void QPulseAudioDeviceModel::appendRow ( const QString & name, int id)
{
	beginInsertRows ( QModelIndex(), rowCount(), rowCount() );
//	devices.insert(id, name);
	endInsertRows();
}


void QPulseAudioDeviceModel::removeRow ( int index )
{
	beginRemoveRows ( QModelIndex(), index, index );
//	QPulseAudioThread::SourceContainer::iterator pos
//7		= devices.begin() + index;
//	devices.remove(pos.key());
	endRemoveRows();
}



