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
 Add drop capability to manoeuvres tables.
 */

#include "rendezVousScheduledManoeuvresTable.h"


ScheduledTableManoeuvres::ScheduledTableManoeuvres(QWidget *parent){
    this->setAcceptDrops(true);

    this->setParent(parent);



}

ScheduledTableManoeuvres::~ScheduledTableManoeuvres(){

}


void ScheduledTableManoeuvres::updateTable(QList<RVmanoeuvreDescriptor> list){
    int j;
    QString temp,temp2,temp3,temp4;

    //Every new column must have memory allocation for its new cells
    for (j=0;j<list.size();j++){
            QTableWidgetItem *title=new QTableWidgetItem();

            if(j>=this->columnCount())
            {this->insertColumn(j);
               temp.setNum(j+1);
               title->setText(temp);
               this->setHorizontalHeaderItem(j,title);
               temp.clear();
            }
            QTableWidgetItem *name=new QTableWidgetItem();
            this->setItem(0,j,name);

            //NAMES AND DESCRIPTORS

            if(list.at(j).descriptor==FREE_DRIFT)
                this->item(0,j)->setText("Free Drift");

            if(list.at(j).descriptor==DELTA_V)
                this->item(0,j)->setText("Full DeltaV");

            if(list.at(j).descriptor==TBT_ALONG_X_AXIS)
                this->item(0,j)->setText("2TBTalong x-axis");

            if(list.at(j).descriptor==TBT_HOHMANN_TRANSFER)
                this->item(0,j)->setText("2TBTHohmann Transfer");

            if(list.at(j).descriptor==TBT_90_DEGREES_FLY_AROUND)
                this->item(0,j)->setText("2TBT90 degrees fly-around");

            if(list.at(j).descriptor==RBT_ALONG_X_AXIS)
                this->item(0,j)->setText("2RBTalong x-axis");

            if(list.at(j).descriptor==RBT_90_DEGREES_FLY_AROUND)
                this->item(0,j)->setText("2RBT90 degrees radial fly-around");

            if(list.at(j).descriptor==RBT_360_DEGREES_FLY_AROUND)
                this->item(0,j)->setText("2RBT360 degrees radial fly-around");

            if(list.at(j).descriptor==HOLD_POINTS_VBAR)
                this->item(0,j)->setText("Hold Points outside V-bar");

            if(list.at(j).descriptor==CFT_TANGENTIAL)
                this->item(0,j)->setText("CFT Tangential");
            if(list.at(j).descriptor==CFT_RADIAL)
                this->item(0,j)->setText("CFT Radial");
            if(list.at(j).descriptor==SLT_VBAR)
                this->item(0,j)->setText("SLT V-bar");
            if(list.at(j).descriptor==SLT_RBAR)
                this->item(0,j)->setText("SLT R-bar");


            //InitTime cell
            sta::StateVector* auxStateVector=new sta::StateVector();
            *auxStateVector=list.at(j).initPosVel;

            QTableWidgetItem *initTime=new QTableWidgetItem();
            this->setItem(1,j,initTime);
            temp4.append(list.at(j).initTime.toString("dd.MM.yyyy hh:mm:ss.zzz"));
            this->item(1,j)->setText(temp4);
            temp4.clear();
            //
            //EndTime cell
            QTableWidgetItem *endTime=new QTableWidgetItem();
            this->setItem(2,j,endTime);
            temp4.append(list.at(j).endTime.toString("dd.MM.yyyy hh:mm:ss.zzz"));
            this->item(2,j)->setText(temp4);
            temp4.clear();
            //

            //InitPos cell
            QTableWidgetItem *initPos=new QTableWidgetItem();
            this->setItem(3,j,initPos);
            temp4.append("X.- ");

            temp.setNum(auxStateVector->position(0));
            temp2.setNum(auxStateVector->position(1));
            temp3.setNum(auxStateVector->position(2));

            temp.append("\n");
            temp2.append("\n");
            temp4.append(temp);
            temp4.append("Y.- ");
            temp4.append(temp2);
            temp4.append("Z.- ");
            temp4.append(temp3);
            this->item(3,j)->setText(temp4);
            temp4.clear();
            QSize size(30,50);
            this->item(3,j)->setSizeHint(size);

            //
            //EndPos cell

            *auxStateVector=list.at(j).endPosVel;

            QTableWidgetItem *endPos=new QTableWidgetItem();
            this->setItem(4,j,endPos);
            temp4.append("X.- ");

            temp.setNum(auxStateVector->position(0));
            temp2.setNum(auxStateVector->position(1));
            temp3.setNum(auxStateVector->position(2));

            temp.append("\n");
            temp2.append("\n");
            temp4.append(temp);
            temp4.append("Y.- ");
            temp4.append(temp2);
            temp4.append("Z.- ");
            temp4.append(temp3);
            this->item(4,j)->setText(temp4);
            temp4.clear();
            this->item(4,j)->setSizeHint(size);

            //

            //DeltaV's

            QTableWidgetItem *itemDeltaV1=new QTableWidgetItem();
            this->setItem(5,j,itemDeltaV1);

            temp.setNum(list[j].getDeltaV1());
            temp.append(" km/s");

            if(this->item(0,j)->text().compare("Free Drift")==0 ||
               this->item(0,j)->text().compare("Hold Points outside V-bar")==0 ||
               this->item(0,j)->text().compare("CFT Tangential")==0 ||
               this->item(0,j)->text().compare("CFT Radial")==0)
                this->item(5,j)->setText("NOT USED");
            else
                this->item(5,j)->setText(temp);

            temp.clear();
            QTableWidgetItem *itemDeltaV2=new QTableWidgetItem();
            this->setItem(6,j,itemDeltaV2);

            temp.setNum(list[j].getDeltaV2());
            temp.append("  km/s");
            if(this->item(0,j)->text().compare("Free Drift")==0 ||
               this->item(0,j)->text().compare("Hold Points outside V-bar")==0 ||
               this->item(0,j)->text().compare("CFT Tangential")==0 ||
               this->item(0,j)->text().compare("CFT Radial")==0)
                this->item(6,j)->setText("NOT USED");
            else
                this->item(6,j)->setText(temp);

            temp.clear();


            QTableWidgetItem *itemTotalDeltaV=new QTableWidgetItem();
            this->setItem(7,j,itemTotalDeltaV);

            temp.setNum(list[j].getTotalDeltaV());
            temp.append("  km/s");

            this->item(7,j)->setText(temp);
            temp.clear();

            //input cell
            QTableWidgetItem *input=new QTableWidgetItem();
            this->setItem(8,j,input);
            temp.setNum(list[j].getRVinput());
            this->item(8,j)->setText(temp);


        }
}




void ScheduledTableManoeuvres::updateManoeuvreList(){
    int j;
    for (j=0;j<this->columnCount();j++){
            RVmanoeuvreDescriptor *descriptor=new RVmanoeuvreDescriptor();

            if(this->item(0,j)->text().compare("Free Drift")==0)
                descriptor->setDescriptor(FREE_DRIFT);

            if(this->item(0,j)->text().compare("SBtangential")==0)
                descriptor->setDescriptor(SB_TANGENTIAL);
            if(this->item(0,j)->text().compare("SBradial")==0)
                descriptor->setDescriptor(SB_RADIAL);
            if(this->item(0,j)->text().compare("SBlateral")==0)
                descriptor->setDescriptor(SB_LATERAL);
            if(this->item(0,j)->text().compare("Full DeltaV")==0)
                descriptor->setDescriptor(DELTA_V);

            if(this->item(0,j)->text().compare("2TBTalong x-axis")==0)
            {
                descriptor->setDescriptor(TBT_ALONG_X_AXIS);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }


            if(this->item(0,j)->text().compare("2TBTHohmann Transfer")==0)
            {
                descriptor->setDescriptor(TBT_HOHMANN_TRANSFER);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }


            if(this->item(0,j)->text().compare("2TBT90 degrees fly-around")==0)
            {
                descriptor->setDescriptor(TBT_90_DEGREES_FLY_AROUND);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }

            if(this->item(0,j)->text().compare("2RBTalong x-axis")==0)
            {
                descriptor->setDescriptor(RBT_ALONG_X_AXIS);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());

            }


            if(this->item(0,j)->text().compare("2RBT90 degrees radial fly-around")==0)
            {
                descriptor->setDescriptor(RBT_90_DEGREES_FLY_AROUND);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());

            }

            if(this->item(0,j)->text().compare("2RBT360 degrees radial fly-around")==0)
            {
                descriptor->setDescriptor(RBT_360_DEGREES_FLY_AROUND);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }

            if(this->item(0,j)->text().compare("Hold Points outside V-bar")==0)
            {
                descriptor->setDescriptor(HOLD_POINTS_VBAR);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());

            }

            if(this->item(0,j)->text().compare("SLT V-bar")==0)
            {
                descriptor->setDescriptor(SLT_VBAR);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());

            }

            if(this->item(0,j)->text().compare("SLT R-bar")==0)
            {
                descriptor->setDescriptor(SLT_RBAR);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }

            if(this->item(0,j)->text().compare("CFT Tangential")==0)
            {
                descriptor->setDescriptor(CFT_TANGENTIAL);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }


            if(this->item(0,j)->text().compare("CFT Radial")==0)
            {
                descriptor->setDescriptor(CFT_RADIAL);
                descriptor->setRVinput(this->item(8,j)->data(Qt::DisplayRole).toDouble());
            }

            descriptor->setName(this->item(0,j)->text());


            manoeuvreList.append(*descriptor);

        }

}





void ScheduledTableManoeuvres::mousePressEvent(QMouseEvent *event)
{
    int column;
    // Get current selection
    QTableWidgetItem *selectedItem = currentItem();

    if ((event->buttons() & Qt::RightButton)){
        this->removeColumn(this->currentColumn());

        return;

    }



    // If the selected Item exists
    if (selectedItem)
    {

        QByteArray itemData;
        column=selectedItem->column();
        itemData.append(this->item(0,column)->text());

        //Erasing text
        this->item(0,column)->setText(" ");

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-qabstractitemmodeldatalist",itemData);

        // Create drag
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        drag->exec(Qt::CopyAction);
   }

    QTableWidget::mousePressEvent(event);
}




void ScheduledTableManoeuvres::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source()==this){

        event->setDropAction( Qt::CopyAction );
        event->accept();


    }

    event->acceptProposedAction();



}




void ScheduledTableManoeuvres::dropEvent(QDropEvent *event){
    QString temp;
    int i,column;

    QByteArray itemData;

    if(event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")){
        if(event->source()==this){

            //column=receiver->column();

            itemData.append(event->mimeData()->data("application/x-qabstractitemmodeldatalist").data());

            this->item(0,this->currentItem()->column())->setText(itemData);
        }
        else
           {
            //Drop comes from TreeDrag


             itemData.append(event->mimeData()->data("application/x-qabstractitemmodeldatalist").data());

             this->item(0,this->columnCount()-1)->setText(itemData);
             this->insertColumn(this->columnCount());

             for (i=0;i<=this->rowCount();i++){
                 QTableWidgetItem *newItem=new QTableWidgetItem();
                 newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|
                                   Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
                 this->setItem(i,this->columnCount()-1,newItem);
             }

           }

    }else
        this->item(0,0)->setText("EY!");



}


