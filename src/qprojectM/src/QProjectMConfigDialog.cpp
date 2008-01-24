#include "QProjectMConfigDialog.hpp"

QProjectMConfigDialog::QProjectMConfigDialog(const std::string & configFile, projectM & projectM, QWidget * parent = 0, Qt::WindowFlags f = 0 ) : QDialog(parent, f), _configFile(configFile), _projectM(projectM) {
	
	_ui.setupUi(this);
	connect(_ui.buttonBox, SIGNAL(accept()), this, SLOT(saveConfig()));
}


void QProjectMConfigDialog::saveConfig() {
	
	projectM::Settings settings = _projectM.settings();
	
	settings.meshX = _ui.meshSizeXComboBox->itemData(_ui.meshSizeXComboBox->currentIndex()).toInt();
	settings.meshY = _ui.meshSizeYComboBox->itemData(_ui.meshSizeYComboBox->currentIndex()).toInt();
	settings.windowHeight = _ui.windowHeightSpinBox->value();
	settings.windowWidth = _ui.windowWidthSpinBox->value();	
	/// @bug menu font, title font, path vs file? 
	settings.titleFontURL = settings.menuFontURL = _ui.fontPathLineEdit->text().toStdString();
	settings.presetURL = _ui.startupPlaylistLineEdit->text().toStdString();
	settings.textureSize = _ui.textureSizeComboBox->itemData(_ui.textureSizeComboBox->currentIndex()).toInt();
	settings.smoothPresetDuration = _ui.smoothPresetDurationSpinBox->value();
	settings.presetDuration = _ui.presetDurationSpinBox->value();
	settings.fps = _ui.maxFPSSpinBox->value();
	settings.aspectCorrection = _ui.useAspectCorrectionCheckBox->checkState() == Qt::Checked;
	settings.beatSensitivity = _ui.beatSensitivitySpinBox->value();
		
	_projectM.writeConfig(_configFile, settings);	
}


void QProjectMConfigDialog::loadConfig() {
	
}
