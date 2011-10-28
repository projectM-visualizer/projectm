/**
 * projectM-qt -- Qt4 based projectM GUI 
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

#ifndef QPROJECTM_PLAYLISTMODEL_H
#define QPROJECTM_PLAYLISTMODEL_H

#include <cassert>
#include <QAbstractTableModel>
#include <QVector>

#include <projectM.hpp>
#include <QStringList>
#include <QtDebug>

class QXmlStreamReader;
class QDragMoveEvent;

class QPlaylistModel : public QAbstractTableModel
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

public:
static const int URLInfoRole = Qt::UserRole;
static const int RatingRole = Qt::UserRole+1;
static const int NameRole = Qt::UserRole+2;
static const int BreedabilityRole = Qt::UserRole+3;

     QPlaylistModel(projectM & _projectM, QObject * parent = 0);
     ~QPlaylistModel() { }
bool setData(const QModelIndex & index, const QVariant & value, int role=Qt::EditRole);

void appendRow (const QString & presetURL, const QString & presetName, int rating, int breedability);
void insertRow (int index, const QString & presetURL, const QString & presetName, int rating, int breedability);

bool removeRow (int index, const QModelIndex & parent = QModelIndex());
bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex());
QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

void clear();

QVariant data ( const QModelIndex & index, int role) const;
int rowCount ( const QModelIndex & parent = QModelIndex()) const ;
int columnCount ( const QModelIndex & parent= QModelIndex()) const ;

bool readPlaylist(const QString & file);
bool writePlaylist ( const QString & file );
QVariant breedabilityToIcon( int rating )  const;
QString getBreedabilityToolTip(int rating) const;
Qt::ItemFlags flags(const QModelIndex &index) const;


inline const QString & playlistDesc() {
 	return m_playlistDesc;
}


inline void setPlaylistDesc(const QString & desc) {
	m_playlistDesc = desc;
}

inline Qt::DropActions supportedDropActions() const
{
	return Qt::MoveAction;
}

void notifyDataChanged(unsigned int index);

inline QStringList mimeTypes () const  {
	QStringList mimeTypes;
	mimeTypes << PRESET_MIME_TYPE;
	return mimeTypes;
}
		
bool dropMimeData(const QMimeData *data, Qt::DropAction action,
				 int row, int column, const QModelIndex &parent);
void clearItems();

public slots:
	void updateItemHighlights();
	
	 private:
	static QString getSillyRatingToolTip(int rating);
	void readPlaylistItem(QXmlStreamReader & reader);
	static QString PRESET_MIME_TYPE;
	QVariant ratingToIcon(int rating) const;
	projectM & m_projectM;
	QString m_playlistName;
	QString m_playlistDesc;

private:
bool softCutRatingsEnabled() const;
};
#endif
