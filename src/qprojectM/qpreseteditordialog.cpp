#include "qpreseteditordialog.hpp"
#include <QFile>
#include <QMessageBox>
QPresetEditorDialog::QPresetEditorDialog(QProjectMWidget * widget, QWidget * parent, Qt::WindowFlags f): QDialog(parent,f), _qprojectMWidget(widget) {
	_ui.setupUi(this);
	
	connect(_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxHandler(QAbstractButton*)));
		
}

void QPresetEditorDialog::setPreset(QString url, const QModelIndex & index) {
	
	m_index = index;
	
	disconnect(_ui.presetTextEdit, 0, this, 0);
	
	m_presetUrl = url;	
	_ui.presetTextEdit->loadPresetText(url);
	
	this->setWindowTitle(QString("Preset Editor - %1 [*]").arg(url));
	_ui.presetTextEdit->setWindowModified(false);

	connect(_ui.presetTextEdit, SIGNAL(textChanged()), this, SLOT(updateWindowTitle()));
}


void QPresetEditorDialog::updateWindowTitle()  {
	_ui.presetTextEdit->setWindowModified(true);
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
		QMessageBox::warning (0, "Preset File Error", QString(tr("There was a problem trying to save the preset \"%1\".  The file may no longer exist or you may not have permission to read the file.")).arg(presetUrl()));
		return ;
	}
	
	QTextStream textStream(&qfile);
	
	textStream << _ui.presetTextEdit->toPlainText();
	
	textStream.flush();
}

void QPresetEditorDialog::buttonBoxHandler(QAbstractButton * button) {
	switch (_ui.buttonBox->standardButton(button)) {
		case QDialogButtonBox::Close:
			this->hide();
			break;
		case QDialogButtonBox::Apply:
			qDebug() << "pre file save";
			saveFile();
			qDebug() << "emitting preset mod";
			emit(presetModified(m_index));
			break;
		case QDialogButtonBox::Reset:
			revertBuffer();
			break;
		default:
			break;
	}
}

