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

#include "qpreseteditordialog.hpp"
#include <QFile>
#include <QMessageBox>
#include <QKeyEvent>

QPresetEditorDialog::QPresetEditorDialog(QProjectMWidget * widget, QWidget * parent, Qt::WindowFlags f): QDialog(parent,f), _qprojectMWidget(widget) {
	_ui.setupUi(this);
	
	QHBoxLayout * hboxLayout = new QHBoxLayout();
	
	hboxLayout->addWidget(_ui.layoutWidget);
	this->setLayout(hboxLayout);
	
	connect(_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxHandler(QAbstractButton*)));
	
}

void QPresetEditorDialog::setPreset(QString url, int rowIndex) {
	
	disconnect(_ui.presetTextEdit, 0, this, 0);

	m_index = rowIndex;
	
	m_presetUrl = url;	
	_ui.presetTextEdit->loadPresetText(url);
	
	this->setWindowTitle(QString("Preset Editor - %1 [*]").arg(url));
	this->setWindowModified(false);

	connect(_ui.presetTextEdit, SIGNAL(textChanged()), this, SLOT(updateWindowTitle()));
	connect(_ui.presetTextEdit, SIGNAL(applyRequested()), this, SLOT(saveAndNotify()), Qt::DirectConnection);
}


void QPresetEditorDialog::updateWindowTitle()  {
	this->setWindowModified(true);
}

const QString & QPresetEditorDialog::presetUrl() const {
	return m_presetUrl;
}

void QPresetEditorDialog::revertBuffer() {
	setPreset(m_presetUrl, m_index);
}

void QPresetEditorDialog::saveFile() {
	QFile qfile(presetUrl());
	
	if (!qfile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QMessageBox::warning (0, "Preset File Error", QString(tr("There was a problem trying to save the preset \"%1\".  You may not have permission to write to the file.")).arg(presetUrl()));
		return ;
	}
	
	QTextStream textStream(&qfile);
	
	textStream << _ui.presetTextEdit->toPlainText();
	
	textStream.flush();
	this->setWindowModified(false);
}

void QPresetEditorDialog::saveAndNotify() {

	qDebug() << "save and notify";
	saveFile();
	
	emit(presetModified(m_index));
}

void QPresetEditorDialog::buttonBoxHandler(QAbstractButton * button) {
	switch (_ui.buttonBox->standardButton(button)) {
		case QDialogButtonBox::Close:
			this->hide();
			break;
		case QDialogButtonBox::Apply:
			saveAndNotify();
			break;
		case QDialogButtonBox::Reset:
			revertBuffer();
			break;
		default:
			break;
	}
}

