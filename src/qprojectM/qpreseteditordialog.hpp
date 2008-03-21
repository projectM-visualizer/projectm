#ifndef QPRESET_EDITOR_DIALOG_HPP
#define QPRESET_EDITOR_DIALOG_HPP
#include "ui_qpreseteditordialog.h"
class QProjectMWidget;
#include <QModelIndex>
class QPresetEditorDialog : public QDialog {
	
	Q_OBJECT

	public:
		QPresetEditorDialog(QProjectMWidget * widget, QWidget * parent = 0, Qt::WindowFlags f = 0);
		void setPreset(QString url, int rowIndex);
		const QString & presetUrl() const;
		
		inline int presetIndex() const {
			return m_index;
		}
	signals:
		void presetModified(int rowIndex);
	protected:	
		//void keyReleaseEvent(QKeyEvent * e);
	public slots:
		

	private slots:
		void buttonBoxHandler(QAbstractButton * button);
		void saveFile();
		void saveAndNotify();
		void updateWindowTitle();

	private:
		int m_index;
		void revertBuffer();
		QProjectMWidget * _qprojectMWidget;
		Ui::QPresetEditorDialog _ui;
		QString	m_presetUrl;
		
	
	

};
#endif
