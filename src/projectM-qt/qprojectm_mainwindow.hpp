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

#ifndef QPROJECTM_MAINWINDOW_H
#define QPROJECTM_MAINWINDOW_H
#define CONFIG_FILE "/share/projectM/config.inp"

#include <QMainWindow>
#include <QCloseEvent>
#include <cassert>
#include <QGLWidget>
#include <QHash>
#include <QtDebug>
#include <QMutex>

#include "projectM.hpp"

#include <QModelIndexList>
class QHeaderView;
class QAction;
class QMenu;
class QPresetFileDialog;
class QPlaylistFileDialog;
class QPlaylistModel;
class QModelIndex;
class QProjectMConfigDialog;
class QPresetEditorDialog;
class QProjectM;
class QProjectMWidget;

template <class T>
class Nullable;

namespace Ui {
class QProjectM_MainWindow;
}

#include <iostream>


class QProjectM_MainWindow:public QMainWindow
{
      Q_OBJECT

	signals:
		void shuttingDown();
	public:
	
	projectM * GetProjectM();

      typedef struct PlaylistItemMetaData {
		PlaylistItemMetaData() {}
		PlaylistItemMetaData(const QString & _url, const QString & _name, int _rating, int _breed, long _id):
			url(_url), name(_name), rating(_rating), breedability(_breed), id(_id) {}

		QString url;
		QString name;		
		int rating;
		int breedability;
		long id;
		inline bool operator==(const PlaylistItemMetaData & data) {
			return data.id == id;
		}
		
		
		inline bool operator==(long rhs_id) {
			return rhs_id == id;
		}
      } PlaylistItemMetaData;


      void addPCM(float * buffer, unsigned int bufferSize);

		
      typedef QVector<long> PlaylistItemVector;

      QProjectM_MainWindow(const std::string & config_file, QMutex * audioMutex);
      virtual ~QProjectM_MainWindow();
      void registerSettingsAction(QAction * action);
      void unregisterSettingsAction(QAction * action);
      void setMenuVisible(bool visible);
      void setMenuAndStatusBarsVisible(bool visible);
      void keyReleaseEvent ( QKeyEvent * e );
      QProjectM * qprojectM();
      void refreshPlaylist();

      QProjectMWidget * qprojectMWidget() { return m_QProjectMWidget; }


      void registerAction(const QAction &);

	public slots:
protected:


            void closeEvent(QCloseEvent *event);

private slots:
void dockLocationChanged(Qt::DockWidgetArea area);
void presetRatingChanged(unsigned int, int, PresetRatingType);
void presetHardCut();
void presetSoftCut();
	void insertPresetsDialog() ;
	void removeSelectedPlaylistItems();

	void openPresetEditorDialogForSelectedPreset();
	  void popupPlaylistContextMenu(QMouseEvent *, const QModelIndexList & items);
      void hotkeyReference();
      void selectPlaylistItem ( int rowIndex);
      void clearPlaylistModel();
      void openPresetEditorDialog(int rowIndex);
      void aboutQt();

      void clearPlaylist();
      void addPresetsDialog();
      void addPresetsDialog(const QModelIndex & index);
      void openPlaylistDialog();
      void savePlaylistButtonClick();
      void savePlaylistAsDialog();
      void about();
      void postProjectM_Initialize();
      void updatePlaylistSelection(bool hardCut, unsigned int index);
      void selectPlaylistItem(const QModelIndex & index);
      void changePresetAttribute(const QModelIndex & index);
      void openSettingsDialog();
      void updateFilteredPlaylist(const QString & text);
      void refreshHeaders(QResizeEvent * event = 0);
      void removePlaylistItems(const QModelIndexList & items);
      void dragAndDropPlaylistItems(const QModelIndexList &, const QModelIndex &);

	
      private:
	void readPlaylistSettings();
	bool warnIfPlaylistModified();
	
	void insertPlaylistItem(const PlaylistItemMetaData & data, int targetIndex);

	void savePlaylist();
	void updatePlaylistUrl(const QString & url);
	unsigned long playlistItemCounter;
	QSize _oldPlaylistSize;	
	void readConfig(const std::string & configFile);
	void writeConfig();
	void copyPlaylist();
	
	QPresetEditorDialog * m_QPresetEditorDialog;
	QHeaderView * hHeader;
	QHeaderView * vHeader;
      	QString m_currentPlaylistUrl;
	QModelIndexList selectedPlaylistIndexes;
	
      QPlaylistModel * playlistModel;
      Ui::QProjectM_MainWindow * ui;
      QProjectMConfigDialog * configDialog;

      QHash<long, PlaylistItemMetaData> playlistItemMetaDataHash;

      QHash<QString, PlaylistItemVector*> historyHash;
      Nullable<long> * activePresetIndex;

      bool _menuVisible;
      bool _menuAndStatusBarsVisible;
      QTimer * m_timer;
	
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void readSettings();
      void writeSettings();
      void loadFile(const QString &fileName, int rating , int breed, const Nullable<int> & row);
      QString strippedName(const QString &fullFileName);

      QProjectMWidget * m_QProjectMWidget;

      QMenu *playlistContextMenu;
      QToolBar *fileToolBar;

      QAction *openAct;;
      QAction *exitAct;
      QAction *aboutAct;
      QAction *aboutQtAct;

      QString previousFilter;	
      Qt::DockWidgetArea dockWidgetArea;

      QPresetFileDialog * m_QPresetFileDialog;
      QPlaylistFileDialog * m_QPlaylistFileDialog;

};

#endif
