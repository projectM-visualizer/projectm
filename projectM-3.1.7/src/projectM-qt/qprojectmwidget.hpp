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

#ifndef QPROJECTM_WIDGET_HPP
#define QPROJECTM_WIDGET_HPP

#include <iostream>
#include "qprojectm.hpp"
#include <QGLWidget>
#include <QMutex>
#include <QtDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QApplication>
#include <QSettings>

class QProjectMWidget : public QGLWidget
{

		Q_OBJECT        // must include this if you use Qt signals/slots

	public:
		static const int MOUSE_VISIBLE_TIMEOUT_MS = 5000;
		QProjectMWidget ( const std::string & config_file, QWidget * parent, QMutex * audioMutex = 0 )
				: QGLWidget ( parent ), m_config_file ( config_file ), m_projectM ( 0 ), m_audioMutex ( audioMutex ), m_mouseTimer ( 0 )
		{

			m_mouseTimer = new QTimer ( this );

			QSettings settings("projectM", "qprojectM");
			mouseHideTimeoutSeconds = 
				settings.value("MouseHideOnTimeout", MOUSE_VISIBLE_TIMEOUT_MS/1000).toInt();
			
			if (mouseHideTimeoutSeconds > 0)
				m_mouseTimer->start ( mouseHideTimeoutSeconds * 1000);

			connect ( m_mouseTimer, SIGNAL ( timeout() ), this, SLOT ( hideMouse() ) );
			this->setMouseTracking ( true );

		}

		~QProjectMWidget() { destroyProjectM(); }



		void resizeGL ( int w, int h )
		{
			// Setup viewport, projection etc
			setup_opengl ( w,h );
			m_projectM->projectM_resetGL ( w, h );
		}

		inline const std::string & configFile()
		{
			return m_config_file;
		}

		inline void seizePresetLock()
		{

			m_presetSeizeMutex.lock();
			m_presetWasLocked = qprojectM()->isPresetLocked();
			qprojectM()->setPresetLock ( true );

		}

		inline void releasePresetLock()
		{
			qprojectM()->setPresetLock ( m_presetWasLocked );
			m_presetSeizeMutex.unlock();
		}


		inline QProjectM * qprojectM() { return m_projectM; }

	protected slots:
		inline void mouseMoveEvent ( QMouseEvent * event )
		{

			m_mouseTimer->stop();
			QApplication::restoreOverrideCursor();
			if (mouseHideTimeoutSeconds > 0)
				m_mouseTimer->start ( mouseHideTimeoutSeconds*1000 );

		}

		inline void leaveEvent ( QEvent * event )
		{
			/// @bug testing if this resolves a bug for ubuntu users
			QApplication::restoreOverrideCursor();
		}

	public slots:

		void resetProjectM()
		{
			std::cout << "reseting" << std::endl;
			qDebug() << "reset start";

			emit ( projectM_BeforeDestroy() );

			if ( m_audioMutex )
				m_audioMutex->lock();

			destroyProjectM();

			// Make a new projectM instance and reset the opengl state
			initializeGL();

			// Allow audio thread to continue its business
			if ( m_audioMutex )
			{
				m_audioMutex->unlock();
			}
			qDebug() << "reinit'ed";
		}

		void setAudioMutex ( QMutex * mutex )
		{
			m_audioMutex = mutex;
		}

		void setPresetLock ( int state )
		{
			m_projectM->setPresetLock ( ( bool ) state );
			emit ( presetLockChanged ( ( bool ) state ) );
		}

		void setShuffleEnabled ( int state )
		{
			m_projectM->setShuffleEnabled ( ( bool ) state );
			emit ( shuffleEnabledChanged ( ( bool ) state ) );
		}

		void mousePressEvent ( QMouseEvent * event )
		{
			this->setFocus();
		}

	signals:
		void projectM_Initialized ( QProjectM * );
		void projectM_BeforeDestroy();
		void presetLockChanged ( bool isLocked );
		void shuffleEnabledChanged ( bool isShuffleEnabled );

	private slots:
		void hideMouse()
		{
			if ( this->underMouse() && this->hasFocus() )
				QApplication::setOverrideCursor ( Qt::BlankCursor );
		}
	private:
		std::string m_config_file;
		QProjectM * m_projectM;
		void destroyProjectM()
		{

			if ( m_projectM )
			{
				delete ( m_projectM );
				m_projectM = 0;
			}
		}

		QTimer * m_mouseTimer;
		QMutex * m_audioMutex;
		QMutex m_presetSeizeMutex;
		bool m_presetWasLocked;
	protected:


		void keyReleaseEvent ( QKeyEvent * e )
		{

			projectMKeycode pkey;
			bool ignore = false;
			switch ( e->key() )
			{
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
					if (e->modifiers() & Qt::ShiftModifier)
						pkey =  PROJECTM_K_R;
					else
						pkey =  PROJECTM_K_r;
					break;				
				case Qt::Key_L:
					pkey =  PROJECTM_K_l;
					ignore = true;
					break;
				case Qt::Key_N:	
					if (e->modifiers() & Qt::ShiftModifier)
						pkey =  PROJECTM_K_N;
					else
						pkey =  PROJECTM_K_n;
					break;
				case Qt::Key_P:
					if (e->modifiers() & Qt::ShiftModifier)
						pkey =  PROJECTM_K_P;
					else
						pkey =  PROJECTM_K_p;
					break;
				case Qt::Key_F5:
					pkey =  PROJECTM_K_F5;
					break;
				case Qt::Key_Plus:
					pkey =  PROJECTM_K_PLUS;
					break;
				case Qt::Key_Minus:
					pkey =  PROJECTM_K_MINUS;
					break;
				case Qt::Key_Equal:
					pkey = PROJECTM_K_EQUALS;
					break;
				default:
					e->ignore();
					return;
			}
			projectMModifier modifier;

			m_projectM->key_handler ( PROJECTM_KEYDOWN, pkey, modifier );
			if ( ignore )
				e->ignore();


		}

		void initializeGL()
		{

		        if (m_projectM == 0) {
			    this->m_projectM = new QProjectM ( m_config_file );
			    projectM_Initialized ( m_projectM );
			}
		}

		inline void paintGL()
		{
			m_projectM->renderFrame();
		}

	private:
		int mouseHideTimeoutSeconds;
		void setup_opengl ( int w, int h )
		{

			/* Our shading model--Gouraud (smooth). */
			glShadeModel ( GL_SMOOTH );
			/* Culling. */
			//    glCullFace( GL_BACK );
			//    glFrontFace( GL_CCW );
			//    glEnable( GL_CULL_FACE );
			/* Set the clear color. */
			glClearColor ( 0, 0, 0, 0 );
			/* Setup our viewport. */
			glViewport ( 0, 0, w, h );
			/*
					* Change to the projection matrix and set
					* our viewing volume.
			*/
			glMatrixMode ( GL_TEXTURE );
			glLoadIdentity();

			//    gluOrtho2D(0.0, (GLfloat) width, 0.0, (GLfloat) height);
			glMatrixMode ( GL_PROJECTION );
			glLoadIdentity();

			//    glFrustum(0.0, height, 0.0,width,10,40);
			glMatrixMode ( GL_MODELVIEW );
			glLoadIdentity();

			glDrawBuffer ( GL_BACK );
			glReadBuffer ( GL_BACK );
			glEnable ( GL_BLEND );

			glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glEnable ( GL_LINE_SMOOTH );
			glEnable ( GL_POINT_SMOOTH );
			glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
//   glClear(GL_COLOR_BUFFER_BIT);

			// glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,texsize,texsize,0);
			//glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,texsize,texsize);
			glLineStipple ( 2, 0xAAAA );
		}


};
#endif
