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
