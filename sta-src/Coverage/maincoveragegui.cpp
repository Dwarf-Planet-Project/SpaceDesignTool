#include "maincoveragegui.h"
#include <QtGui>
#include <QAction>
#include <QStackedWidget>
#include "QFileDialog"



MainCoverageGUI::MainCoverageGUI( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}



//This is a constructor for the main GUI of Coverage module with a validator in order to let the user to input only positive numbers

MainCoverageGUI::MainCoverageGUI(ScenarioTree* parent): QDialog(parent){

    setupUi(this);

    QDoubleValidator* positiveDoubleValidator= new QDoubleValidator (this);
    positiveDoubleValidator->setBottom(0.0);

    ElLineEdit->setValidator(positiveDoubleValidator);
    EfficiencyLineEdit->setValidator(positiveDoubleValidator);
    FrequencyLineEdit->setValidator(positiveDoubleValidator);
    TiltLineEdit->setValidator(positiveDoubleValidator);

    //For Transmitter

    PowerLineEdit->setValidator(positiveDoubleValidator);
    TxFeederLossLineEdit->setValidator(positiveDoubleValidator);
    TxDepointingLossLineEdit->setValidator(positiveDoubleValidator);
    DataRateLineEdit->setValidator(positiveDoubleValidator);

    //For Receiver

    SystemTempLineEdit->setValidator(positiveDoubleValidator);
    RxFeederLineEdit->setValidator(positiveDoubleValidator);
    RxDepointingLineEdit->setValidator(positiveDoubleValidator);


}
//This is the function that load the values of the GUI into the XML schema. Attention (in staschema.h) that ScenarioCommunicationPayloadType returns
//a QList of QSharedPointers to ScenarioAntennaType elements

bool MainCoverageGUI::loadValues(ScenarioCommunicationPayloadType* commPayload)
{
    
    QList<QSharedPointer<ScenarioAntennaType> > & AntennaList=commPayload->Antenna();
    qDebug()<<AntennaList.length()<<"   length2";

    foreach(QSharedPointer<ScenarioAntennaType> antenna,AntennaList)
    {
        qDebug()<<"test1";
        ElLineEdit->setText(QString::number(antenna->PointingDirection()->elevation()));
        AzLineEdit->setText(QString::number(antenna->PointingDirection()->azimuth()));
        EfficiencyLineEdit->setText(QString::number(antenna->EMproperties()->Efficiency()));
        //FrequencyLineEdit->setText(QString::number(antenna->EMproperties()->Frequency()));
        //Tilt angle is not necessary by now

        //For Transmitter
        if(dynamic_cast<ScenarioTransmitter*>(antenna.data()) != NULL)
        {
        ScenarioTransmitter* transmitter = dynamic_cast<ScenarioTransmitter*>(antenna.data());
        PowerLineEdit->setText(QString::number(transmitter->Power()));
        TxFeederLossLineEdit->setText(QString::number(transmitter->FedderLossTx()));
        TxDepointingLossLineEdit->setText(QString::number(transmitter->DepointingLossTx()));
        DataRateLineEdit->setText(QString::number(transmitter->Modulation()->DataRate()));
        }

        //For Receiver
        if(dynamic_cast<ScenarioReceiver*>(antenna.data()) != NULL)
        {
        ScenarioReceiver* receiver = dynamic_cast<ScenarioReceiver*>(antenna.data());
        //SystemTempLineEdit->setText(QString::number(receiver->SystemTemperature()->TotalSystemTemp()));
        RxFeederLineEdit->setText(QString::number(receiver->FeederLossRx()));
        RxDepointingLineEdit->setText(QString::number(receiver->DepointingLossRx()));
        }

    }

    return true;

}

//This is the function that save the values from the XML schema into the GUI

bool MainCoverageGUI::saveValues(ScenarioCommunicationPayloadType* commPayload)
{
    QList<QSharedPointer<ScenarioAntennaType> > & AntennaList=commPayload->Antenna();


    foreach(QSharedPointer<ScenarioAntennaType> antenna,AntennaList)
    {

        antenna->PointingDirection()->setElevation(ElLineEdit->text().toDouble());
        antenna->PointingDirection()->setAzimuth(AzLineEdit->text().toDouble());
        antenna->EMproperties()->setEfficiency(EfficiencyLineEdit->text().toDouble());
        //antenna->EMproperties()->setFrequency(FrequencyLineEdit->text().toDouble());
        //Tilt angle is not necessary by now

        if(dynamic_cast<ScenarioTransmitter*>(antenna.data()) != NULL && Transmitter->isChecked())
        {

        //For Transmitter
        ScenarioTransmitter* transmitter = dynamic_cast<ScenarioTransmitter*>(antenna.data());
        transmitter->setPower(PowerLineEdit->text().toDouble());
        transmitter->setFedderLossTx(TxFeederLossLineEdit->text().toDouble());
        transmitter->setDepointingLossTx(TxDepointingLossLineEdit->text().toDouble());
        transmitter->Modulation()->setDataRate(DataRateLineEdit->text().toDouble());
        }

        if(dynamic_cast<ScenarioReceiver*>(antenna.data()) != NULL)
        {
        //For Receiver
        ScenarioReceiver* receiver = dynamic_cast<ScenarioReceiver*>(antenna.data());

        //receiver->SystemTemperature()->setTotalSystemTemp(SystemTempLineEdit->text().toDouble());
        receiver->setFeederLossRx(RxFeederLineEdit->text().toDouble());
        receiver->setDepointingLossRx(RxDepointingLineEdit->text().toDouble());
        }
    }


    return true;

}



MainCoverageGUI::~MainCoverageGUI()
{
}

void MainCoverageGUI::on_tabWidget_2_currentChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_PointingGroupBox_2_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_ElLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_AzLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_ConeShapeComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

/*void enableConeShape(int i){

    ConeShapeDialog dialog();
    
    if(i==0){
        dialog.stackedWidget->setCurrentWidget(page);
    } else if (i==1){

    } else if (i==2){
        stackedWidget->setCurrentWidget(page_2);
    }
    
}*/


void MainCoverageGUI::on_ConeShapeToolButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
        int i=0;
        qDebug()<<"RICARDO COMPRUEBA EL TEXTO DE LA COMBO BOX:   "<<ConeShapeComboBox->currentText();
        if(ConeShapeComboBox->currentText()=="Circular"){
            i=0;
             qDebug()<<"TOMA YA CIRCULAR";
         } else if(ConeShapeComboBox->currentText()=="Rectangular"){
             qDebug()<<"TOMA YA RECTANGULAR ";
            i=1;
         }
         ConeShapeDialog dialog(this);

         dialog.exec();
        // dialog.stackedWidget->setCurrentIndex(i);

       // QStackedWidget stackedWidgetTry=dialog.stackedWidget;
        //stackedWidgetTry.setCurrentWidget(page_2);
        //dialog.stackedWidget->setCurrentWidget(page_2);

        //dialog.stackedWidget->currentIndex();


       /* if(ConeShapeComboBox->currentIndex()))
        {
            //dialog.stackedWidget->setCurrentWidget(page);
            qDebug()<<"RICARDO SAYS THAT IT IS CIRCULAR!!!!!!!!!!";
        }
        else if(ConeShapeComboBox->currentText()=="Rectangular")
        {
             //dialog.stackedWidget->setCurrentWidget(page_2);
             qDebug()<<"RICARDO SAYS THAT IT IS RECTANGULAR!!!!!!!!!!";
        }*/


        //dialog.exec();
}

void MainCoverageGUI::on_PolarisationGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_PolarisationComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_TiltLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_AntennaPropGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_EfficiencyLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_FrequencyLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_TypeBeamComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_SizeOptionsButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
        SizeOptionsDialog dialog(this);
        dialog.exec();

}

void MainCoverageGUI::on_Transmitter_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
        if(Transmitter->isChecked()){
            Receiver->setChecked(0);

        }



}

void MainCoverageGUI::on_PowerLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_TxFeederLossLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_TxDepointingLossLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_ModulationGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_TypeModComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_DataRateLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_Receiver_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
        if(Receiver->isChecked()){
            Transmitter->setChecked(0);

           // ScenarioReceiver* receiver=new ScenarioReceiver;
        }
}


void MainCoverageGUI::on_SystemTempToolButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
        SystemTempDialog dialog(this);
        dialog.exec();

}

void MainCoverageGUI::on_SystemTempLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_RxFeederLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_RxDepointingLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void MainCoverageGUI::on_OkButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);

       // MainCoverageGUI::close();

}

void MainCoverageGUI::on_CancelButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

