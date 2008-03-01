/***************************************************************************
 *   Copyright (C) 2007 by carm   *
 *   carm@localhost   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef QPLAYLIST_FILEDIALOG_H
#define QPLAYLIST_FILEDIALOG_H

#include <QFileDialog>
#include <QStringList>
#include <QtDebug>

 class QPlaylistFileDialog : public QFileDialog
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     inline QPlaylistFileDialog(QWidget * parent = 0): 
		QFileDialog(parent, "Open a playlist file or directory", QString()) {

	     updateFileMode(selectedFiles());
	     
	     //connect(this, SIGNAL(filesSelected(const QStringList&)),
		//     this, SLOT(updateFileMode(const QStringList&)));
	     
	     connect(this, SIGNAL(currentChanged(const QString&)),
		     this, SLOT(updateFileMode(const QString&)));
	}
	
	void setAllowDirectorySelect(bool isAllowed) {
		m_directorySelectAllowed = isAllowed;
		updateFileMode(selectedFiles());
				
	}
	
	
	void setAllowFileSelect(bool isAllowed) {
		m_fileSelectAllowed = isAllowed;	
		updateFileMode(selectedFiles());
	}
	
	
	bool isFileSelectAllowed() const {
		return m_fileSelectAllowed;	
	}
	
	
	bool isDirectorySelectAllowed() const {
		return m_directorySelectAllowed;		
	}
	
       ~QPlaylistFileDialog() { }

	 private:
		 bool m_directorySelectAllowed;
		 bool m_fileSelectAllowed;
		 
 private slots:
	 
	 void updateFileMode(const QString & fileName) {
		 
		 QString filter;
		 
		 if (isDirectorySelectAllowed()) {
			 
			 filter += "Directories";
		 }
		 
		 if (isFileSelectAllowed()) {			 
			 if (filter != QString())
				 filter += " and ";
		
			 filter += "Preset Playlists (*.ppl)";
			 //this->setFileMode(QFileDialog::ExistingFiles);
		 } 
		 
		
		 if (QFileInfo(fileName).isDir())
			 if (isDirectorySelectAllowed())  
			 	this->setFileMode(QFileDialog::Directory);
		 	 else
				 ;
         	else
			 if (isFileSelectAllowed())
				this->setFileMode(QFileDialog::ExistingFiles);
			 else
				;
			
		 this->setFilter(filter);		 
	 }
	 
	 void updateFileMode(const QStringList & selectedFiles) {
		 qDebug() << "in update";
		 if (selectedFiles.empty())
			 return;
		 updateFileMode(selectedFiles[0]);
	}
};



#endif
