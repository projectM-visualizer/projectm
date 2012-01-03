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

#include <QtGui>
#include "qprojectm_mainwindow.hpp"
#include "qpresetfiledialog.hpp"
#include "qplaylistfiledialog.hpp"
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include "qplaylistmodel.hpp"
#include "qpresettextedit.hpp"
#include "ui_qprojectm_mainwindow.h"
#include "qprojectmconfigdialog.hpp"
#include "qpreseteditordialog.hpp"
#include "ui_qpreseteditordialog.h"
#include "configfile.hpp"
#include "qxmlplaylisthandler.hpp"
#include "nullable.hpp"
#include "qprojectmwidget.hpp"

class PlaylistWriteFunctor {
	public:
		PlaylistWriteFunctor(const QVector<QProjectM_MainWindow::PlaylistItemMetaData*>::iterator & begin,
			const QVector<QProjectM_MainWindow::PlaylistItemMetaData*>::iterator & end, const QString & desc) :
		 m_pos(begin), m_end(end), m_desc(desc) {}

		inline const QString & playlistDesc() const {
			return m_desc;
		}

		inline bool nextItem(QString & name, QString & url, int & rating, int & breedability) {

			if (m_pos == m_end)
				return false;

			const QProjectM_MainWindow::PlaylistItemMetaData & data = **m_pos;

			url = data.url;
			rating = data.rating;
			name = data.name;

			breedability = data.breedability;
			m_pos++;
			return true;
		}
	private:
		QVector<QProjectM_MainWindow::PlaylistItemMetaData*>::const_iterator m_pos;
		QVector<QProjectM_MainWindow::PlaylistItemMetaData*>::const_iterator m_end;

		const QString & m_desc;

};

QProjectM_MainWindow::QProjectM_MainWindow ( const std::string & config_file, QMutex * audioMutex)
		:m_QPresetFileDialog ( new QPresetFileDialog ( this ) ), ui(0), m_QPlaylistFileDialog
		( new QPlaylistFileDialog ( this )), playlistModel(0),
		configDialog(0), hHeader(0), vHeader(0), _menuVisible(true), _menuAndStatusBarsVisible(true),
activePresetIndex(new Nullable<long>), playlistItemCounter(0), m_QPresetEditorDialog(0)
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

	connect(ui->tableView, SIGNAL(mousePressed(QMouseEvent*, const QModelIndexList &)), this,
		SLOT(popupPlaylistContextMenu(QMouseEvent*, const QModelIndexList &)));

	connect ( m_QProjectMWidget, SIGNAL ( projectM_Initialized(QProjectM*) ),
		  this, SLOT ( postProjectM_Initialize() ) );

	//connect(this, SIGNAL(dockLocationChanged ( Qt::DockWidgetArea)), SLOT(dockLocationChanged(Qt::DockWidgetArea)));
	if (!m_QProjectMWidget->isValid()) {
		int ret = QMessageBox::warning(this, tr("projectM cannot be started."),
					       tr("Your graphics driver or configuration is not supported by projectM. Please contact the developers (carmelo.piccione+projectM@gmail.com or psperl+projectM@gmail.com) with your card and driver information so we can help you get it working."),
			      QMessageBox::Ok);
		exit(-1);
	}


	m_QProjectMWidget->makeCurrent();
	m_QProjectMWidget->setFocus();
	setCentralWidget ( m_QProjectMWidget );

	m_timer->start ( 0 );

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	readSettings();

	readPlaylistSettings();

	connect ( ui->tableView, SIGNAL ( activated ( const QModelIndex & ) ),
		  this, SLOT ( selectPlaylistItem ( const QModelIndex & ) ) );
	connect ( ui->tableView, SIGNAL ( clicked ( const QModelIndex & ) ),
		  this, SLOT ( changePresetAttribute ( const QModelIndex & ) ) );

	connect ( ui->presetSearchBarLineEdit, SIGNAL ( textChanged ( const QString& ) ),
		  this, SLOT ( updateFilteredPlaylist ( const QString& ) ) );

}

void QProjectM_MainWindow::dockLocationChanged(Qt::DockWidgetArea area) {
	dockWidgetArea = area;
}

#include <QMouseEvent>
void QProjectM_MainWindow::popupPlaylistContextMenu(QMouseEvent * mouseEvent,const QModelIndexList & items) {


		selectedPlaylistIndexes = items;

		if (items.empty()) {
			mouseEvent->accept();
			return;
		}

		if (mouseEvent->button() == Qt::RightButton) {

			QPoint point = mouseEvent->globalPos();
			playlistContextMenu->popup(point, ui->actionEdit_this_preset);
			mouseEvent->accept();
		} else {
			mouseEvent->accept();
		}
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


	this->resize(wvw,wvh);
}

QProjectM_MainWindow::~QProjectM_MainWindow()
{


	//this->qprojectMWidget()->mutex()->lock();


	for ( QHash<QString, PlaylistItemVector*>::iterator pos = historyHash.begin(); pos != historyHash.end(); ++pos )
	{
		if ( pos.value() )
			delete ( pos.value() );
	}

	if (ui)
		delete ( ui );

	if (activePresetIndex)
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
	playlistItemMetaDataHash.clear();
	previousFilter = QString();
	activePresetIndex->nullify();

	ui->presetSearchBarLineEdit->clear();
}

QProjectM * QProjectM_MainWindow::qprojectM()
{

	return m_QProjectMWidget->qprojectM();
}


projectM * QProjectM_MainWindow::GetProjectM()
{
	return m_QProjectMWidget->qprojectM();
}

void QProjectM_MainWindow::addPCM(float * buffer, unsigned int bufferSize) {

	qprojectM()->pcm()->addPCMfloat(buffer, bufferSize);
}

void QProjectM_MainWindow::updatePlaylistSelection ( bool hardCut, unsigned int index )
{

	if ( hardCut )
		statusBar()->showMessage ( tr(QString( "*** Hard cut to \"%1\" ***" ).arg(this->qprojectM()->getPresetName(index).c_str()).toStdString().c_str()) , 2000 );
	else
		statusBar()->showMessage ( tr ( "*** Soft cut to \"%1\" ***" ).arg(this->qprojectM()->getPresetName(index).c_str()).toStdString().c_str(), 2000);

	*activePresetIndex = (*historyHash[previousFilter])[index];
}

void QProjectM_MainWindow::selectPlaylistItem ( const QModelIndex & index )
{
	if ( index.column() > 0 )
		return;

	selectPlaylistItem(index.row());

}

void QProjectM_MainWindow::selectPlaylistItem ( int rowIndex)
{

	qprojectM()->selectPreset (rowIndex);
	*activePresetIndex = rowIndex;

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

	if (!configDialog) {
		configDialog = new QProjectMConfigDialog(m_QProjectMWidget->configFile(), m_QProjectMWidget, this);
	}

	readConfig(m_QProjectMWidget->configFile());


	connect ( m_QProjectMWidget->qprojectM(), SIGNAL ( presetSwitchedSignal ( bool,unsigned int ) ),
		  this, SLOT ( updatePlaylistSelection ( bool,unsigned int ) ) );

	connect ( m_QProjectMWidget->qprojectM(), SIGNAL ( presetRatingChangedSignal ( unsigned int,int, PresetRatingType) ),
			  this, SLOT ( presetRatingChanged( unsigned int,int, PresetRatingType) ));

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

	/// @bug hack: shouldn't have to change width for this to work correctly
	m_QProjectMWidget->resize(m_QProjectMWidget->size().width()-1, m_QProjectMWidget->size().height());




}

void QProjectM_MainWindow::openPresetEditorDialog(int rowIndex) {


	qprojectMWidget()->seizePresetLock();

	if (!m_QPresetEditorDialog) {
		m_QPresetEditorDialog = new QPresetEditorDialog(qprojectMWidget());
		connect(m_QPresetEditorDialog, SIGNAL(presetModified(int)),
			this, SLOT(selectPlaylistItem(int)));
	}

	Q_ASSERT(historyHash.contains(previousFilter));
	Q_ASSERT(playlistItemMetaDataHash.contains((*historyHash[previousFilter])[rowIndex]));
	const QString presetUrl = playlistItemMetaDataHash[(*historyHash[previousFilter])[rowIndex]].url;

	m_QPresetEditorDialog->setPreset(presetUrl, rowIndex);

	if (m_QPresetEditorDialog->exec()) {

	}

	qprojectMWidget()->releasePresetLock();

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

	PlaylistItemMetaData metaData = playlistItemMetaDataHash[historyHash[previousFilter]->value(firstIndex.row())];

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
		// Self drag event ignored
		// abort();
		return;
	}

	QModelIndexList singularList;
	singularList.append(firstIndex);

	removePlaylistItems(singularList);
	insertPlaylistItem(metaData, newIndex);
	ui->presetPlayListDockWidget->setWindowModified ( true );
}

void QProjectM_MainWindow::setMenuAndStatusBarsVisible(bool visible) {

        if (visible) {
                if (_menuVisible) {
                        menuBar()->show();
			statusBar()->show();
		}
                else {
                        menuBar()->hide();
			statusBar()->hide();
		}
                _menuAndStatusBarsVisible = true;
        } else {
                if (_menuVisible) {
                        menuBar()->hide();
			statusBar()->hide();
		}
                else {
                        menuBar()->hide();
			statusBar()->hide();
		}
                _menuAndStatusBarsVisible = false;
        }
}

void QProjectM_MainWindow::readPlaylistSettings() {
	const QSettings settings ( "projectM", "qprojectM" );

	/// @bug Hack for default values here. Should be auto placed or something elative to main window.

	const QPoint playlistPos = settings.value("playlistPos",
		 QPoint(200,200)).toPoint();

	bool playlistDocked = settings.value("playlistDocked", true).toBool();

	const QSize playlistWindowSize = settings.value("playlistWindowSize", QSize(300,800)).toSize();

	//ui->presetPlayListDockWidget
	//ui->presetPlayListDockWidget->set

	if (!playlistDocked) {
		ui->presetPlayListDockWidget->hide();
		ui->presetPlayListDockWidget->setFloating(!playlistDocked);
		//ui->presetPlayListDockWidget->setFloating(!playlistDocked);
		ui->presetPlayListDockWidget->move(playlistPos);
		ui->presetPlayListDockWidget->resize(playlistWindowSize);
	} else {

		if (!ui->menuBar->isVisible())
			ui->presetPlayListDockWidget->hide();
		else
			ui->presetPlayListDockWidget->show();
		ui->presetPlayListDockWidget->hide();
	}
}

void QProjectM_MainWindow::setMenuVisible(bool visible) {



	if (visible) {
		ui->dockWidgetContents->resize(_oldPlaylistSize);

		ui->presetPlayListDockWidget->show();
		if (_menuAndStatusBarsVisible) {
			menuBar()->show();
			statusBar()->show();
		}
		else {
			menuBar()->hide();
			statusBar()->hide();
		}
		_menuVisible = true;
	} else {
		_oldPlaylistSize = ui->dockWidgetContents->size();

		// Only hide the playlist when it is attached to the main window.
		if (!ui->presetPlayListDockWidget->isFloating())
			ui->presetPlayListDockWidget->hide();

		menuBar()->hide();
		statusBar()->hide();
		_menuVisible = false;
	}

}

void QProjectM_MainWindow::changePresetAttribute ( const QModelIndex & index )
{

	if ( index.column() == 0 )
		return;

	PlaylistItemVector & lastCache =  *historyHash[previousFilter];
	const long id = lastCache[index.row()];

	ui->presetPlayListDockWidget->setWindowModified ( true );

	if (index.column() == 1)
	{
		/// @bug get rid of hard coded rating boundaries
		const int newRating = ( ( playlistModel->data ( index, QPlaylistModel::RatingRole ).toInt() ) % 6 ) +1  ;

		playlistModel->setData ( index, newRating, QPlaylistModel::RatingRole );


	}
	else if (index.column() == 2)
	{

		/// @bug get rid of hard coded breedability boundaries
		const int newBreedability = ( ( playlistModel->data ( index, QPlaylistModel::BreedabilityRole ).toInt() ) % 6 ) +1  ;

		playlistItemMetaDataHash[id].breedability = newBreedability;

		playlistModel->setData ( index, newBreedability, QPlaylistModel::BreedabilityRole );
	}

}

void QProjectM_MainWindow::keyReleaseEvent ( QKeyEvent * e )
{

	QModelIndex modelIndex;
	switch ( e->key() )
	{

		case Qt::Key_E:
			if (e->modifiers() & Qt::ControlModifier) {
				if (activePresetIndex->hasValue())
					openPresetEditorDialog(historyHash[previousFilter]->indexOf(activePresetIndex->value()));
			} else
				e->ignore();
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
		{
			if (!(e->modifiers() & Qt::ControlModifier)) {
				if ( ui->presetSearchBarLineEdit->hasFocus() )
					return;

				if (ui->tableView->hasFocus())
					return;
			}

			bool projectM_HadFocus = qprojectMWidget()->hasFocus();
			this->setWindowState ( this->windowState() ^ Qt::WindowFullScreen );

			if (projectM_HadFocus || !_menuVisible)
				qprojectMWidget()->setFocus();

			return;
		}

             case Qt::Key_B:
                        if (!(e->modifiers() & Qt::ControlModifier)) {
                                if ( ui->presetSearchBarLineEdit->hasFocus() )
                                        return;

                                if (ui->tableView->hasFocus())
                                        return;
                        }

                        setMenuAndStatusBarsVisible(!_menuAndStatusBarsVisible);
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

	const int numRatings =  qprojectM()->settings().softCutRatingsEnabled ? 2 : 1;

	int sizeTotal = 0;
	for (int i = 0; i < numRatings; i++) {
		// Add 1 to skip the Name column
		hHeader->setResizeMode (i+1, QHeaderView::ResizeToContents);
		sizeTotal += hHeader->sectionSize(i+1);
	}
	hHeader->resizeSection(0, ui->tableView->size().width()-20-sizeTotal);



}


bool QProjectM_MainWindow::warnIfPlaylistModified() {

	if (ui->presetPlayListDockWidget->isWindowModified()) {
		int ret = QMessageBox::warning(this, tr("Warning: unsaved playlist!"),
					       tr("The open playlist has been modified.\n"
							       "Do you want to save your changes?"),
	      QMessageBox::Save | QMessageBox::Discard
			      | QMessageBox::Cancel,
	 QMessageBox::Save);

		switch (ret) {
			case QMessageBox::Save:
				savePlaylistButtonClick();
				return true;
			case QMessageBox::Discard:
				return true;
			case QMessageBox::Cancel:
				return false;
			default:
				return true;
		}
	}
	return true;
}

void QProjectM_MainWindow::closeEvent ( QCloseEvent *event )
{

	if (!warnIfPlaylistModified())
		event->ignore();
	writeSettings();
}

void QProjectM_MainWindow::insertPresetsDialog() {

	if (selectedPlaylistIndexes.empty())
		return;

	return addPresetsDialog(selectedPlaylistIndexes[0]);

}

void QProjectM_MainWindow::addPresetsDialog() {
	return addPresetsDialog(QModelIndex());
}

void QProjectM_MainWindow::addPresetsDialog(const QModelIndex & index)
{

	/// @bug this probably isn't thread safe
	if ( m_QPresetFileDialog->exec() )
	{
		const QStringList & files = m_QPresetFileDialog->selectedFiles();

		int i = 0;
		for ( QStringList::const_iterator pos = files.begin();
		        pos != files.end(); ++pos )
		{
			if ( *pos != "" ) {

				Nullable<int> row;
				if (index != QModelIndex() && index.isValid()) {

					if (index.row() == 0)
						row = 0;
					else
						row = index.row()-1;
				}
				loadFile ( *pos, 3, 3, row);
				i++;
			}
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
	PlaylistItemVector * playlistItemKeys = historyHash.value(QString(), 0);


	QVector<PlaylistItemMetaData*> itemMetaDataVector;
	foreach (long key, *playlistItemKeys) {
		itemMetaDataVector.push_back(&playlistItemMetaDataHash[key]);
	}

	PlaylistWriteFunctor writeFunctor(itemMetaDataVector.begin(), itemMetaDataVector.end(), playlistModel->playlistDesc());


	QXmlPlaylistHandler::writePlaylist(&qfile, writeFunctor);
	this->ui->statusbar->showMessage ( QString ( "Saved preset playlist \"%1\" successfully." ).arg ( m_currentPlaylistUrl ), 4000 );
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


		const bool loadedOk = playlistModel->readPlaylist ( url );

		if (!loadedOk) {
			qDebug() << "could not open playlist";
			url = QString();
		}

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
	playlistItemMetaDataHash.clear();
	PlaylistItemVector * items = new PlaylistItemVector();

	for ( long i = 0; i < playlistModel->rowCount(); i++ )
	{

		QModelIndex index = playlistModel->index ( i, 0 );

		const QString & url = playlistModel->data ( index,
		                      QPlaylistModel::URLInfoRole ).toString();
		const QString & name = playlistModel->data ( index, Qt::DisplayRole ).toString();
		int rating = playlistModel->data ( index, QPlaylistModel::RatingRole ).toInt();
		int breed = playlistModel->data ( index, QPlaylistModel::BreedabilityRole).toInt();
		items->push_back (playlistItemCounter );
		playlistItemMetaDataHash[playlistItemCounter] =
				PlaylistItemMetaData ( url, name, rating, breed, playlistItemCounter );

		playlistItemCounter++;

	}

	historyHash.insert ( QString(), items );

	uint index;
	if (qprojectM()->selectedPresetIndex(index))
		*activePresetIndex =  index;
	else
		activePresetIndex->nullify();

	qprojectMWidget()->releasePresetLock();
}

void QProjectM_MainWindow::removeSelectedPlaylistItems() {
	removePlaylistItems(selectedPlaylistIndexes);
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
		foreach (long id, lastCachedItems) {
			if (sortedItems.contains(i)) {
				zombieItems.push_back(id);
				playlistItemMetaDataHash.remove(id);
				if (activePresetIndex->hasValue() && id == activePresetIndex->value())
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
		ui->presetPlayListDockWidget->setWindowModified ( true );
}

void QProjectM_MainWindow::insertPlaylistItem
		(const PlaylistItemMetaData & data, int targetIndex) {

	qprojectMWidget()->seizePresetLock();

	const long targetId = historyHash[previousFilter]->value(targetIndex);

	playlistItemMetaDataHash[data.id] = data;

	foreach(PlaylistItemVector * items, historyHash.values()) {
		int insertIndex = items->indexOf(targetId);

		items->insert(insertIndex, data.id);
	}

	playlistModel->insertRow(targetIndex, data.url, data.name, data.rating, data.breedability);

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

	vHeader->hide();
}

void QProjectM_MainWindow::about()
{
	QMessageBox::about ( this, tr ( "About projectM-qt and projectM" ),
	                     tr ( "<p><b>projectM-qt</b> provides useful gui extensions to the projectM core library. For problems please email Carmelo Piccione: \n<a href=\"mailto:carmelo.piccione+projectM@gmail.com\"> carmelo.piccione+projectM@gmail.com</a>.</p><p><b>projectM</b> is an advanced opensource music visualizer originally based on Milkdrop but over time has gone above and beyond it. For more info visit us at <a href=\"http://projectm.sf.net\">projectm.sf.net</a> or join #projectM on the freenode IRC network." ) );
}



void QProjectM_MainWindow::hotkeyReference()
{
	QMessageBox::information ( this, tr( "Hotkey Reference"),
				   tr ( "<dl><dt><b>CTRL-L</b></dt> <dd>Locks / unlocks the active preset</dd><dt><b>CTRL-N</b></dt> <dd>Moves to next preset in the playlist </dd><dt><dt><b>CTRL-P</b></dt> <dd>Moves to previous preset in the playlist </dd><dt><b>CTRL-R</b></dt> <dd>Randomly selects a preset from the playlist </dd><dt><b>CTRL-M</b></dt><dd>Show / hide the menu display</dd><dt><b>CTRL-E</b></dt> <dd> Opens the preset editor with the active preset</dd><dt><b>CTRL-F</b></dt> <dd> Enables / disables full screen mode</dd><dt><b>CTRL-Y</b></dt><dd>Enables / disables shuffle mode </dd><dt><b>CTRL-S</b></dt> <dd>Saves and applies changes to the active preset (only when editing) </dd><dt><b>CTRL-B</b></dt><dd>Shows / hides menu and status bar</dd></dl>" ));
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
	connect ( ui->actionAbout_projectM, SIGNAL ( triggered() ), this, SLOT ( about() ) );
	connect ( ui->actionConfigure_projectM, SIGNAL ( triggered() ), this, SLOT (openSettingsDialog()) );
	connect ( ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect ( ui->actionHotkey_Reference, SIGNAL(triggered()), this, SLOT(hotkeyReference()));

	connect (ui->actionRemove_selection, SIGNAL(triggered()), this, SLOT(removeSelectedPlaylistItems()));
	connect (ui->actionInsert_presets, SIGNAL(triggered()), this, SLOT(insertPresetsDialog()));
	connect (ui->actionEdit_this_preset, SIGNAL(triggered()), this, SLOT(openPresetEditorDialogForSelectedPreset()));
	connect (ui->actionHard_cut_to_this_preset, SIGNAL(triggered()), this, SLOT(presetHardCut()));
	connect (ui->actionSoft_cut_to_this_preset, SIGNAL(triggered()), this, SLOT(presetSoftCut()));
}


void QProjectM_MainWindow::presetHardCut() {
	if (selectedPlaylistIndexes.empty())
		return;

	qprojectM()->selectPreset(selectedPlaylistIndexes[0].row(), true);
}


void QProjectM_MainWindow::presetSoftCut() {
	if (selectedPlaylistIndexes.empty())
		return;

	qprojectM()->selectPreset(selectedPlaylistIndexes[0].row(), false);
}


void QProjectM_MainWindow::openPresetEditorDialogForSelectedPreset() {

	if (selectedPlaylistIndexes.empty())
		return;

	openPresetEditorDialog(selectedPlaylistIndexes[0].row());
}

void QProjectM_MainWindow::registerSettingsAction(QAction * action) {
	ui->menuSettings->addAction(action);
}

void QProjectM_MainWindow::unregisterSettingsAction(QAction * action) {
	ui->menuSettings->removeAction(action);
}

void QProjectM_MainWindow::createMenus()
{

	playlistContextMenu = new QMenu("Playlist Actions", this);
	playlistContextMenu->addAction(ui->actionEdit_this_preset);

	playlistContextMenu->addAction(ui->actionInsert_presets);
	playlistContextMenu->addAction(ui->actionRemove_selection);

	playlistContextMenu->addAction(ui->actionHard_cut_to_this_preset);
	playlistContextMenu->addAction(ui->actionSoft_cut_to_this_preset);

	ui->menuBar->hide();


}



void QProjectM_MainWindow::createToolBars()
{

}

void QProjectM_MainWindow::createStatusBar()
{
	statusBar()->hide();
	statusBar()->showMessage ( tr ( "Welcome to projectM!" ) );
}

void QProjectM_MainWindow::readSettings()
{

	// The settings exclusively due to qprojectM
	QSettings settings ( "projectM", "qprojectM" );
	QPoint pos = settings.value ( "pos", QPoint ( 200, 200 ) ).toPoint();
	//QSize size = settings.value ( "size", QSize ( 1024, 768 ) ).toSize();

	// Load up where the playlist file dialog last was visited last time
	m_QPlaylistFileDialog->setDirectory
	( settings.value ( "playlistPath", m_QPlaylistFileDialog->directory().absolutePath() ).toString() );


	//resize ( size().width(), size().height() );
	move ( pos );
}

void QProjectM_MainWindow::writeSettings()
{
	QSettings settings ( "projectM", "qprojectM" );
	settings.setValue ( "pos", pos() );
	//settings.setValue ( "size", size() );
	settings.setValue ( "playlistPath", m_QPlaylistFileDialog->directory().absolutePath() );

	if (m_currentPlaylistUrl != QString())
		settings.setValue("PlaylistFile", m_currentPlaylistUrl);

	const QPoint playlistPos = ui->presetPlayListDockWidget->pos();
	const bool playlistDocked = !ui->presetPlayListDockWidget->isFloating();

	settings.setValue("playlistDocked", playlistDocked);
	settings.setValue("playlistPos", playlistPos);
	settings.setValue("playlistWindowSize", ui->presetPlayListDockWidget->size());

	settings.setValue("playlistDockLocation", dockWidgetArea);

}

void QProjectM_MainWindow::loadFile ( const QString &fileName, int rating, int breed, const Nullable<int> & row)
{

	const QString & name = strippedName ( fileName );

	PlaylistItemVector * playlistItems = historyHash.value ( QString(), 0 );
	Q_ASSERT ( playlistItems != 0 );

	if (playlistItems->empty())
		ui->presetSavePushButton->setEnabled(true);

	playlistItemMetaDataHash[playlistItemCounter] =
			PlaylistItemMetaData ( fileName, name, rating, breed, playlistItemCounter) ;


	if (row.hasValue())
		playlistItems->insert(row.value(), playlistItemCounter);
	else
		playlistItems->push_back (playlistItemCounter);
	playlistItemCounter++;

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
	Nullable<unsigned int> activePresetId;

	if (!presetSelected && activePresetIndex->hasValue()) {
		activePresetId =  activePresetIndex->value();
	} else if (presetSelected) {
		const PlaylistItemVector & oldPlaylistItems = *historyHash.value(previousFilter);

		if ((presetIndexBackup >=0) && (presetIndexBackup < oldPlaylistItems.size())) {
			activePresetId = oldPlaylistItems[presetIndexBackup];
		}


	} else {
	}

	playlistModel->clearItems();

	Q_ASSERT(!qprojectM()->presetPositionValid());

	bool presetExistsWithinFilter = false;
	if ( historyHash.contains ( filter ) )
	{
		const PlaylistItemVector & playlistItems = *historyHash.value ( filter );
		for ( PlaylistItemVector::const_iterator pos = playlistItems.begin(); pos != playlistItems.end();++pos )
		{
			const PlaylistItemMetaData & data = playlistItemMetaDataHash[*pos];

			playlistModel->appendRow ( data.url, data.name,  data.rating, data.breedability);

			if (activePresetId.hasValue() && data.id == activePresetId.value()) {
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

			const PlaylistItemMetaData & data = playlistItemMetaDataHash[*pos];

			if ( ( data.name ).contains ( filter, Qt::CaseInsensitive ) )
			{
				playlistModel->appendRow ( data.url, data.name, data.rating, data.breedability);
				if (activePresetId.hasValue() && data.id == activePresetId.value()) {
					qprojectM()->selectPresetPosition(playlistModel->rowCount()-1);
					presetExistsWithinFilter = true;
				}

				playlistItems2->push_back ( *pos );
			}
		}
		historyHash.insert ( filter, playlistItems2 );
	}

	Q_ASSERT(presetExistsWithinFilter == qprojectM()->presetPositionValid());

	previousFilter = filter;
	qprojectMWidget()->releasePresetLock();
}


void QProjectM_MainWindow::presetRatingChanged( unsigned int index, int rating, PresetRatingType ratingType)
{

	PlaylistItemVector & lastCache =  *historyHash[previousFilter];
	const long id = lastCache[index];

	qDebug() << "preset rating changed: " << index << " (rating = " << rating << ")";

	playlistItemMetaDataHash[id].rating = rating;

	ui->presetPlayListDockWidget->setWindowModified ( true );

	playlistModel->notifyDataChanged(index);
}

