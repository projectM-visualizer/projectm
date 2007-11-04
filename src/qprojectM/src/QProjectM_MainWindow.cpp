/***************************************************************************
 *   Copyright (C) 2007 by carm   *
 *   carm@localhost   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>
#include "QProjectM_MainWindow.hpp"
#include "QProjectMFileDialog.hpp"

#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include "QPlaylistModel.hpp"

QProjectM_MainWindow::QProjectM_MainWindow(const std::string & config_file)
:m_QProjectMFileDialog(new QProjectMFileDialog(this))
{

      ui.setupUi(this);

      m_QProjectMWidget = new QProjectMWidget(config_file, this);
	
      m_timer = new QTimer(this);
      connect(m_timer, SIGNAL(timeout()), m_QProjectMWidget, SLOT(updateGL()));

      connect(ui.lockPresetCheckBox, SIGNAL(stateChanged(int)),
	m_QProjectMWidget, SLOT(setPresetLock(int)));

      connect(ui.clearPresetList_PushButton, SIGNAL(pressed()),
	this, SLOT(clearPlaylist()));

      connect(m_QProjectMWidget, SIGNAL(projectM_Initialized()), this, SLOT(postProjectM_Initialize()));
      
	
      m_QProjectMWidget->makeCurrent();
      m_QProjectMWidget->setFocus();

      m_timer->start(0);

      setCentralWidget(m_QProjectMWidget);
	
      createActions();
      createMenus();
      createToolBars();
      createStatusBar();
      readSettings();
      ui.presetPlayListDockWidget->hide();
       

}

void QProjectM_MainWindow::clearPlaylist() {

	playlistModel->clear();
}

QProjectM * QProjectM_MainWindow::getQProjectM() {
	return m_QProjectMWidget->getQProjectM();
}

void QProjectM_MainWindow::updatePlaylistSelection(bool hardCut, unsigned int index) {
	
	if (hardCut)
		statusBar()->showMessage(tr("*** Hard cut ***") , 2000);
	else
		statusBar()->showMessage(tr("*** Soft cut ***") , 2000);

	//ui.tableView->selectRow(index);


}

void QProjectM_MainWindow::selectPlaylistItem(const QModelIndex & index) {
	getQProjectM()->selectPreset(index.row());
}


void QProjectM_MainWindow::postProjectM_Initialize() {

	playlistModel = new QPlaylistModel(*m_QProjectMWidget->getQProjectM(),this);
	refreshPlaylist();
  	
	ui.tableView->setModel(playlistModel);
        connect(m_QProjectMWidget->getQProjectM(), SIGNAL(presetSwitchedSignal(bool,unsigned int)), this, SLOT(updatePlaylistSelection(bool,unsigned int)));
	connect(ui.presetSearchBarLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateFilteredPlaylist(const QString&)));
	
        connect(ui.tableView, SIGNAL(activated(const QModelIndex &)), 
		this, SLOT(selectPlaylistItem(const QModelIndex &)));

	connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), 
		this, SLOT(changeRating(const QModelIndex &)));

}

void QProjectM_MainWindow::changeRating(const QModelIndex & index) {
	if (index.column() == 0)
		return;
	
	playlistModel->setData
		(index, (playlistModel->data(index, QPlaylistModel::RatingRole).toInt()+1) % 6, QPlaylistModel::RatingRole);
}

void QProjectM_MainWindow::keyReleaseEvent ( QKeyEvent * e )  {

	QModelIndex modelIndex;
	switch (e->key()) {
	case Qt::Key_L:
		if (ui.presetSearchBarLineEdit->hasFocus())
			return;

		if (ui.lockPresetCheckBox->checkState() == Qt::Checked)
			ui.lockPresetCheckBox->setCheckState(Qt::Unchecked);
		else
			ui.lockPresetCheckBox->setCheckState(Qt::Checked);
	
		// the projectM widget handles the actual lock
		//e->ignore();			
		//m_QProjectMWidget->keyReleaseEvent(e);
		return;

	case Qt::Key_F1:
		 //emit(keyPressed m_QProjectMWidget, 
	case Qt::Key_F:			
		if (ui.presetSearchBarLineEdit->hasFocus())
			return;
		this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
		return;

	case Qt::Key_M:
		if (ui.presetSearchBarLineEdit->hasFocus())
			return;
		
		if (ui.presetPlayListDockWidget->isVisible()) {
			ui.presetPlayListDockWidget->hide();
		} else
			ui.presetPlayListDockWidget->show();
	
		if (menuBar()->isVisible()) {
			menuBar()->hide();
			m_QProjectMWidget->setFocus();
		}
		else {
		   menuBar()->show();
		}

		if (statusBar()->isVisible()) {
			statusBar()->hide();
		}
		else {
		   statusBar()->show();
		}
		return;	

	case Qt::Key_R:
		if (ui.presetSearchBarLineEdit->hasFocus())
			return;

		//modelIndex.selectRandom()
		//modelIndex = QModelIndex(0,0,0);
		//selectPlaylistItem(modelIndex);
		//updatePlaylistSelection(true, modelIndex.row());
		return;
	default:
		//m_QProjectMWidget->keyReleaseEvent(e);
		break;//e->ignore();	
	}
	
}

void QProjectM_MainWindow::closeEvent(QCloseEvent *event)
{
}


void QProjectM_MainWindow::open()
{


            if ( m_QProjectMFileDialog->exec()) {
		const QStringList & files = m_QProjectMFileDialog->selectedFiles();

		for (QStringList::const_iterator pos = files.begin(); 
			pos != files.end(); ++pos) {
			if (*pos != "")
				loadFile(*pos);
		}
	    }
	
	//playlistModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);
}

//void QAbstractItemView::clicked ( const QModelIndex & index )   [signal]
void QProjectM_MainWindow::refreshPlaylist() {

    StringPairVector * stringPairs = new StringPairVector();
    for (int i = 0; i < playlistModel->rowCount();i++) {
	QModelIndex index = playlistModel->index(i, 0);
	const QString & name = playlistModel->data(index, Qt::DisplayRole).toString();
	const QString & url = playlistModel->data(index, QPlaylistModel::URLInfoRole).toString();
			
	stringPairs->push_back(StringPair(url, name));
    }
    historyHash.insert(QString(), stringPairs);

    
    QHeaderView * hHeader = new QHeaderView(Qt::Horizontal, this);
    QHeaderView * vHeader = new QHeaderView(Qt::Vertical, this);

    hHeader->setClickable(false);
    hHeader->setSortIndicatorShown(false);
    //hHeader->setSortIndicator(1, Qt::AscendingOrder);
    hHeader->setStretchLastSection(false);
	
	ui.tableView->setVerticalHeader(vHeader);	
	ui.tableView->setHorizontalHeader(hHeader);
	
	hHeader->resizeSection(0, 200);
	hHeader->setResizeMode(0, QHeaderView::Stretch);
	hHeader->setResizeMode(1, QHeaderView::Fixed);
	hHeader->resizeSection(1, 25);


//	playlistModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);

	vHeader->hide();
//	playlistModel->setHeaderData(0, Qt::Horizontal, 200, Qt::SizeHintRole);
	//playlistModel->setHeaderData(1, Qt::Horizontal, tr("Rating"));//, Qt::DisplayRole);
	//playlistModel->setHeaderData(2, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);
	
	
}

void QProjectM_MainWindow::about()
{
      QMessageBox::about(this, tr("About QProjectM"),
            tr("<b>QProjectM</b> provides useful gui extensions to the projectM core library."));
}


void QProjectM_MainWindow::createActions()
{

      connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

      connect(ui.actionAddPresets, SIGNAL(triggered()), this, SLOT(open()));

      connect(ui.actionAbout_qprojectM, SIGNAL(triggered()), this, SLOT(about()));

      //connect(ui.actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));

}

void QProjectM_MainWindow::createMenus()
{
	ui.menuBar->hide();

}

void QProjectM_MainWindow::createToolBars()
{ 
}

void QProjectM_MainWindow::createStatusBar()
{
      statusBar()->hide();
      statusBar()->showMessage(tr("Welcome to qprojectM!"));
}

void QProjectM_MainWindow::readSettings()
{
      QSettings settings("Trolltech", "QProjectM");
      QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
      QSize size = settings.value("size", QSize(1000, 600)).toSize();
      resize(size);
      move(pos);
}

void QProjectM_MainWindow::writeSettings()
{
      QSettings settings("Trolltech", "QProjectM");
      settings.setValue("pos", pos());
      settings.setValue("size", size());
}



void QProjectM_MainWindow::loadFile(const QString &fileName)
{

	const QString & name = strippedName(fileName);
		
	//if (name.contains(previousFilter, Qt::CaseInsensitive))
		playlistModel->appendRow(fileName, name);
	//else
	//	exclusionHash.value(
	
}


QString QProjectM_MainWindow::strippedName(const QString &fullFileName)
{
      return QFileInfo(fullFileName).fileName();
}


void QProjectM_MainWindow::updateFilteredPlaylist(const QString & text) {
	
	const QString filter = text.toLower();

	playlistModel->clear();
	
	if (historyHash.contains(filter)) {
		const StringPairVector & stringPairs = *historyHash.value(filter);
		for (StringPairVector::const_iterator pos = stringPairs.begin(); pos != stringPairs.end();++pos) {
			playlistModel->appendRow(pos->first, pos->second);
		}
	} else {
		const StringPairVector & stringPairs = *historyHash.value(QString());

		StringPairVector * stringPairs2 = new StringPairVector();
		for (StringPairVector::const_iterator pos = stringPairs.begin(); pos != stringPairs.end();++pos) {
			if ((pos->first).contains(filter, Qt::CaseInsensitive)) {
				playlistModel->appendRow(pos->first, pos->second);
				stringPairs2->push_back(StringPair(pos->first,pos->second));
			}			
		}
		historyHash.insert(filter, stringPairs2);
	}
	#if 0
	if (!(filter.substring(0, filter.length()-1) == previousFilter)) {

		StringPairVector & stringPairs = *exclusionHash.value(previousFilter);
		while (!stringPairs.empty()) {
			const StringPair & pair = stringPairs.last();
			/// @bug need to do **ordered** insert, or something faster (index reference book keeping?)
			playlistModel->appendRow(pair.first, pair.second);
			stringPairs.pop_back();
			
		}
		exclusionHash.remove(previousFilter);
		delete(&stringPairs);
		
	} else {
		assert(filter.length() != previousFilter.length());
		StringPairVector * stringPairs = new StringPairVector();
		exclusionHash.insert(filter, stringPairs);
		
		for (int i = 0; i < playlistModel->rowCount(); i++) {
			QModelIndex index = playlistModel->index(i, 0);
			const QString & name = playlistModel->data(index, Qt::DisplayRole).toString();
			const QString & url = playlistModel->data(index, QPlaylistModel::URLInfoRole).toString();
			if (!name.contains(filter, Qt::CaseInsensitive)) {
					
				stringPairs->push_back(StringPair(url, name));
				assert (i < playlistModel->rowCount());
				assert (i >= 0);
				playlistModel->removeRow(i);
				i--;
			}
			
		}
		
	}
#endif
}

