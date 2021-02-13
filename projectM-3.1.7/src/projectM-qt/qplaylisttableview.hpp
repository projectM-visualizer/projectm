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

#ifndef QPLAYLIST_TABLEVIEW_HPP
#define QPLAYLIST_TABLEVIEW_HPP

#include <QTableView>
#include <QtDebug>
#include <QKeyEvent>
#include <QMap>
#include <QList>
#include <QMenu>
class QPlaylistTableView : public QTableView
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     inline QPlaylistTableView(QWidget * parent = 0): 
		QTableView(parent) {}
     
	
 signals:
	 void mousePressed(QMouseEvent*, const QModelIndexList & items);
	 void resized(QResizeEvent * event);
	 void deletesRequested(const QModelIndexList & items);
	 void internalDragAndDropRequested(const QModelIndexList & items, const QModelIndex & target = QModelIndex());
	 	 
 public slots:
	 void dropEvent ( QDropEvent * event )  {
		 
		 QModelIndex dropIndex = this->indexAt(event->pos());
		 
		 if (!dropIndex.isValid()) {
			event->ignore();
			return;
		 }
		 
		 const QModelIndexList & items = selectedIndexes();
		 
		 if (items.empty()) {
			 event->ignore();
			 return;			 
		 }
		 
		 int i = items[0].row();
		 
		 // Don't drag and drop into same slot
		 if (i == dropIndex.row()) {
			event->ignore();
			return;
		 }

		 m_firstContiguousBlock.clear();
		 foreach (QModelIndex index, items) {
			if (index.row() == i)
				m_firstContiguousBlock.append(index);
			else
				break;
			i++;
		 }
		 		 
		 emit(internalDragAndDropRequested(m_firstContiguousBlock, dropIndex));
		 event->accept();
	 }
    
	 
	 void dragLeaveEvent ( QDragLeaveEvent * event )  {
		 //qDebug() << "drag leave";
	 }
	 
	 void dragEnterEvent ( QDragEnterEvent * event )  {
		 //qDebug() << "drag enter event";		 
		 event->accept();
		 
	 }
	 
	 
	 void dragMoveEvent ( QDragMoveEvent * event )  {
		 //qDebug() << "drag move event";
		 event->accept();
	 }
	 
	 inline void resizeEvent(QResizeEvent * event) {
		QTableView::resizeEvent(event);
		emit(resized(event));
	 }	
	 
	 inline void mousePressEvent(QMouseEvent * event) {
		QAbstractItemView::mousePressEvent(event);
		if (event->button() == Qt::RightButton) {
			emit(mousePressed(event, selectedIndexes()));
		}
		else
				;
	 }
	 
	inline void keyReleaseEvent(QKeyEvent * event) {
		 
		switch (event->key()) {
			case Qt::Key_Delete: 
				emit(deletesRequested(selectedIndexes()));
				break;
				
			default:
				event->ignore();
				break;
		}
	 }
	 
	 private:
		QModelIndexList m_firstContiguousBlock;
};



#endif
