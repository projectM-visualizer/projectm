#include "QPlaylistModel.hpp"

QPlaylistModel::QPlaylistModel(projectM & _projectM, QObject * parent): 
		QAbstractTableModel(parent), m_projectM(_projectM) {


}

QVariant QPlaylistModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const {
	
	if (role == Qt::DisplayRole)
		return QVariant(QString(m_projectM.getPresetName(index.row()).c_str()));
	else
		return QVariant();
}

QVariant QPlaylistModel::headerData ( int section, Qt::Orientation orientation, int role) const {

	if ((section == 0) && (role == Qt::DisplayRole))
		return QString(tr("Preset"));
	else
		return QAbstractTableModel::headerData(section, orientation, role);
}

int QPlaylistModel::rowCount ( const QModelIndex & parent = QModelIndex() ) const {
	return m_projectM.getPlaylistSize();
}


int QPlaylistModel::columnCount ( const QModelIndex & parent = QModelIndex() ) const {

	// eventually add ratings here so size should be 2
	if (rowCount() > 0)
		return 1;
	else
		return 0;
}

void QPlaylistModel::appendRow (const QString & presetURL, const QString & presetName) {
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_projectM.addPresetURL(presetURL.toStdString(), presetName.toStdString());
	endInsertRows();
}


void QPlaylistModel::removeRow (int index) {
	beginRemoveRows(QModelIndex(), index, index);
	m_projectM.removePreset(index);
	endRemoveRows();
}

void QPlaylistModel::clear() {
	beginRemoveRows(QModelIndex(), 0, rowCount()-1);
	m_projectM.clearPlaylist();
	endRemoveRows();
}
