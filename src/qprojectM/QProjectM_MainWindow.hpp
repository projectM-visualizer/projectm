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

template <class T>
class Nullable;

namespace Ui { 
class QProjectM_MainWindow;
}

#include <iostream>


class QProjectM : public QObject, public projectM {
	Q_OBJECT

	public:
		 QProjectM(const std::string & config_file):projectM(config_file) {} 

		 void presetSwitchedEvent(bool hardCut, unsigned int index) const {
			presetSwitchedSignal(hardCut, index);
		 }
	signals:
		void presetSwitchedSignal(bool hardCut, unsigned int index) const;
	public slots:
			
};

class QProjectMWidget : public QGLWidget
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     QProjectMWidget(const std::string & _config_file, QWidget *parent, QMutex * audioMutex = 0)
         : QGLWidget(parent), config_file(_config_file), m_projectM(0), m_audioMutex(audioMutex) {}
     
     ~QProjectMWidget() { destroyProjectM(); }

     inline const std::string & configFile() {
	     return config_file;
     }
 
     inline void seizePresetLock() {

	     m_presetSeizeMutex.lock();
	     m_presetWasLocked = qprojectM()->isPresetLocked();
	     qprojectM()->setPresetLock(true);

     }
	
     inline void releasePresetLock() {

		qprojectM()->setPresetLock(m_presetWasLocked);
		m_presetSeizeMutex.unlock();
     }
     inline QProjectM * qprojectM() { return m_projectM; }

	
public slots:
	
     void resetProjectM() {
	
	qDebug() << "reset start";
	// First wait for audio thread to stop by{{{
	// waiting on it's mutex
//	s_audioMutex.tryLock(20000);
	if (m_audioMutex) {
		qDebug() << "lock set start!";
		m_audioMutex->lock();
		qDebug() << "lock set end!";
	}
	// Now destroy the projectM instance
	destroyProjectM();
	
	// Make a new projectM instance and reset the opengl state
	initializeGL();
	
	// Allow audio thread to continue its business
	if (m_audioMutex) {
	//	m_audioMutex->unlock();
	//	qDebug() << "UNLOCKED";
	}
	qDebug() << "reinit'ed";
     }

     
     void setAudioMutex(QMutex * mutex) {
	     m_audioMutex = mutex;	
     }
     
     void setPresetLock(int state) {
		m_projectM->setPresetLock((bool)state);
		emit(presetLockChanged((bool)state));
     }
     
     void setShuffleEnabled(int state) {
	     m_projectM->setShuffleEnabled((bool)state);
	     emit(shuffleEnabledChanged((bool)state));
     }
     
     void mousePressEvent ( QMouseEvent * event )   {
	     this->setFocus();
     }
     
  signals:
	void projectM_BeforeDestroy();
	void projectM_Initialized(QProjectM *);
	void presetLockChanged(bool isLocked);
	void shuffleEnabledChanged(bool isShuffleEnabled);
 private:
	std::string config_file;
	QProjectM * m_projectM;	 
		 void destroyProjectM() {
			 emit(projectM_BeforeDestroy());
	
			 if (m_projectM) {	
				 delete(m_projectM);
				 m_projectM = 0;
			 }
		 }
	
	QMutex * m_audioMutex;
	QMutex m_presetSeizeMutex;
	bool m_presetWasLocked;
 protected:


void keyReleaseEvent ( QKeyEvent * e )  {
	
    projectMKeycode pkey;
    bool ignore = false;
    switch (e->key()) {
	case Qt::Key_F4:
		pkey =  PROJECTM_K_F4;
		break;
	case Qt::Key_F3:
		pkey =  PROJECTM_K_F3;
		break;
	case Qt::Key_F2:
		pkey =  PROJECTM_K_F2;
		break;
	case Qt::Key_F1:
		pkey =  PROJECTM_K_F1;
		break;
	case Qt::Key_R:
		pkey =  PROJECTM_K_r;
		break;
	case Qt::Key_L:
		pkey =  PROJECTM_K_l;
		ignore = true;
		break;	
	case Qt::Key_N:
  		pkey =  PROJECTM_K_n;
		break;
	case Qt::Key_P:	
		pkey =  PROJECTM_K_p;
		break;		
	default:
		e->ignore();
		return;
    }
       projectMModifier modifier;

	m_projectM->key_handler(PROJECTM_KEYDOWN, pkey, modifier);
	if (ignore)
		e->ignore();

	
     }

     void initializeGL()
     {
	this->m_projectM = new QProjectM(config_file);
	projectM_Initialized(m_projectM);
     }

     void resizeGL(int w, int h)
     {
        // Setup viewport, projection etc	
	setup_opengl(w,h);
	m_projectM->projectM_resetGL(  w, h ); 
     }

     inline void paintGL()
     {
   	m_projectM->renderFrame();
     }

   private:

 void setup_opengl( int w, int h )
{

    /* Our shading model--Gouraud (smooth). */
     glShadeModel( GL_SMOOTH);
    /* Culling. */
    //    glCullFace( GL_BACK );
    //    glFrontFace( GL_CCW );
    //    glEnable( GL_CULL_FACE );
    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );
    /* Setup our viewport. */
     glViewport( 0, 0, w, h );
    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    //    gluOrtho2D(0.0, (GLfloat) width, 0.0, (GLfloat) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  
 
  //    glFrustum(0.0, height, 0.0,width,10,40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDrawBuffer(GL_BACK); 
  glReadBuffer(GL_BACK); 
  glEnable(GL_BLEND); 

     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
     // glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//   glClear(GL_COLOR_BUFFER_BIT);
 
  // glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,texsize,texsize,0);
  //glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,texsize,texsize);
   glLineStipple(2, 0xAAAA);
}


 };

class QProjectM_MainWindow:public QMainWindow
{
      Q_OBJECT

public:
      typedef struct PlaylistItemMetaData {
		PlaylistItemMetaData() {}
		PlaylistItemMetaData(long _id) :id(_id)  {}
		PlaylistItemMetaData(const QString & _url, const QString & _name, int _rating, long _id):
			url(_url), name(_name), rating(_rating), id(_id) {}

		QString url;
		QString name;
		int rating;
		long id;
		inline bool operator==(const PlaylistItemMetaData & data) {
			return data.id == id;
		}
		
		
		inline bool operator==(long rhs_id) {
			return rhs_id == id;
		}
      } PlaylistItemMetaData;

      typedef QVector<PlaylistItemMetaData> PlaylistItemVector;

      QProjectM_MainWindow(const std::string & config_file, QMutex * audioMutex);
      virtual ~QProjectM_MainWindow();
      void registerSettingsAction(QAction * action);
      void unregisterSettingsAction(QAction * action);
      void setMenuVisible(bool visible);

      
      void keyReleaseEvent ( QKeyEvent * e );
      QProjectM * qprojectM();
      void refreshPlaylist();

      QProjectMWidget * qprojectMWidget() { return m_QProjectMWidget; }


      void registerAction(const QAction &);

protected:


            void closeEvent(QCloseEvent *event);

private slots:

      void aboutQt();
      void clearPlaylist();
      void addPresets();
      void openPlaylist();
      void savePlaylist();
      void about();
      void postProjectM_Initialize();
      void updatePlaylistSelection(bool hardCut, unsigned int index);
      void selectPlaylistItem(const QModelIndex & index);
      void changeRating(const QModelIndex & index);
      void openSettingsDialog();
      void updateFilteredPlaylist(const QString & text);
      void refreshHeaders(QResizeEvent * event = 0);
      void removePlaylistItems(const QModelIndexList & items);
      private:
	
	unsigned long playlistItemCounter;
	QSize _oldPlaylistSize;	
	void readConfig(const std::string & configFile);
	void writeConfig();
	void copyPlaylist();
	
	QHeaderView * hHeader;
	QHeaderView * vHeader;
      QString m_currentPlaylistFile;

      QPlaylistModel * playlistModel;
      Ui::QProjectM_MainWindow * ui;
      QProjectMConfigDialog * configDialog;

      QHash<QString, PlaylistItemVector*> historyHash;
      Nullable<long> * activePresetIndex;

      bool _menuVisible;
      QTimer * m_timer;
	
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void readSettings();
      void writeSettings();
      void loadFile(const QString &fileName, int rating = 3);
      QString strippedName(const QString &fullFileName);

      QProjectMWidget * m_QProjectMWidget;

      QMenu *fileMenu;
      QMenu *helpMenu;
      QToolBar *fileToolBar;

      QAction *openAct;;
      QAction *exitAct;
      QAction *aboutAct;
      QAction *aboutQtAct;

      QString previousFilter;

      QPresetFileDialog * m_QPresetFileDialog;
      QPlaylistFileDialog * m_QPlaylistFileDialog;

};

#endif
