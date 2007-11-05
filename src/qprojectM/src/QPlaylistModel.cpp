#include "QPlaylistModel.hpp"
#include <QIcon>
#include <QXmlStreamReader>
#include <QtDebug>
#include <QFileInfo>

QPlaylistModel::QPlaylistModel ( projectM & _projectM, QObject * parent ) :
		QAbstractTableModel ( parent ), m_projectM ( _projectM )
{
	m_ratings = QVector<int> ( rowCount(), 3 );
}


void QPlaylistModel::updateItemHighlights()
{
	if ( rowCount() == 0 )
		return;

	emit ( dataChanged ( this->index ( 0,0 ), this->index ( rowCount()-1,columnCount()-1 ) ) )
	;
}

bool QPlaylistModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
	if ( role == QPlaylistModel::RatingRole )
	{
		//QAbstractTableModel::setData(index, ratingToIcon(value.toInt()), Qt::DecorationRole);
		//std::cerr << "here" << std::endl;
		m_ratings[index.row() ] = value.toInt();
		emit ( dataChanged ( index, index ) );
		return true;
	}
	else
		return QAbstractTableModel::setData ( index, value, role );

}

QVariant QPlaylistModel::ratingToIcon ( int rating )  const
{
	switch ( rating )
	{
		case 0:
			return QVariant ( QIcon ( ":/images/icons/face0.png" ) );
		case 1:
			return QVariant ( QIcon ( ":/images/icons/face1.png" ) );
		case 2:
			return QVariant ( QIcon ( ":/images/icons/face2.png" ) );
		case 3:
			return QVariant ( QIcon ( ":/images/icons/face3.png" ) );
		case 4:
			return QVariant ( QIcon ( ":/images/icons/face4.png" ) );
		case 5:
			return QVariant ( QIcon ( ":/images/icons/face5.png" ) );
		default:
			return QVariant ( QIcon ( ":/images/icons/face0.png" ) );
	}
}

QVariant QPlaylistModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
{

	switch ( role )
	{
		case Qt::DisplayRole:
			if ( index.column() == 0 )
				return QVariant ( QString ( m_projectM.getPresetName ( index.row() ).c_str() ) );
			else
				return ratingToIcon ( m_ratings[index.row() ] );
		case Qt::ToolTip:
			if ( index.column() == 0 )
				return QVariant ( QString ( m_projectM.getPresetName ( index.row() ).c_str() ) );
			else
				return QString ( "Current rating is %1 / 5" ).arg ( m_ratings[index.row() ] );
		case Qt::DecorationRole:
			if ( index.column() == 1 )
				return ratingToIcon ( m_ratings[index.row() ] );
			else
				return QVariant();
		case QPlaylistModel::RatingRole:
			return QVariant ( m_ratings[index.row() ] );

		case Qt::BackgroundRole:

			if ( m_projectM.isPresetLocked() && ( index.row() == m_projectM.selectedPresetIndex() ) )
				return Qt::red;
			if ( !m_projectM.isPresetLocked() && ( index.row() == m_projectM.selectedPresetIndex() ) )
				return Qt::green;

			return Qt::white;

		case QPlaylistModel::URLInfoRole:
			return QVariant ( QString ( m_projectM.getPresetURL ( index.row() ).c_str() ) );
		default:
			return QVariant();
	}
}

QVariant QPlaylistModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{

	if ( orientation == Qt::Vertical )
		return QAbstractTableModel::headerData ( section, orientation, role );

	if ( ( section == 0 ) && ( role == Qt::SizeHintRole ) )
		return QVariant ( 500 );
//	 if ((section == 1) && (role == Qt::SizeHintRole))
//		return QVariant(60);
	if ( ( section == 0 ) && ( role == Qt::DisplayRole ) )
		return QString ( tr ( "Preset" ) );
	if ( ( section == 1 ) && ( role == Qt::DisplayRole ) )
		return QString ( tr ( "Rating" ) );

	return QAbstractTableModel::headerData ( section, orientation, role );
}

int QPlaylistModel::rowCount ( const QModelIndex & parent ) const
{
	return m_projectM.getPlaylistSize();
}


int QPlaylistModel::columnCount ( const QModelIndex & parent ) const
{

	// eventually add ratings here so size should be 2
	if ( rowCount() > 0 )
		return 2;
	else
		return 0;
}

void QPlaylistModel::appendRow ( const QString & presetURL, const QString & presetName, int rating )
{
	beginInsertRows ( QModelIndex(), rowCount(), rowCount() );
	m_projectM.addPresetURL ( presetURL.toStdString(), presetName.toStdString() );
	m_ratings.push_back ( rating );
	endInsertRows();
}


void QPlaylistModel::removeRow ( int index )
{
	beginRemoveRows ( QModelIndex(), index, index );
	m_projectM.removePreset ( index );
	m_ratings.remove ( index );
	endRemoveRows();
}

void QPlaylistModel::clear()
{
	beginRemoveRows ( QModelIndex(), 0, rowCount()-1 );
	m_projectM.clearPlaylist();
	m_ratings.clear();
	endRemoveRows();
}

void QPlaylistModel::readPlaylist ( const QString & file )
{

	QXmlStreamReader reader ( file );

	QXmlStreamReader::TokenType token;

	while ( !reader.atEnd() ) {
		token = reader.readNext();
	switch ( token )
	{
		case QXmlStreamReader::StartElement:
			qDebug() << "start elem:" << reader.name().toString();
			if (reader.name() == "item") {
				readPlaylistItem(reader);
				break;
			}

		case QXmlStreamReader::NoToken:
		case QXmlStreamReader::Invalid:
		case QXmlStreamReader::StartDocument:
		case QXmlStreamReader::EndDocument:
		case QXmlStreamReader::EndElement:
		case QXmlStreamReader::Characters:
		case QXmlStreamReader::Comment:
		case QXmlStreamReader::DTD:
		case QXmlStreamReader::EntityReference:
		case QXmlStreamReader::ProcessingInstruction:
		default:
			break;
	}
	}
	if (reader.hasError()) {
		qDebug() << "error occurred: " << reader.error();
		exit(0);
	}
}

void QPlaylistModel::readPlaylistItem(QXmlStreamReader & reader) {

	
	QString url;
	int rating;

	while (reader.readNext() != QXmlStreamReader::EndElement)
		if (reader.name() == "url") {
			reader.readNext();
			url = reader.text().toString();
			reader.readNext();
		} else if (reader.name() == "rating") {
			reader.readNext();
			rating = reader.text().toString().toInt();
		}
		else {
			/// throw a stream exception
			qDebug() << (reader.name().toString());
			throw 0;
		}
	this->appendRow(url, QFileInfo(url).fileName(), rating);
}	
