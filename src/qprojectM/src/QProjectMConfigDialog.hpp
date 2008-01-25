#ifndef QPROJECTM_CONFIG_DIALOG_HPP
#include "ui_QProjectMConfigDialog.h"
#include "libprojectM/projectM.hpp"

class QProjectMConfigDialog : public QDialog {

	Q_OBJECT

	public:
		QProjectMConfigDialog(const std::string & configFile, projectM * projectM, QWidget * parent = 0, Qt::WindowFlags f = 0);
	private:
		void loadConfig();

	private slots:
		void saveConfig();
	private:
		 const std::string _configFile;
		 projectM & _projectM;
		 Ui::QProjectMConfigDialog _ui;

};
#endif
