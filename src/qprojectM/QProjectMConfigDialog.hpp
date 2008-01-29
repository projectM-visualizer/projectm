#ifndef QPROJECTM_CONFIG_DIALOG_HPP
#define QPROJECTM_CONFIG_DIALOG_HPP
#include "ui_QProjectMConfigDialog.h"
#include "QProjectM_MainWindow.hpp"

class QProjectMConfigDialog : public QDialog {

	Q_OBJECT

	public:
		QProjectMConfigDialog(const std::string & configFile, QProjectMWidget * widget, QWidget * parent = 0, Qt::WindowFlags f = 0);
	private:
		void loadConfig();
	private slots:
		void openPlaylistFileDialog();
		void openFontFileDialog();
		void saveConfig();
		void buttonBoxHandler(QAbstractButton * button);

	private:
		 const std::string _configFile;
		 QProjectMWidget * _qprojectMWidget;
		 Ui::QProjectMConfigDialog _ui;
		 void populateTextureSizeComboBox();
	signals:
		void projectM_Reset();
};
#endif
