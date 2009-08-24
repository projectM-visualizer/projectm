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

#ifndef QPRESET_EDITOR_DIALOG_HPP
#define QPRESET_EDITOR_DIALOG_HPP
#include "ui_qpreseteditordialog.h"
class QProjectMWidget;
#include <QModelIndex>
class QPresetEditorDialog : public QDialog {
	
	Q_OBJECT

	public:
		QPresetEditorDialog(QProjectMWidget * widget, QWidget * parent = 0, Qt::WindowFlags f = 0);
		void setPreset(QString url, int rowIndex);
		const QString & presetUrl() const;
		
		inline int presetIndex() const {
			return m_index;
		}
	signals:
		void presetModified(int rowIndex);
	protected:	
		//void keyReleaseEvent(QKeyEvent * e);
	public slots:
		

	private slots:
		void buttonBoxHandler(QAbstractButton * button);
		void saveFile();
		void saveAndNotify();
		void updateWindowTitle();

	private:
		int m_index;
		void revertBuffer();
		QProjectMWidget * _qprojectMWidget;
		Ui::QPresetEditorDialog _ui;
		QString	m_presetUrl;
		
	
	

};
#endif
