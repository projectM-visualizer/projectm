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
#include "qprojectm_mainwindow.hpp"
#include "qpresetfiledialog.hpp"
#include "qplaylistfiledialog.hpp"
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include "qplaylistmodel.hpp"
#include "ui_qprojectm_mainwindow.h"
#include "qprojectmconfigdialog.hpp"

#include "ConfigFile.h"
#include "qxmlplaylisthandler.hpp"
#include "nullable.hpp"
class PlaylistWriteFunctor {
	public:
		PlaylistWriteFunctor(const QProjectM_MainWindow::PlaylistItemVector::iterator & begin,
			const QProjectM_MainWindow::PlaylistItemVector::iterator & end, const QString & desc) : 
		 m_pos(begin), m_end(end), m_desc(desc) {}

		inline const QString & playlistDesc() const {
			return m_desc;
		}

		inline bool nextItem(QString & url, int & rating) {

			if (m_pos == m_end)
				return false;

			QProjectM_MainWindow::PlaylistItemMetaData data = *m_pos;
			
			url = data.url;
			rating = data.rating;
			m_pos++;
			return true;
		}
	private:
		QProjectM_MainWindow::PlaylistItemVector::const_iterator m_pos;
		const QProjectM_MainWindow::PlaylistItemVector::const_iterator m_end;
		const QString & m_desc;
		

};

QProjectM_MainWindow::QProjectM_MainWindow ( const std::string & config_file, QMutex * audioMutex)
		:m_QPresetFileDialog ( new QPresetFileDialog ( this ) ), m_QPlaylistFileDialog 
		( new QPlaylistFileDialog ( this )), playlistModel(0), 
		configDialog(0), hHeader(0), vHeader(0), _menuVisible(true), activePresetIndex(new Nullable<long>), playlistItemCounter(0)
{

	
	ui = new Ui::QProjectM_MainWindow();	
	ui->setupUi ( this );
	
	m_QProjectMWidget = new QProjectMWidget ( config_file, this, audioMutex);

	m_timer = new QTimer ( this );
	connect ( m_timer, SIGNAL ( timeout() ), m_QProjectMWidget, SLOT ( updateGL() ) );

	connect ( ui->lockPresetCheckBox, SIGNAL ( stateChanged ( int ) ),
	          m_QProjectMWidget, SLOT ( setPresetLock ( int ) ) );
	
	connect ( ui->shuffleEnabledCheckBox, SIGNAL ( stateChanged ( int ) ),
		  m_QProjectMWidget, SLOT ( setShuffleEnabled ( int ) ) );

	connect ( ui->clearPresetList_PushButton, SIGNAL ( pressed() ),
	          this, SLOT ( clearPlaylist() ) );

	connect(ui->tableView, SIGNAL(resized(QResizeEvent *)), this, SLOT(refreshHeaders(QResizeEvent*)));
	
	connect ( m_QProjectMWidget, SIGNAL ( projectM_Initialized(QProjectM*) ), 
		  this, SLOT ( postProjectM_Initialize() ) );
	
	m_QProjectMWidget->makeCurrent();
	m_QProjectMWidget->setFocus();

	m_timer->start ( 0 );

	setCentralWidget ( m_QProjectMWidget );
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	readSettings();
	
	ui->presetPlayListDockWidget->hide();

	connect ( ui->tableView, SIGNAL ( activated ( const QModelIndex & ) ),
		  this, SLOT ( selectPlaylistItem ( const QModelIndex & ) ) );
	connect ( ui->tableView, SIGNAL ( clicked ( const QModelIndex & ) ),
		  this, SLOT ( changeRating ( const QModelIndex & ) ) );
	connect ( ui->presetSearchBarLineEdit, SIGNAL ( textChanged ( const QString& ) ),
		  this, SLOT ( updateFilteredPlaylist ( const QString& ) ) );

}

void QProjectM_MainWindow::readConfig(const std::string & configFile ) {
	
	QSettings settings ( "projectM", "qprojectM" );
	
	if ( settings.value("FullscreenOnStartup", false).toBool() )
		this->setWindowState ( this->windowState() | Qt::WindowFullScreen );
	else
		this->setWindowState ( this->windowState() & ~Qt::WindowFullScreen );

	setMenuVisible(settings.value("MenuOnStartup", false).toBool());

	
	
	ConfigFile config ( configFile );
	
	int wvw = config.read<int> ( "Window Width", 1024 );
	int wvh = config.read<int> ( "Window Height", 768 );

	ui->shuffleEnabledCheckBox->setCheckState(qprojectM()->settings().shuffleEnabled ? Qt::Checked : Qt::Unchecked);
			
	
	// Suggest to the widget the projectM window size configuration settings
	m_QProjectMWidget->setBaseSize ( wvw, wvh );
}

QProjectM_MainWindow::~QProjectM_MainWindow()
{

	for ( QHash<QString, PlaylistItemVector*>::iterator pos = historyHash.begin(); pos != historyHash.end(); ++pos )
	{
		if ( pos.value() )
			delete ( pos.value() );
	}

	delete ( ui );
	delete (activePresetIndex);
}

void QProjectM_MainWindow::clearPlaylist()
{

	playlistModel->clear();
	updatePlaylistUrl(QString());
	
	for ( QHash<QString, PlaylistItemVector*>::iterator pos = historyHash.begin(); pos != historyHash.end(); ++pos )
	{
		delete ( pos.value() );
	}
	historyHash.clear();
	historyHash.insert ( QString(), new PlaylistItemVector );
	previousFilter = QString();
	activePresetIndex->nullify();
	
	ui->presetSearchBarLineEdit->clear();
}

QProjectM * QProjectM_MainWindow::qprojectM()
{
	return m_QProjectMWidget->qprojectM();
}

void QProjectM_MainWindow::updatePlaylistSelection ( bool hardCut, unsigned int index )
{

	if ( hardCut )
		statusBar()->showMessage ( tr(QString( "*** Hard cut to \"%1\" ***" ).arg(this->qprojectM()->getPresetName(index).c_str()).toStdString().c_str()) , 2000 );
	else
		statusBar()->showMessage ( tr ( "*** Soft cut to \"%1\" ***" ).arg(this->qprojectM()->getPresetName(index).c_str()).toStdString().c_str(), 2000);
	
	*activePresetIndex = (*historyHash[previousFilter])[index].id;
}

void QProjectM_MainWindow::selectPlaylistItem ( const QModelIndex & index )
{
	if ( index.column() > 0 )
		return;

	qprojectM()->selectPreset ( index.row() );
	*activePresetIndex = index.row();
	
	playlistModel->updateItemHighlights();

}

void QProjectM_MainWindow::postProjectM_Initialize()
{
	QSettings qSettings("projectM", "qprojectM");
		
	playlistModel = new QPlaylistModel ( *qprojectM(), this );
	ui->tableView->setModel ( playlistModel );

	/// @bug only do this at startup? fix me
	//static bool firstOfRefreshPlaylist = true;

	QString url;
	
	//if (firstOfRefreshPlaylist) {
	//if (firstOfRefreshPlaylist) {
		QString playlistFile;
		if ((playlistFile = qSettings.value("PlaylistFile", QString()).toString()) == QString())
			url = QString(qprojectM()->settings().presetURL.c_str());
		else
			url = playlistFile;
		
		if (!playlistModel->readPlaylist(url))
			url = QString();
		
		updatePlaylistUrl(url);
		
		refreshPlaylist();
	//} else
	//	refreshHeaders();
	//firstOfRefreshPlaylist = false;
	

	if (!configDialog) {
		configDialog = new QProjectMConfigDialog(m_QProjectMWidget->configFile(), m_QProjectMWidget, this);	
	}

	readConfig(m_QProjectMWidget->configFile());
	
	
	connect ( m_QProjectMWidget->qprojectM(), SIGNAL ( presetSwitchedSignal ( bool,unsigned int ) ), 
		  this, SLOT ( updatePlaylistSelection ( bool,unsigned int ) ) );
		
	connect ( m_QProjectMWidget->qprojectM(), SIGNAL ( presetSwitchedSignal ( bool,unsigned int ) ), 
		  playlistModel, SLOT ( updateItemHighlights() ) );

	disconnect (m_QProjectMWidget);
	connect ( m_QProjectMWidget, SIGNAL ( presetLockChanged ( bool ) ),
		  playlistModel, SLOT ( updateItemHighlights() ) );
	
	disconnect(ui->presetSearchBarLineEdit);
	connect ( ui->presetSearchBarLineEdit, SIGNAL ( textChanged ( const QString& ) ), 
		  playlistModel, SLOT ( updateItemHighlights() ) );

	disconnect(ui->tableView);
	
	connect(ui->tableView, SIGNAL(deletesRequested(const QModelIndexList&)), 
		this, SLOT(removePlaylistItems(const QModelIndexList&)));
	
	connect(ui->tableView, SIGNAL(internalDragAndDropRequested(const QModelIndexList &, const QModelIndex &)),
		this, SLOT(dragAndDropPlaylistItems(const QModelIndexList &, const QModelIndex &)));
	connect(qprojectMWidget(), SIGNAL(projectM_BeforeDestroy()), 
		this, SLOT(clearPlaylistModel()), Qt::DirectConnection);
	
}

void QProjectM_MainWindow::clearPlaylistModel() {
	/// put in a dummy model until projectM is reinitialized
	ui->tableView->setModel(new QStandardItemModel(this));	
	if (playlistModel)
		delete(playlistModel);
				
	playlistModel = 0;
}

void QProjectM_MainWindow::dragAndDropPlaylistItems(const QModelIndexList & indices, const QModelIndex & targetIndex) {
	
	/// @bug only first index of the indices passed in is supported!
	QModelIndex firstIndex = indices.value(0);
	
	PlaylistItemMetaData metaData = historyHash[previousFilter]->value(firstIndex.row());;
	
	int newIndex;
	if (targetIndex.row() < firstIndex.row()) {
		newIndex = targetIndex.row();
	} 
	else if (targetIndex.row() > firstIndex.row()) 
	{
		newIndex = targetIndex.row()-1;
	} 
	else 
	{
		abort();
		return;
	}
	
	QModelIndexList singularList;
	singularList.append(firstIndex);
	
	removePlaylistItems(singularList);	
	insertPlaylistItem(metaData, newIndex);
}

void QProjectM_MainWindow::setMenuVisible(bool visible) {
	
	
	
	if (visible) {		
		ui->dockWidgetContents->resize(_oldPlaylistSize);
		ui->presetPlayListDockWidget->show();		
		menuBar()->show();
		statusBar()->show();
		//m_QProjectMWidget->setFocus();
		_menuVisible = true;				
	} else {		
		_oldPlaylistSize = ui->dockWidgetContents->size();
		ui->presetPlayListDockWidget->hide();
		menuBar()->hide();
		statusBar()->hide();
		_menuVisible = false;
	}
	
}

void QProjectM_MainWindow::changeRating ( const QModelIndex & index ) {
	
	if ( index.column() == 0 )
		return;

	/// @bug get rid of hard coded rating boundaries
	int newRating = (( playlistModel->data ( index, QPlaylistModel::RatingRole ).toInt()  ) % 6)+1  ;
	
	
	PlaylistItemVector & lastCache =  *historyHash[previousFilter];
	
	long id = lastCache[index.row()].id;
	
	foreach (PlaylistItemVector * items, historyHash.values()) {
		
		int i = 0;
		foreach (PlaylistItemMetaData metaData, *items) {
			if (metaData.id == id)  {
				(*items)[i].rating = newRating;
			}
			i++;
		}
	}
	qDebug() << "new rating: "  << newRating ;
	playlistModel->setData( index, newRating, QPlaylistModel::RatingRole);
	
}

void QProjectM_MainWindow::keyReleaseEvent ( QKeyEvent * e )
{

	QModelIndex modelIndex;
	switch ( e->key() )
	{
		case Qt::Key_L:
			
			if (!(e->modifiers() & Qt::ControlModifier)) {
				if ( ui->presetSearchBarLineEdit->hasFocus() )
					return;

				if (ui->tableView->hasFocus())
					return;
			}
			
			if ( ui->lockPresetCheckBox->checkState() == Qt::Checked )
			{
				ui->lockPresetCheckBox->setCheckState ( Qt::Unchecked );
			}
			else
			{
				ui->lockPresetCheckBox->setCheckState ( Qt::Checked );
			}

			// the projectM widget handles the actual lock
			//e->ignore();
			//m_QProjectMWidget->keyReleaseEvent(e);
			return;
		case Qt::Key_Y:
			if (!(e->modifiers() & Qt::ControlModifier)) {
				if ( ui->presetSearchBarLineEdit->hasFocus() )
					return;

				if (ui->tableView->hasFocus())
					return;
			}
			
			if ( ui->shuffleEnabledCheckBox->checkState() == Qt::Checked )
			{
				ui->shuffleEnabledCheckBox->setCheckState ( Qt::Unchecked );
			}
			else
			{
				ui->shuffleEnabledCheckBox->setCheckState ( Qt::Checked );
			}

			// the projectM widget handles the actual shuffle event
			return;

		case Qt::Key_F1:
			return;
			//emit(keyPressed m_QProjectMWidget,
		case Qt::Key_F:
			if (!(e->modifiers() & Qt::ControlModifier)) {
				if ( ui->presetSearchBarLineEdit->hasFocus() )
					return;

				if (ui->tableView->hasFocus())
					return;
			}
			
			this->setWindowState ( this->windowState() ^ Qt::WindowFullScreen );
			return;
		
		case Qt::Key_M:
			if (!(e->modifiers() & Qt::ControlModifier)) {
				if ( ui->presetSearchBarLineEdit->hasFocus() )
					return;

				if (ui->tableView->hasFocus())
					return;
			}
			
			setMenuVisible(!_menuVisible);
			
			refreshHeaders();
			
			return;

		case Qt::Key_R:
			if (!(e->modifiers() & Qt::ControlModifier)) {
				if ( ui->presetSearchBarLineEdit->hasFocus() )
					return;

				if (ui->tableView->hasFocus())
					return;
			}
						
			return;
		default:
			break;
	}

}


void QProjectM_MainWindow::refreshHeaders(QResizeEvent * event) {
	
	
	hHeader->setResizeMode ( 0, QHeaderView::Fixed);
	hHeader->setResizeMode ( 1, QHeaderView::ResizeToContents);		
	hHeader->resizeSection(0, ui->tableView->size().width()-20-hHeader->sectionSize(1));
	
	

}


void QProjectM_MainWindow::closeEvent ( QCloseEvent *event )
{
	writeSettings();
}


void QProjectM_MainWindow::addPresetsDialog()
{

	/// @bug this probably isn't thread safe
	if ( m_QPresetFileDialog->exec() )
	{
		const QStringList & files = m_QPresetFileDialog->selectedFiles();

		for ( QStringList::const_iterator pos = files.begin();
		        pos != files.end(); ++pos )
		{
			if ( *pos != "" )
				loadFile ( *pos );
		}

		PlaylistItemVector * playlistItems = historyHash.value ( QString() );

		for ( QHash<QString, PlaylistItemVector*>::iterator pos = historyHash.begin(); pos != historyHash.end(); ++pos )
		{
			if ( pos.key() != QString() )
				delete ( pos.value() );
		}

		historyHash.clear();
		historyHash.insert ( QString(), playlistItems );

		updateFilteredPlaylist ( previousFilter );
		ui->presetPlayListDockWidget->setWindowModified ( true );
	}
	
	//playlistModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);
}

void QProjectM_MainWindow::savePlaylist()
{

	//m_currentPlaylistUrl = file;

	if ( m_currentPlaylistUrl == QString() )
	{
		qDebug() << "current playlist file null!" ;
		return;
	}

	/// @idea add ability to save filtered list
	#if 0
		if ( playlistModel->writePlaylist ( m_currentPlaylistUrl ) ) {
		this->ui->statusbar->showMessage ( QString ( "Saved cropped preset playlist \"%1\" successfully." ).arg ( m_currentPlaylistUrl ), 3000 );
		this->ui->presetPlayListDockWidget->setWindowModified ( false );
		} 
	#endif

	QFile qfile(m_currentPlaylistUrl);

 	if (!qfile.open(QIODevice::WriteOnly)) {
		QMessageBox::warning (0, "Playlist Save Error", QString("There was a problem trying to save the playlist \"%1\".  You may not have permission to modify this file.").arg(m_currentPlaylistUrl));				
		return ;
	}

	// Use the hash that maps "" to playlist items since this list contains the entire playlist item set
	PlaylistItemVector * playlistItems = historyHash.value(QString(), 0);
	assert(playlistItems);

	PlaylistWriteFunctor writeFunctor(playlistItems->begin(), playlistItems->end(), playlistModel->playlistDesc());


	QXmlPlaylistHandler::writePlaylist(&qfile, writeFunctor);
	this->ui->statusbar->showMessage ( QString ( "Saved preset playlist \"%1\" successfully." ).arg ( m_currentPlaylistUrl ), 3000 );
	this->ui->presetPlayListDockWidget->setWindowModified ( false ); 
	
}
void QProjectM_MainWindow::updatePlaylistUrl(const QString & url = QString()) {
	
	m_currentPlaylistUrl = url;
	
	if (url == QString()) {
		ui->presetPlayListDockWidget->setWindowTitle ( "Preset Playlist - Untitled [*]" );	
		ui->presetSavePushButton->setEnabled(false);
	}
	else if (QFileInfo(url).isDir())  {
		ui->presetPlayListDockWidget->setWindowTitle
				( QString ( "Preset Directory - %1 [*]" ).arg ( url ) );		
		ui->presetSavePushButton->setEnabled(true);
	}
	else {
		ui->presetPlayListDockWidget->setWindowTitle
				( QString ( "Preset Playlist - %1 [*]" ).arg ( QFileInfo(url).fileName() ) );	
		ui->presetSavePushButton->setEnabled(true);
	}
	
	ui->presetPlayListDockWidget->setWindowModified ( false );
}



void QProjectM_MainWindow::savePlaylistAsDialog()
{
	
	m_QPlaylistFileDialog->setAllowDirectorySelect(false);	
	m_QPlaylistFileDialog->setAllowFileSelect(true);
	m_QPlaylistFileDialog->setPlaylistSaveMode(true);
	
	if ( m_QPlaylistFileDialog->exec() ) {
				
		const QStringList & files = m_QPlaylistFileDialog->selectedFiles();
		
		if (files.empty())
			return;
		
		QString url = files[0];
			
		if (url != QString()) {
			qDebug() << "passing " << url  << "to update playlist";
			updatePlaylistUrl(url);
			savePlaylist();					
		}

	}
}


void QProjectM_MainWindow::savePlaylistButtonClick()
{
	
	if (m_currentPlaylistUrl == QString() || QFileInfo(m_currentPlaylistUrl).isDir())
		return savePlaylistAsDialog();
	else {
		savePlaylist();
	}
}

void QProjectM_MainWindow::openPlaylistDialog()
{
	m_QPlaylistFileDialog->setAllowDirectorySelect(true);	
	m_QPlaylistFileDialog->setAllowFileSelect(true);	
	m_QPlaylistFileDialog->setPlaylistSaveMode(false);

	if ( m_QPlaylistFileDialog->exec() )
	{
		
		if (m_QPlaylistFileDialog->selectedFiles().empty())
			return;
		
		QString searchText = ui->presetSearchBarLineEdit->text();		
		clearPlaylist();

		
		QString url = m_QPlaylistFileDialog->selectedFiles() [0];

		
		if ( !playlistModel->readPlaylist ( url ) )
			url = QString();
		qDebug() << "url: " << url;
		updatePlaylistUrl(url);
		
		copyPlaylist();
		ui->presetSearchBarLineEdit->setText(searchText);
		updateFilteredPlaylist ( ui->presetSearchBarLineEdit->text() );
	}
}

void QProjectM_MainWindow::copyPlaylist()
{
	qprojectMWidget()->seizePresetLock();
	PlaylistItemVector * items = new PlaylistItemVector();

	for ( long i = 0; i < playlistModel->rowCount(); i++ )
	{

		QModelIndex index = playlistModel->index ( i, 0 );

		const QString & url = playlistModel->data ( index,
		                      QPlaylistModel::URLInfoRole ).toString();
		const QString & name = playlistModel->data ( index, Qt::DisplayRole ).toString();
		int rating = playlistModel->data ( index, QPlaylistModel::RatingRole ).toInt();
		
		items->push_back ( PlaylistItemMetaData ( url, name, rating, playlistItemCounter++ ) );

	}
	historyHash.insert ( QString(), items );
	
	uint index;
	if (qprojectM()->selectedPresetIndex(index))
		*activePresetIndex =  index;
	else {
		activePresetIndex->nullify();
		qDebug() << "NULLIFIED";
	}
	qprojectMWidget()->releasePresetLock();
}

void QProjectM_MainWindow::removePlaylistItems(const QModelIndexList & items) {
				
		qprojectMWidget()->seizePresetLock();
		
		QMap<int, QModelIndex> sortedItems;
		QList<int> reverseOrderKeys;
					
		foreach (QModelIndex index, items) {
			sortedItems[index.row()] = index;
		}
				
		foreach (int key, sortedItems.keys()) {
			reverseOrderKeys.insert(0, key);
		}
				
		PlaylistItemVector & lastCachedItems = *historyHash[previousFilter];
		assert (lastCachedItems.size() == playlistModel->rowCount());
		
		QVector<long> zombieItems;
		
		int i = 0;
		foreach (PlaylistItemMetaData data, lastCachedItems) {
			if (sortedItems.contains(i)) {
				zombieItems.push_back(data.id);
				if (activePresetIndex->hasValue() && data.id == activePresetIndex->value())
					activePresetIndex->nullify();
			}
			i++;
		}
		
		foreach (QString filter, historyHash.keys()) {
			
			PlaylistItemVector & cachedItems = *historyHash.value(filter);
			
			foreach(long id, zombieItems) {
				
				int index = cachedItems.indexOf(id);
				if (index >= 0)
					cachedItems.remove(index);
			}
		}
		
		foreach (int key, reverseOrderKeys) {	
			playlistModel->removeRow(key);
		}
		
		qprojectMWidget()->releasePresetLock();
}

void QProjectM_MainWindow::insertPlaylistItem
		(const PlaylistItemMetaData & data, int targetIndex) {
				
	qprojectMWidget()->seizePresetLock();
	
	long targetId = historyHash[previousFilter]->value(targetIndex).id;
		
	foreach(PlaylistItemVector * items, historyHash.values()) {
		int insertIndex = items->indexOf(targetId);
		items->insert(insertIndex, data);
	}
		
	playlistModel->insertRow(targetIndex, data.url, data.name, data.rating);
	
	qprojectMWidget()->releasePresetLock();
}

void QProjectM_MainWindow::refreshPlaylist()
{
	copyPlaylist();

	if (hHeader)
		disconnect(hHeader);
	
	
	hHeader = new QHeaderView ( Qt::Horizontal, this );
	vHeader = new QHeaderView ( Qt::Vertical, this );
	
	hHeader->setClickable ( false );
	hHeader->setSortIndicatorShown ( false );

	ui->tableView->setVerticalHeader ( vHeader );
	ui->tableView->setHorizontalHeader ( hHeader );

	refreshHeaders();
	
	/*
	hHeader->resizeSection(0, 200);
	hHeader->setResizeMode(0, QHeaderView::Stretch);
	hHeader->setResizeMode(1, QHeaderView::Fixed);
	hHeader->resizeSection(1, 25);
	*/

//	connect(ui->tableView, SIGNAL(resizeEvent(QResizeEvent *)), this, SLOT(refreshHeaders(QResizeEVent*)));
//	playlistModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);

	vHeader->hide();


}

void QProjectM_MainWindow::about()
{
	QMessageBox::about ( this, tr ( "About QProjectM and ProjectM" ),
	                     tr ( "<p><b>QProjectM</b> provides useful gui extensions to the projectM core library. For problems please email Carmelo Piccione: \n<a href=\"mailto:carmelo.piccione+qprojectM@gmail.com\"> carmelo.piccione@gmail.com</a>.</p><p><b>projectM</b> is an advanced opensource music visualizer based on Geiss's Milkdrop. For more info visit us at <a href=\"http://projectm.sf.net\">projectm.sf.net</a>.</p>" ) );
}

void QProjectM_MainWindow::openSettingsDialog() {
	
	if (configDialog->exec()) {
		
	}

}

void QProjectM_MainWindow::aboutQt() {
	return QApplication::aboutQt();
}

void QProjectM_MainWindow::createActions()
{

	connect ( ui->actionExit, SIGNAL ( triggered() ), this, SLOT ( close() ) );
	connect ( ui->actionAddPresets, SIGNAL ( triggered() ), this, SLOT ( addPresetsDialog() ) );
	connect ( ui->actionOpen_Play_List, SIGNAL ( triggered() ), this, SLOT ( openPlaylistDialog() ) );
	connect ( ui->actionSave_play_list, SIGNAL ( triggered() ), this, SLOT ( savePlaylistButtonClick() ) );
	connect ( ui->actionSave_play_list_as, SIGNAL ( triggered() ), this, SLOT ( savePlaylistAsDialog() ) );
	connect ( ui->actionAbout_qprojectM, SIGNAL ( triggered() ), this, SLOT ( about() ) );
	connect ( ui->actionConfigure_projectM, SIGNAL ( triggered() ), this, SLOT (openSettingsDialog()) );
	connect ( ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));

}

void QProjectM_MainWindow::registerSettingsAction(QAction * action) {
	ui->menuSettings->addAction(action);
}

void QProjectM_MainWindow::unregisterSettingsAction(QAction * action) {
	ui->menuSettings->removeAction(action);
}

void QProjectM_MainWindow::createMenus()
{
	ui->menuBar->hide();
}

void QProjectM_MainWindow::createToolBars()
{
	
}

void QProjectM_MainWindow::createStatusBar()
{
	statusBar()->hide();
	statusBar()->showMessage ( tr ( "Welcome to qprojectM!" ) );
}

void QProjectM_MainWindow::readSettings()
{
	
	// The settings exclusively due to qprojectM
	QSettings settings ( "projectM", "qprojectM" );
	QPoint pos = settings.value ( "pos", QPoint ( 200, 200 ) ).toPoint();
	QSize size = settings.value ( "size", QSize ( 1024, 768 ) ).toSize();
	
	// Load up where the playlist file dialog last was visited last time
	m_QPlaylistFileDialog->setDirectory
	( settings.value ( "playlistPath", m_QPlaylistFileDialog->directory().absolutePath() ).toString() );

	resize ( size );
	move ( pos );
}

void QProjectM_MainWindow::writeSettings()
{
	QSettings settings ( "projectM", "qprojectM" );
	settings.setValue ( "pos", pos() );
	settings.setValue ( "size", size() );
	settings.setValue ( "playlistPath", m_QPlaylistFileDialog->directory().absolutePath() );
}

void QProjectM_MainWindow::loadFile ( const QString &fileName, int rating )
{

	const QString & name = strippedName ( fileName );

	PlaylistItemVector * playlistItems = historyHash.value ( QString(), 0 );
	assert ( playlistItems != 0 );

	if (playlistItems->empty())
		ui->presetSavePushButton->setEnabled(true);
	
	playlistItems->push_back ( PlaylistItemMetaData ( fileName, name, rating, playlistItemCounter++ ) );

	
}


QString QProjectM_MainWindow::strippedName ( const QString &fullFileName )
{
	return QFileInfo ( fullFileName ).fileName();
}


void QProjectM_MainWindow::updateFilteredPlaylist ( const QString & text )
{

	
	qprojectMWidget()->seizePresetLock();
	
	const QString filter = text.toLower();
	unsigned int presetIndexBackup ;
	bool presetSelected = qprojectM()->selectedPresetIndex(presetIndexBackup);
	Nullable<uint> activePresetId;
	
	if (!presetSelected && activePresetIndex->hasValue()) {
		qDebug() << "active preset index has value: " << activePresetIndex->value();
		activePresetId =  activePresetIndex->value();
	} else if (presetSelected) {
		qDebug() << "preset still selected (current = " << presetIndexBackup << ")";
		const PlaylistItemVector & oldPlaylistItems = *historyHash.value(previousFilter);
		int i = 0;
		for ( PlaylistItemVector::const_iterator pos = oldPlaylistItems.begin(); pos != oldPlaylistItems.end();++pos )
		{
			if (presetIndexBackup == i) {
				activePresetId = pos->id;
				qDebug() << "preset still selected true id:" << activePresetId.value();		
				break;
			}
			i++;
		}	
	} else {
		qDebug() << "active preset index does NOT have value.";
	}
	
	/// NEED A MUTEX TO STOP PROJECTM FROM SWITCHING PRESETS
	playlistModel->clearItems();
	
	assert(!qprojectM()->presetPositionValid());
	
	bool presetExistsWithinFilter = false;
	qDebug() << "preset position valid (preloop):" << qprojectM()->presetPositionValid();
	if ( historyHash.contains ( filter ) )
	{
		const PlaylistItemVector & playlistItems = *historyHash.value ( filter );
		for ( PlaylistItemVector::const_iterator pos = playlistItems.begin(); pos != playlistItems.end();++pos )
		{
			playlistModel->appendRow ( pos->url, pos->name,  pos->rating);
			
			if (activePresetId.hasValue() && pos->id == activePresetId.value()) {
				qDebug() << "new position is " << (playlistModel->rowCount()-1);	
				qprojectM()->selectPresetPosition(playlistModel->rowCount()-1);
				presetExistsWithinFilter = true;
			}
		}
	}
	else
	{
		const PlaylistItemVector & playlistItems = *historyHash.value ( QString() );

		PlaylistItemVector * playlistItems2 = new PlaylistItemVector();
		for ( PlaylistItemVector::const_iterator pos = playlistItems.begin(); pos != playlistItems.end();++pos )
		{
			if ( ( pos->name ).contains ( filter, Qt::CaseInsensitive ) )
			{
				playlistModel->appendRow ( pos->url, pos->name, pos->rating);
				qDebug() << "preset position valid (inner loop):" << qprojectM()->presetPositionValid();
				if (activePresetId.hasValue() && pos->id == activePresetId.value()) {
					qDebug() << "new position is " << (playlistModel->rowCount()-1);
					qprojectM()->selectPresetPosition(playlistModel->rowCount()-1);
					presetExistsWithinFilter = true;
				}
				
				playlistItems2->push_back ( *pos );
			}
		}
		historyHash.insert ( filter, playlistItems2 );
	}
	qDebug() << "preset exists within filter: " << presetExistsWithinFilter;
	qDebug() << "preset position valid:" << qprojectM()->presetPositionValid();
	assert(presetExistsWithinFilter == qprojectM()->presetPositionValid());
	
	previousFilter = filter;
	qprojectMWidget()->releasePresetLock();
}

