#include "QPlaylistModel.hpp"

QPlaylistModel::QPlaylistModel(projectM & _projectM, QObject * parent): 
		QAbstractTableModel(parent), m_projectM(_projectM) {
	for (int i = 0; i < _projectM.getPlaylistSize(); i++) {
		this->appendRow(QString(m_projectM.getPresetURL(i).c_str()), 
			QString(m_projectM.getPresetName(i).c_str()));
	}

}

QVariant QPlaylistModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const {
	
	return QVariant(QString(m_projectM.getPresetName(index.row()).c_str()));
}


int QPlaylistModel::rowCount ( const QModelIndex & parent = QModelIndex() ) const {
	return m_projectM.getPlaylistSize();
}


int QPlaylistModel::colCount ( const QModelIndex & parent = QModelIndex() ) const {

	// eventually add ratings here so size should be 2
	return 1;
}

void QPlaylistModel::appendRow (const QString & presetURL, const QString & presetName) {
	m_projectM.addPresetURL(presetURL.toStdString(), presetName.toStdString());
}
