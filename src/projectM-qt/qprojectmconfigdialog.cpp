#include "qprojectmconfigdialog.hpp"
#include <QtDebug>
#include <QAction>
#include "qplaylistfiledialog.hpp"
#include <QSettings>
#include "qprojectmwidget.hpp"

QProjectMConfigDialog::QProjectMConfigDialog(const std::string & configFile, QProjectMWidget * qprojectMWidget, QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f), _configFile(configFile), _qprojectMWidget(qprojectMWidget), _settings("projectM", "qprojectM") {
	
	
	_ui.setupUi(this);
		
	QHBoxLayout * hboxLayout = new QHBoxLayout();
	
	hboxLayout->addWidget(_ui.layoutWidget);
	this->setLayout(hboxLayout);
	
//	assert(!layout());
	
//	_ui.layoutWidget->setLayout(_ui.gridLayout);
	
	
	//_ui.setLayout
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
	
	projectM::Settings settings = _qprojectMWidget->qprojectM()->settings();
	
	settings.meshX = _ui.meshSizeWidthSpinBox->value();
	settings.meshY = _ui.meshSizeHeightSpinBox->value();
	settings.windowHeight = _ui.windowHeightSpinBox->value();
	settings.windowWidth = _ui.windowWidthSpinBox->value();
	settings.titleFontURL = _ui.titleFontPathLineEdit->text().toStdString();
	settings.menuFontURL = _ui.menuFontPathLineEdit->text().toStdString();
	settings.presetURL = _ui.startupPlaylistDirectoryLineEdit->text().toStdString();
	settings.textureSize = _ui.textureSizeComboBox->itemData(_ui.textureSizeComboBox->currentIndex()).toInt();
	settings.smoothPresetDuration = _ui.smoothPresetDurationSpinBox->value();
	settings.presetDuration = _ui.presetDurationSpinBox->value();
	settings.fps = _ui.maxFPSSpinBox->value();
	settings.aspectCorrection = _ui.useAspectCorrectionCheckBox->checkState() == Qt::Checked;
	settings.beatSensitivity = _ui.beatSensitivitySpinBox->value();
	settings.easterEgg = _ui.easterEggParameterSpinBox->value();
	settings.shuffleEnabled = _ui.shuffleOnStartupCheckBox->checkState() == Qt::Checked;
	projectM::writeConfig(_configFile, settings);
	
	QSettings qSettings("projectM", "qprojectM");
	
	qSettings.setValue("FullscreenOnStartup", _ui.fullscreenOnStartupCheckBox->checkState() == Qt::Checked);
	qSettings.setValue("MenuOnStartup", _ui.menuOnStartupCheckBox->checkState() == Qt::Checked);
	
	qSettings.setValue("PlaylistFile", _ui.startupPlaylistFileLineEdit->text());

}



void QProjectMConfigDialog::populateTextureSizeComboBox() {

	_ui.textureSizeComboBox->clear();
	for (int textureSize = 1<<1; textureSize < 1<<14; textureSize<<=1) {		
		_ui.textureSizeComboBox->addItem(QString("%1").arg(textureSize), textureSize);		
	}
}

void QProjectMConfigDialog::loadConfig() {
	
	const projectM::Settings & settings = (* _qprojectMWidget->qprojectM()).settings();
	
	_ui.meshSizeWidthSpinBox->setValue(settings.meshX);
	_ui.meshSizeHeightSpinBox->setValue(settings.meshY);
	
	_ui.titleFontPathLineEdit->setText(settings.titleFontURL.c_str());
	_ui.menuFontPathLineEdit->setText(settings.menuFontURL.c_str());
	
	_ui.startupPlaylistDirectoryLineEdit->setText(settings.presetURL.c_str());
	_ui.useAspectCorrectionCheckBox->setCheckState(settings.aspectCorrection ? Qt::Checked : Qt::Unchecked);
	_ui.maxFPSSpinBox->setValue(settings.fps);
	_ui.beatSensitivitySpinBox->setValue(settings.beatSensitivity);
	_ui.windowHeightSpinBox->setValue(settings.windowHeight);
	_ui.windowWidthSpinBox->setValue(settings.windowWidth);
	_ui.shuffleOnStartupCheckBox->setCheckState(settings.shuffleEnabled ? Qt::Checked : Qt::Unchecked);
	 populateTextureSizeComboBox();
	_ui.textureSizeComboBox->insertItem(0, QString("%1").arg(settings.textureSize), settings.textureSize);
	_ui.textureSizeComboBox->setCurrentIndex(0);
	
	_ui.smoothPresetDurationSpinBox->setValue(settings.smoothPresetDuration);
	_ui.presetDurationSpinBox->setValue(settings.presetDuration);
	_ui.easterEggParameterSpinBox->setValue(settings.easterEgg);
	
	
	QSettings qSettings("projectM", "qprojectM");
	_ui.fullscreenOnStartupCheckBox->setCheckState(qSettings.value("FullscreenOnStartup", false).toBool() ? Qt::Checked : Qt::Unchecked);
	_ui.menuOnStartupCheckBox->setCheckState(qSettings.value("MenuOnStartup", false).toBool() ? Qt::Checked : Qt::Unchecked);
	_ui.startupPlaylistFileLineEdit->setText(qSettings.value("PlaylistFile", QString()).toString()	);
	 
	qDebug() << "load config END";
		
}
