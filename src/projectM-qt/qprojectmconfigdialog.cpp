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
#include "qprojectmconfigdialog.hpp"
#include <QtDebug>
#include <QAction>
#include "qplaylistfiledialog.hpp"
#include <QSettings>
#include "qprojectmwidget.hpp"

QProjectMConfigDialog::QProjectMConfigDialog(const QString& configFile, QProjectMWidget * qprojectMWidget, QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f), _settings("projectM", "qprojectM"), _configFile(configFile), _qprojectMWidget(qprojectMWidget) {


	_ui.setupUi(this);

	QHBoxLayout * hboxLayout = new QHBoxLayout();

	hboxLayout->addWidget(_ui.layoutWidget);
	this->setLayout(hboxLayout);

	connect(_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxHandler(QAbstractButton*)));
	connect(this, SIGNAL(projectM_Reset()), _qprojectMWidget, SLOT(resetProjectM()));
	connect (_ui.startupPlaylistFileToolButton, SIGNAL(clicked()), this, SLOT(openPlaylistFileDialog()));
	connect (_ui.startupPlaylistDirectoryToolButton, SIGNAL(clicked()), this, SLOT(openPlaylistDirectoryDialog()));
	connect (_ui.titleFontPathToolButton, SIGNAL(clicked()), this, SLOT(openTitleFontFileDialog()));
	connect (_ui.menuFontPathToolButton, SIGNAL(clicked()), this, SLOT(openMenuFontFileDialog()));
	loadConfig();
}

void QProjectMConfigDialog::buttonBoxHandler(QAbstractButton * button) {

	switch (_ui.buttonBox->standardButton(button)) {
		case QDialogButtonBox::Close:
			this->hide();
			break;
		case QDialogButtonBox::Save:
			saveConfig();
#ifdef PROJECTM_RESET_IS_THREAD_SAFE
			emit(projectM_Reset());
#endif
			break;
		case QDialogButtonBox::Reset:
			loadConfig();
			break;
		default:
			break;
	}
}

void QProjectMConfigDialog::openPlaylistFileDialog() {

	QPlaylistFileDialog dialog(this);

	dialog.setAllowFileSelect(true);
	dialog.setAllowDirectorySelect(false);

	if (dialog.exec())
	{
		Q_ASSERT(!dialog.selectedFiles().empty());
		_ui.startupPlaylistFileLineEdit->setText(dialog.selectedFiles()[0]);

	}
}


void QProjectMConfigDialog::openPlaylistDirectoryDialog() {

	QPlaylistFileDialog dialog(this);

	dialog.setAllowFileSelect(false);
	dialog.setAllowDirectorySelect(true);

	if (dialog.exec())
	{
		Q_ASSERT(!dialog.selectedFiles().empty());
		_ui.startupPlaylistDirectoryLineEdit->setText(dialog.selectedFiles()[0]);

	}
}
void QProjectMConfigDialog::openMenuFontFileDialog() {


	QFileDialog dialog(this, "Select a menu font", _settings.value("Menu Font Directory", QString()).toString(), "True Type Fonts (*.ttf)" );
	dialog.setFileMode(QFileDialog::ExistingFile);

	if (dialog.exec()) {
		Q_ASSERT(!dialog.selectedFiles().empty());
		_ui.menuFontPathLineEdit->setText(dialog.selectedFiles()[0]);

		_settings.setValue("Menu Font Directory", dialog.directory().absolutePath());
	}


}

void QProjectMConfigDialog::openTitleFontFileDialog() {
	QFileDialog dialog(this, "Select a title font", _settings.value("Title Font Directory", QString()).toString(), "True Type Fonts (*.ttf)" );
	dialog.setFileMode(QFileDialog::ExistingFile);

	if (dialog.exec()) {
		Q_ASSERT(!dialog.selectedFiles().empty());
		_ui.titleFontPathLineEdit->setText(dialog.selectedFiles()[0]);
		_settings.setValue("Title Font Directory", dialog.directory().absolutePath());
	}

}

void QProjectMConfigDialog::saveConfig() {
    // Will only keep data_dir.
    auto settings = projectm_get_settings(_qprojectMWidget->qprojectM()->instance());

    projectm_free_string(settings->title_font_url);
    projectm_free_string(settings->menu_font_url);
    projectm_free_string(settings->preset_url);

	settings->mesh_x = _ui.meshSizeWidthSpinBox->value();
	settings->mesh_y = _ui.meshSizeHeightSpinBox->value();
	settings->window_height = _ui.windowHeightSpinBox->value();
	settings->window_width = _ui.windowWidthSpinBox->value();
	settings->title_font_url = projectm_alloc_string(_ui.titleFontPathLineEdit->text().length() + 1);
	strncpy(settings->title_font_url, _ui.titleFontPathLineEdit->text().toLocal8Bit().data(), _ui.titleFontPathLineEdit->text().length());
	settings->menu_font_url = projectm_alloc_string(_ui.menuFontPathLineEdit->text().length() + 1);
	strncpy(settings->menu_font_url, _ui.menuFontPathLineEdit->text().toLocal8Bit().data(), _ui.menuFontPathLineEdit->text().length());
	settings->preset_url = projectm_alloc_string(_ui.startupPlaylistDirectoryLineEdit->text().length() + 1);
	strncpy(settings->preset_url, _ui.startupPlaylistDirectoryLineEdit->text().toLocal8Bit().data(), _ui.startupPlaylistDirectoryLineEdit->text().length());
	settings->texture_size = _ui.textureSizeComboBox->itemData(_ui.textureSizeComboBox->currentIndex()).toInt();
	settings->soft_cut_duration = _ui.smoothPresetDurationSpinBox->value();
	settings->preset_duration = _ui.presetDurationSpinBox->value();
	settings->fps = _ui.maxFPSSpinBox->value();
	settings->aspect_correction = _ui.useAspectCorrectionCheckBox->checkState() == Qt::Checked;
	settings->beat_sensitivity = _ui.beatSensitivitySpinBox->value();
	settings->easter_egg = _ui.easterEggParameterSpinBox->value();
	settings->shuffle_enabled = _ui.shuffleOnStartupCheckBox->checkState() == Qt::Checked;
	settings->soft_cut_ratings_enabled = _ui.softCutRatingsEnabledCheckBox->checkState() == Qt::Checked;

	projectm_write_config(_configFile.toLocal8Bit().data(), settings);

    projectm_free_settings(settings);

	QSettings qSettings("projectM", "qprojectM");

	qSettings.setValue("FullscreenOnStartup", _ui.fullscreenOnStartupCheckBox->checkState() == Qt::Checked);
	qSettings.setValue("MenuOnStartup", _ui.menuOnStartupCheckBox->checkState() == Qt::Checked);

	qSettings.setValue("PlaylistFile", _ui.startupPlaylistFileLineEdit->text());

	qSettings.setValue("MouseHideOnTimeout", _ui.mouseHideTimeoutSpinBox->value());
}



void QProjectMConfigDialog::populateTextureSizeComboBox() {

	_ui.textureSizeComboBox->clear();
	for (int textureSize = 1<<1; textureSize < 1<<14; textureSize<<=1) {
		_ui.textureSizeComboBox->addItem(QString("%1").arg(textureSize), textureSize);
	}
}

void QProjectMConfigDialog::loadConfig() {

    auto settings = projectm_get_settings(_qprojectMWidget->qprojectM()->instance());

	_ui.meshSizeWidthSpinBox->setValue(settings->mesh_x);
	_ui.meshSizeHeightSpinBox->setValue(settings->mesh_y);

	_ui.titleFontPathLineEdit->setText(settings->title_font_url);
	_ui.menuFontPathLineEdit->setText(settings->menu_font_url);

	_ui.startupPlaylistDirectoryLineEdit->setText(settings->preset_url);
	_ui.useAspectCorrectionCheckBox->setCheckState(settings->aspect_correction ? Qt::Checked : Qt::Unchecked);
	_ui.maxFPSSpinBox->setValue(settings->fps);
	_ui.beatSensitivitySpinBox->setValue(settings->beat_sensitivity);
	_ui.windowHeightSpinBox->setValue(settings->window_height);
	_ui.windowWidthSpinBox->setValue(settings->window_width);
	_ui.shuffleOnStartupCheckBox->setCheckState(settings->shuffle_enabled ? Qt::Checked : Qt::Unchecked);
	 populateTextureSizeComboBox();
	_ui.textureSizeComboBox->insertItem(0, QString("%1").arg(settings->texture_size), settings->texture_size);
	_ui.textureSizeComboBox->setCurrentIndex(0);

	_ui.smoothPresetDurationSpinBox->setValue(settings->soft_cut_duration);
	_ui.presetDurationSpinBox->setValue(settings->preset_duration);
	_ui.easterEggParameterSpinBox->setValue(settings->easter_egg);
	_ui.softCutRatingsEnabledCheckBox->setCheckState(settings->soft_cut_ratings_enabled ? Qt::Checked : Qt::Unchecked);

	projectm_free_settings(settings);

	QSettings qSettings("projectM", "qprojectM");
	_ui.fullscreenOnStartupCheckBox->setCheckState(qSettings.value("FullscreenOnStartup", false).toBool() ? Qt::Checked : Qt::Unchecked);
	_ui.menuOnStartupCheckBox->setCheckState(qSettings.value("MenuOnStartup", false).toBool() ? Qt::Checked : Qt::Unchecked);
	_ui.startupPlaylistFileLineEdit->setText(qSettings.value("PlaylistFile", QString()).toString()	);
	_ui.mouseHideTimeoutSpinBox->setValue(qSettings.value("MouseHideOnTimeout", 5).toInt());

}
