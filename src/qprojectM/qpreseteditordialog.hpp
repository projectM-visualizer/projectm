#ifdef QPRESET_EDITOR_DIALOG_HPP
#define  QPRESET_EDITOR_DIALOG_HPP
#include "ui_qpreseteditordialog.h"

class QPresetEditorDialog : {
	
	Q_OBJECT

	public:
		QPresetEditorDialog(QProjectMWidget * widget, QWidget * parent = 0, Qt::WindowFlags f = 0);
	private:		
	private slots:
		void buttonBoxHandler(QAbstractButton * button);

	private:
		QSettings _settings;
		const std::string _configFile;
		QProjectMWidget * _qprojectMWidget;
		Ui::QPresetEditorDialog _ui;	
	
	

}








#endif