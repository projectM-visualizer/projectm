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

#ifndef QPRESET_FILEDIALOG_H
#define QPRESET_FILEDIALOG_H


#define CONFIG_FILE "/share/projectM/config.inp"

#include <QMainWindow>
#include <QCloseEvent>
#include <cassert>
#include <projectM.hpp>

#include <iostream>
#include <QFileDialog>

 class QPresetFileDialog : public QFileDialog
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     QPresetFileDialog(QWidget * parent = 0): 
		QFileDialog(parent, "Add preset files", QString(), "Presets (*.prjm *.milk *.so)" ) {

		this->setFileMode(QFileDialog::ExistingFiles);
	}
	
     ~QPresetFileDialog() { }
 private:
	
	
};
#endif
