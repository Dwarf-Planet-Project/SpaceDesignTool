/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl
 */

/*
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----

 */

/*
 ------------------ Author: J. Alonso     -------------------------------------------------
 ------------------ ETSIA                 ----------------------------
 Comments:
 Add drag capability to manoeuvres tree.
 */

#include "rendezVousManoeuvresTreeDrag.h"


ManoeuvresTreeDrag::ManoeuvresTreeDrag(QWidget* parent){

   this->setDragEnabled(true);

}

ManoeuvresTreeDrag::~ManoeuvresTreeDrag(){}



void ManoeuvresTreeDrag::mousePressEvent(QMouseEvent *event)
{
    // Get current selection

    QTreeWidgetItem *selectedItem = currentItem();



    // If the selected Item exists
    if (selectedItem)
    {
        QByteArray itemData;

        itemData.append(selectedItem->text(0));
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-qabstractitemmodeldatalist",itemData);

        // Create drag
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);



        drag->exec(Qt::CopyAction);

    }

    QTreeWidget::mousePressEvent(event);
}




