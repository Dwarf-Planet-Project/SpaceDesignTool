/* 
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 */

#include <QItemDelegate>
#include <QLineEdit>

class TreeItemDelegate : public QItemDelegate{
	public:    
		TreeItemDelegate(QObject* parent = 0) : QItemDelegate(parent) {}     
		QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const{
			QWidget* editor = QItemDelegate::createEditor(parent, option, index);    
			if (index.column() == 1)   {
				if (QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor)){ 
					QDoubleValidator* doubleValidator = new QDoubleValidator(editor);
					lineEdit->setValidator(doubleValidator);    
					}
				return editor;  
			}      
			return 0;    
		}
};

