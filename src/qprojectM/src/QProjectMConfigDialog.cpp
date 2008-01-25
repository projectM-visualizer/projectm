#include "QProjectMConfigDialog.hpp"
#include <QtDebug>

QProjectMConfigDialog::QProjectMConfigDialog(const std::string & configFile, projectM * projectM, QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f), _configFile(configFile), _projectM(*projectM) {
	
	qDebug() << "!!!";
	_ui.setupUi(this);
	connect(_ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveConfig()));
	loadConfig();
}


void QProjectMConfigDialog::saveConfig() {
	qDebug() << "SAVE config";
	
	projectM::Settings settings = _projectM.settings();
	
	settings.meshX = _ui.meshSizeXComboBox->itemData(_ui.meshSizeXComboBox->currentIndex()).toInt();
	settings.meshY = _ui.meshSizeYComboBox->itemData(_ui.meshSizeYComboBox->currentIndex()).toInt();
	settings.windowHeight = _ui.windowHeightSpinBox->value();
	settings.windowWidth = _ui.windowWidthSpinBox->value();		
	settings.titleFontURL = _ui.titleFontPathLineEdit->text().toStdString();
	settings.menuFontURL = _ui.menuFontPathLineEdit->text().toStdString();
	settings.presetURL = _ui.startupPlaylistLineEdit->text().toStdString();
	settings.textureSize = _ui.textureSizeComboBox->itemData(_ui.textureSizeComboBox->currentIndex()).toInt();
	settings.smoothPresetDuration = _ui.smoothPresetDurationSpinBox->value();
	settings.presetDuration = _ui.presetDurationSpinBox->value();
	settings.fps = _ui.maxFPSSpinBox->value();
	settings.aspectCorrection = _ui.useAspectCorrectionCheckBox->checkState() == Qt::Checked;
	settings.beatSensitivity = _ui.beatSensitivitySpinBox->value();
		
	projectM::writeConfig(_configFile, settings);
	
	qDebug() << "save end";
}


void QProjectMConfigDialog::loadConfig() {
	
	qDebug() << "load config";
	const projectM::Settings & settings = _projectM.settings();
	
	//settings.meshX = _ui.meshSizeXComboBox->itemData(_ui.meshSizeXComboBox->currentIndex()).toInt();
	//settings.meshY = _ui.meshSizeYComboBox->itemData(_ui.meshSizeYComboBox->currentIndex()).toInt();
	
	_ui.titleFontPathLineEdit->setText(settings.titleFontURL.c_str());
	_ui.menuFontPathLineEdit->setText(settings.menuFontURL.c_str());
	
	_ui.startupPlaylistLineEdit->setText(settings.presetURL.c_str());
	_ui.useAspectCorrectionCheckBox->setCheckState(settings.aspectCorrection ? Qt::Checked : Qt::Unchecked);
	_ui.maxFPSSpinBox->setValue(settings.fps);
	_ui.beatSensitivitySpinBox->setValue(settings.beatSensitivity);
	_ui.windowHeightSpinBox->setValue(settings.windowHeight);
	_ui.windowWidthSpinBox->setValue(settings.windowWidth);
	
	//settings.textureSize = _ui.textureSizeComboBox->itemData(_ui.textureSizeComboBox->currentIndex()).toInt();
	_ui.smoothPresetDurationSpinBox->setValue(settings.smoothPresetDuration);
	_ui.presetDurationSpinBox->setValue(settings.presetDuration);
	
	qDebug() << "load config END";
		
}
