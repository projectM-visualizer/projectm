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
#include "QPresetFileDialog.hpp"
#include "QPlaylistFileDialog.hpp"
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include "QPlaylistModel.hpp"
#include "ui_QProjectM_MainWindow.h"
#include "ConfigFile.h"
#include "QXmlPlaylistHandler.hpp"

class PlaylistWriteFunctor {
	public:
		PlaylistWriteFunctor(const QProjectM_MainWindow::PlaylistItemVector::iterator & begin,
			const QProjectM_MainWindow::PlaylistItemVector::iterator & end,
			const QString & name, const QString & desc) : 
		 m_pos(begin), m_end(end), m_name(name), m_desc(desc) {}

		inline const QString & playlistName() const {
			return m_name;
		}

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
		const QString & m_name;
		const QString & m_desc;
		

};

QProjectM_MainWindow::QProjectM_MainWindow ( const std::string & config_file )
		:m_QPresetFileDialog ( new QPresetFileDialog ( this ) ), m_QPlaylistFileDialog ( new QPlaylistFileDialog ( this ) ), oldPresetIndex ( -1 )
{

	ui = new Ui::qProjectM_MainWindow();

	ui->setupUi ( this );

	m_QProjectMWidget = new QProjectMWidget ( config_file, this );

	m_timer = new QTimer ( this );
	connect ( m_timer, SIGNAL ( timeout() ), m_QProjectMWidget, SLOT ( updateGL() ) );

	connect ( ui->lockPresetCheckBox, SIGNAL ( stateChanged ( int ) ),
	          m_QProjectMWidget, SLOT ( setPresetLock ( int ) ) );

	connect ( ui->clearPresetList_PushButton, SIGNAL ( pressed() ),
	          this, SLOT ( clearPlaylist() ) );


	connect ( m_QProjectMWidget, SIGNAL ( projectM_Initialized() ), this, SLOT ( postProjectM_Initialize() ) );


	m_QProjectMWidget->makeCurrent();
	m_QProjectMWidget->setFocus();

	m_timer->start ( 0 );

	ConfigFile config ( config_file );

	if ( config.read ( "Fullscreen", false ) )
		this->setWindowState ( this->windowState() | Qt::WindowFullScreen );
	else
		this->setWindowState ( this->windowState() & ~Qt::WindowFullScreen );

	setCentralWidget ( m_QProjectMWidget );


	int wvw = config.read<int> ( "Window Width", 512 );
	int wvh = config.read<int> ( "Window Height", 512 );

	// Suggest to the widget the projectM window size configuration settings
	m_QProjectMWidget->setBaseSize ( wvw, wvh );

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	readSettings();
	ui->presetPlayListDockWidget->hide();


}

QProjectM_MainWindow::~QProjectM_MainWindow()
{

	for ( QHash<QString, PlaylistItemVector*>::iterator pos = historyHash.begin(); pos != historyHash.end(); ++pos )
	{
		if ( pos.value() )
			delete ( pos.value() );
	}

	delete ( ui );

}

void QProjectM_MainWindow::clearPlaylist()
{

	playlistModel->clear();
	ui->dockWidgetContents->setWindowTitle ( "Preset Playlist" );
	ui->dockWidgetContents->setWindowModified(false);
	m_currentPlaylistFile = "";

	for ( QHash<QString, PlaylistItemVector*>::iterator pos = historyHash.begin(); pos != historyHash.end(); ++pos )
	{
		delete ( pos.value() );
	}
	historyHash.clear();
	historyHash.insert ( QString(), new PlaylistItemVector );
	previousFilter = QString();
	
	ui->presetSearchBarLineEdit->clear();
}

QProjectM * QProjectM_MainWindow::getQProjectM()
{
	return m_QProjectMWidget->getQProjectM();
}

void QProjectM_MainWindow::updatePlaylistSelection ( bool hardCut, int index )
{

	if ( hardCut )
		statusBar()->showMessage ( tr ( "*** Hard cut ***" ) , 2000 );
	else
		statusBar()->showMessage ( tr ( "*** Soft cut ***" ) , 2000 );


	//playlistModel->setData(playlistModel->index(index, 0), Qt::green, Qt::BackgroundRole);
	//oldPresetIndex = index;

}

void QProjectM_MainWindow::selectPlaylistItem ( const QModelIndex & index )
{
	if ( index.column() > 0 )
		return;

	getQProjectM()->selectPreset ( index.row() );
	playlistModel->updateItemHighlights();

}


void QProjectM_MainWindow::postProjectM_Initialize()
{

	playlistModel = new QPlaylistModel ( *m_QProjectMWidget->getQProjectM(),this );
	ui->tableView->setModel ( playlistModel );
	refreshPlaylist();


	connect ( m_QProjectMWidget->getQProjectM(), SIGNAL ( presetSwitchedSignal ( bool,unsigned int ) ), this, SLOT ( updatePlaylistSelection ( bool,unsigned int ) ) );
	connect ( ui->presetSearchBarLineEdit, SIGNAL ( textChanged ( const QString& ) ), this, SLOT ( updateFilteredPlaylist ( const QString& ) ) );

	connect ( ui->tableView, SIGNAL ( activated ( const QModelIndex & ) ),
	          this, SLOT ( selectPlaylistItem ( const QModelIndex & ) ) );

	connect ( ui->tableView, SIGNAL ( clicked ( const QModelIndex & ) ),
	          this, SLOT ( changeRating ( const QModelIndex & ) ) );
	connect ( m_QProjectMWidget, SIGNAL ( presetLockChanged ( bool ) ),  playlistModel, SLOT ( updateItemHighlights() ) );
	connect ( m_QProjectMWidget->getQProjectM(), SIGNAL ( presetSwitchedSignal ( bool,unsigned int ) ), playlistModel, SLOT ( updateItemHighlights() ) );

	/// @bug not right - selected preset problems when searching
	connect ( ui->presetSearchBarLineEdit, SIGNAL ( textChanged ( const QString& ) ), playlistModel, SLOT ( updateItemHighlights() ) );

}

void QProjectM_MainWindow::changeRating ( const QModelIndex & index )
{
	if ( index.column() == 0 )
		return;

	playlistModel->setData
	( index, ( playlistModel->data ( index, QPlaylistModel::RatingRole ).toInt() +1 ) % 6, QPlaylistModel::RatingRole );
}

void QProjectM_MainWindow::keyReleaseEvent ( QKeyEvent * e )
{

	QModelIndex modelIndex;
	switch ( e->key() )
	{
		case Qt::Key_L:
			if ( ui->presetSearchBarLineEdit->hasFocus() )
				return;

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

		case Qt::Key_F1:
			return;
			//emit(keyPressed m_QProjectMWidget,
		case Qt::Key_F:
			if ( ui->presetSearchBarLineEdit->hasFocus() )
				return;
			this->setWindowState ( this->windowState() ^ Qt::WindowFullScreen );
			return;

		case Qt::Key_M:
			if ( ui->presetSearchBarLineEdit->hasFocus() )
				return;

			if ( ui->presetPlayListDockWidget->isVisible() )
			{
				ui->presetPlayListDockWidget->hide();
			}
			else
				ui->presetPlayListDockWidget->show();

			if ( menuBar()->isVisible() )
			{
				menuBar()->hide();
				m_QProjectMWidget->setFocus();
			}
			else
			{
				menuBar()->show();
			}

			if ( statusBar()->isVisible() )
			{
				statusBar()->hide();
			}
			else
			{
				statusBar()->show();
			}
			return;

		case Qt::Key_R:
			if ( ui->presetSearchBarLineEdit->hasFocus() )
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

void QProjectM_MainWindow::closeEvent ( QCloseEvent *event )
{
	writeSettings();
}


void QProjectM_MainWindow::addPresets()
{


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

	//m_currentPlaylistFile = file;

	if ( m_currentPlaylistFile == "" )
	{
		qDebug() << "current playlist file null!" ;
		return;
	}

	/// @idea add ability to save filtered list
	#if 0
		if ( playlistModel->writePlaylist ( m_currentPlaylistFile ) ) {
		this->ui->statusbar->showMessage ( QString ( "Saved cropped preset playlist \"%1\" successfully." ).arg ( m_currentPlaylistFile ), 3000 );
		this->ui->presetPlayListDockWidget->setWindowModified ( false );
		} 
	#endif

	QFile qfile(m_currentPlaylistFile);

 	if (!qfile.open(QIODevice::WriteOnly)) {
		QMessageBox::warning (0, "Playlist Save Error", QString("There was a problem trying to save the playlist \"%1\".  You may not have permission to modify this file.").arg(m_currentPlaylistFile));				
		return ;
	}

	// Use the hash that maps "" to playlist items since this list contains the entire playlist item set
	PlaylistItemVector * playlistItems = historyHash.value(QString(), 0);
	assert(playlistItems);

	assert(playlistModel->playlistName() != "");
	PlaylistWriteFunctor writeFunctor(playlistItems->begin(), playlistItems->end(), playlistModel->playlistName(), playlistModel->playlistDesc());


	QXmlPlaylistHandler::writePlaylist(&qfile, writeFunctor);
	this->ui->statusbar->showMessage ( QString ( "Saved preset playlist \"%1\" successfully." ).arg ( m_currentPlaylistFile ), 3000 );
	this->ui->presetPlayListDockWidget->setWindowModified ( false ); 
	
	
}

void QProjectM_MainWindow::openPlaylist()
{

	if ( m_QPlaylistFileDialog->exec() )
	{

		QString searchText = ui->presetSearchBarLineEdit->text();		
		clearPlaylist();

		const QString file = m_QPlaylistFileDialog->selectedFiles() [0];

		if ( playlistModel->readPlaylist ( file ) )
		{
			ui->presetPlayListDockWidget->setWindowTitle
			( QString ( "Preset Playlist - %1 [*]" ).arg ( playlistModel->playlistName() ) );
			m_currentPlaylistFile = file;
		}
		else
		{
			ui->dockWidgetContents->setWindowTitle ( "Preset Playlist" );
		}
		ui->presetPlayListDockWidget->setWindowModified ( false );
		copyPlaylist();
		ui->presetSearchBarLineEdit->setText(searchText);
		updateFilteredPlaylist ( ui->presetSearchBarLineEdit->text() );
	}
}

void QProjectM_MainWindow::copyPlaylist()
{
	PlaylistItemVector * items = new PlaylistItemVector();

	for ( int i = 0; i < playlistModel->rowCount(); i++ )
	{

		QModelIndex index = playlistModel->index ( i, 0 );

		const QString & url = playlistModel->data ( index,
		                      QPlaylistModel::URLInfoRole ).toString();
		const QString & name = playlistModel->data ( index, Qt::DisplayRole ).toString();
		int rating = playlistModel->data ( index, QPlaylistModel::RatingRole ).toInt();

		items->push_back ( PlaylistItemMetaData ( url, name, rating ) );

	}
	historyHash.insert ( QString(), items );

}

void QProjectM_MainWindow::refreshPlaylist()
{
	copyPlaylist();

	hHeader = new QHeaderView ( Qt::Horizontal, this );
	vHeader = new QHeaderView ( Qt::Vertical, this );

	hHeader->setClickable ( false );
	hHeader->setSortIndicatorShown ( false );
	//hHeader->setSortIndicator(1, Qt::AscendingOrder);
	hHeader->setStretchLastSection ( false );

	ui->tableView->setVerticalHeader ( vHeader );
	ui->tableView->setHorizontalHeader ( hHeader );

	hHeader->setResizeMode ( 0, QHeaderView::Stretch );
        hHeader->setResizeMode ( 1, QHeaderView::Fixed );

	//hHeader->resizeSection(0, 500);
	hHeader->resizeSection(1, 50);

	/*
	hHeader->resizeSection(0, 200);
	hHeader->setResizeMode(0, QHeaderView::Stretch);
	hHeader->setResizeMode(1, QHeaderView::Fixed);
	hHeader->resizeSection(1, 25);
	*/

//	playlistModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);

	vHeader->hide();
//	playlistModel->setHeaderData(0, Qt::Horizontal, 200, Qt::SizeHintRole);
	//playlistModel->setHeaderData(1, Qt::Horizontal, tr("Rating"));//, Qt::DisplayRole);
	//playlistModel->setHeaderData(2, Qt::Horizontal, tr("Preset"));//, Qt::DisplayRole);


}

void QProjectM_MainWindow::about()
{
	QMessageBox::about ( this, tr ( "About QProjectM and ProjectM" ),
	                     tr ( "<p><b>QProjectM</b> provides useful gui extensions to the projectM core library. For problems please email Carmelo Piccione: \n<a href=\"mailto:carmelo.piccione@gmail.com\"> carmelo.piccione@gmail.com</a>.</p><p><b>projectM</b> is an advanced opensource music visualizer based on Geiss's Milkdrop. For more info visit us at <a href=\"http://projectm.sf.net\">projectm.sf.net</a>.</p>" ) );
}


void QProjectM_MainWindow::createActions()
{

	connect ( ui->actionExit, SIGNAL ( triggered() ), this, SLOT ( close() ) );
	connect ( ui->actionAddPresets, SIGNAL ( triggered() ), this, SLOT ( addPresets() ) );
	connect ( ui->actionOpen_Play_List, SIGNAL ( triggered() ), this, SLOT ( openPlaylist() ) );
	connect ( ui->actionSave_play_list, SIGNAL ( triggered() ), this, SLOT ( savePlaylist() ) );
	connect ( ui->actionAbout_qprojectM, SIGNAL ( triggered() ), this, SLOT ( about() ) );

	//connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));

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
	QSettings settings ( "projectM", "QProjectM" );
	QPoint pos = settings.value ( "pos", QPoint ( 200, 200 ) ).toPoint();
	QSize size = settings.value ( "size", QSize ( 1000, 600 ) ).toSize();
	m_QPlaylistFileDialog->setDirectory
	( settings.value ( "playlistPath", m_QPlaylistFileDialog->directory().absolutePath() ).toString() );

	m_QPresetFileDialog->setDirectory
	( settings.value ( "presetPath", m_QPresetFileDialog->directory().absolutePath() ).toString() );

	resize ( size );
	move ( pos );
}

void QProjectM_MainWindow::writeSettings()
{
	QSettings settings ( "projectM", "QProjectM" );
	settings.setValue ( "pos", pos() );
	settings.setValue ( "size", size() );
	settings.setValue ( "playlistPath", m_QPlaylistFileDialog->directory().absolutePath() );
	settings.setValue ( "presetPath", m_QPresetFileDialog->directory().absolutePath() );
}



void QProjectM_MainWindow::loadFile ( const QString &fileName, int rating )
{

	const QString & name = strippedName ( fileName );

	PlaylistItemVector * playlistItems = historyHash.value ( QString(), 0 );
	assert ( playlistItems != 0 );

	playlistItems->push_back ( PlaylistItemMetaData ( fileName, name, rating ) );

}


QString QProjectM_MainWindow::strippedName ( const QString &fullFileName )
{
	return QFileInfo ( fullFileName ).fileName();
}


void QProjectM_MainWindow::updateFilteredPlaylist ( const QString & text )
{

	const QString filter = text.toLower();

	playlistModel->clearItems();
	
	if ( historyHash.contains ( filter ) )
	{
		const PlaylistItemVector & playlistItems = *historyHash.value ( filter );
		for ( PlaylistItemVector::const_iterator pos = playlistItems.begin(); pos != playlistItems.end();++pos )
		{
			playlistModel->appendRow ( pos->url, pos->name, pos->rating );
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
				playlistModel->appendRow ( pos->url, pos->name, pos->rating );
				playlistItems2->push_back ( *pos );
			}
		}
		historyHash.insert ( filter, playlistItems2 );
	}
	previousFilter = filter;
}

