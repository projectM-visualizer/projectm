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
#ifndef QPROJECTM_CONFIG_DIALOG_HPP
#define QPROJECTM_CONFIG_DIALOG_HPP
#include "ui_qprojectmconfigdialog.h"
#include "qprojectm_mainwindow.hpp"
#include <QSettings>
class QProjectMConfigDialog : public QDialog {

	Q_OBJECT

	public:
		QProjectMConfigDialog(const std::string & configFile, QProjectMWidget * widget, QWidget * parent = 0, Qt::WindowFlags f = 0);
	private:
		void loadConfig();
	private slots:
		void openPlaylistFileDialog();		
		void openPlaylistDirectoryDialog();
	
		void openMenuFontFileDialog();
		void openTitleFontFileDialog();
		void saveConfig();
		void buttonBoxHandler(QAbstractButton * button);

	private:
		 QSettings _settings;
		 const std::string _configFile;
		 QProjectMWidget * _qprojectMWidget;
		 Ui::QProjectMConfigDialog _ui;
		 void populateTextureSizeComboBox();
	signals:
		void projectM_Reset();
};
#endif
