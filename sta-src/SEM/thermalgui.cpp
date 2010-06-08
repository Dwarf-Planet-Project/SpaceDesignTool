#include "thermalgui.h"

ThermalGUI::ThermalGUI( SemMain * SC, QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
        SCVirtual = SC;

        // Display the temperature limits of Payloads to compare
        ThermalGUIPayload1NameLineEdit->setText
                ((SCVirtual->getNewSCThermal()->getPayloadTemperature()+0)->Name);
        ThermalGUIPayload1MinTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+0)->MinimumTemperature)));
        ThermalGUIPayload1MaxTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+0)->MaximumTemperature)));

        ThermalGUIPayload2NameLineEdit->setText
                ((SCVirtual->getNewSCThermal()->getPayloadTemperature()+1)->Name);
        ThermalGUIPayload2MinTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+1)->MinimumTemperature)));
        ThermalGUIPayload2MaxTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+1)->MaximumTemperature)));

        ThermalGUIPayload3NameLineEdit->setText
                ((SCVirtual->getNewSCThermal()->getPayloadTemperature()+2)->Name);
        ThermalGUIPayload3MinTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+2)->MinimumTemperature)));
        ThermalGUIPayload3MaxTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+2)->MaximumTemperature)));

        ThermalGUIPayload4NameLineEdit->setText
                ((SCVirtual->getNewSCThermal()->getPayloadTemperature()+3)->Name);
        ThermalGUIPayload4MinTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+3)->MinimumTemperature)));
        ThermalGUIPayload4MaxTempLineEdit->setText
                (QString::number
                 (((SCVirtual->getNewSCThermal()->getPayloadTemperature()+3)->MaximumTemperature)));

        ThermalGUISCMinTemperatureLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCThermal()->getMinimumSCTemperature()));
        ThermalGUISCMaxTemperatureLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCThermal()->getMaximumSCTemperature()));

        //setting comboBox from a QString
        int i;
        for (i=0;i<ThermalGUIColdCoatingComboBox->count();i++)
            if (ThermalGUIColdCoatingComboBox->itemText(i)
                == SC->getNewSCThermal()->getColdFaceCoatingProperties().Type)
                ThermalGUIColdCoatingComboBox->setCurrentIndex(i);
        //setting comboBox from a QString
        for (i=0;i<ThermalGUIHotCoatingComboBox->count();i++)
            if (ThermalGUIHotCoatingComboBox->itemText(i)
                == SC->getNewSCThermal()->getHotFaceCoatingProperties().Type)
                ThermalGUIHotCoatingComboBox->setCurrentIndex(i);

        //setttings of absorptivity and emmissivity
        ThermalGUIColdEmmissivityLineEdit->setText
                (QString::number(SCVirtual->getNewSCThermal()
                                 ->getColdFaceCoatingProperties().Emmissivity));
        ThermalGUIColdAbsorptivityLineEdit->setText
                (QString::number(SCVirtual->getNewSCThermal()
                                 ->getColdFaceCoatingProperties().Absorptivity));

        ThermalGUIHotEmmissivityLineEdit->setText
                (QString::number(SCVirtual->getNewSCThermal()
                                 ->getHotFaceCoatingProperties().Emmissivity));
        ThermalGUIHotAbsorptivityLineEdit->setText
                (QString::number(SCVirtual->getNewSCThermal()
                                 ->getHotFaceCoatingProperties().Absorptivity));
}

ThermalGUI::~ThermalGUI()
{
}

void ThermalGUI::on_ThermalGUIColdCoatingComboBox_activated(const QString&)
{
    ThermalGUIColdEmmissivityLineEdit->setEnabled(0);
    ThermalGUIColdAbsorptivityLineEdit->setEnabled(0);

    if (ThermalGUIColdCoatingComboBox->currentText()== "No_Coating")
    {
        SCVirtual->getNewSCThermal()->setColdFaceCoatingProperties
                ("No_Coating",
                 SCVirtual->getNewSCThermal()->getSCStructuralProperties()->Emmissivity,
                 SCVirtual->getNewSCThermal()->getSCStructuralProperties()->Absorptivity);
        qDebug()<< "-----------------------";
        qDebug()<< SCVirtual->getNewSCThermal()->getColdFaceCoatingProperties().Type;
    }
    else
    {
        if (ThermalGUIColdCoatingComboBox->currentText()== "User_Defined")
        {
            ThermalGUIColdEmmissivityLineEdit->setEnabled(1);
            ThermalGUIColdAbsorptivityLineEdit->setEnabled(1);
            ThermalGUIColdEmmissivityLineEdit->setText("0.0");
            ThermalGUIColdAbsorptivityLineEdit->setText("0.0");
        }else
        {
            SCVirtual->getNewSCThermal()->setColdFaceCoatingProperties
                    (ThermalGUIColdCoatingComboBox->currentText());
        }
    }

    qDebug() << "+++++++";
    qDebug() <<"final" <<SCVirtual->getNewSCThermal()->getColdFaceCoatingProperties().Type;

    ThermalGUIColdEmmissivityLineEdit->setText
            (QString::number(SCVirtual->getNewSCThermal()
                             ->getColdFaceCoatingProperties().Emmissivity));
    ThermalGUIColdAbsorptivityLineEdit->setText
            (QString::number(SCVirtual->getNewSCThermal()
                             ->getColdFaceCoatingProperties().Absorptivity));

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ThermalGUI::on_ThermalGUIColdEmmissivityLineEdit_textEdited(const QString&)
{
// insert the cold face properties
    double temp = SCVirtual->getNewSCThermal()->getColdFaceCoatingProperties().Absorptivity;
    SCVirtual->getNewSCThermal()->setColdFaceCoatingProperties
            ("UserDefined",
             ThermalGUIColdEmmissivityLineEdit->text().toDouble(),
             temp);

    qWarning("TODO: %s	%d",__FILE__,__LINE__);

}

void ThermalGUI::on_ThermalGUIColdAbsorptivityLineEdit_textEdited(const QString&)
{
// insert the cold face properties
    double temp = SCVirtual->getNewSCThermal()->getColdFaceCoatingProperties().Emmissivity;
    SCVirtual->getNewSCThermal()->setColdFaceCoatingProperties
            ("UserDefined",
             temp,
             ThermalGUIColdAbsorptivityLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);

}

void ThermalGUI::on_ThermalGUIHotCoatingComboBox_activated(const QString&)
{
    ThermalGUIHotEmmissivityLineEdit->setEnabled(0);
    ThermalGUIHotAbsorptivityLineEdit->setEnabled(0);

    if (ThermalGUIHotCoatingComboBox->currentText()== "No_Coating")
    {
        SCVirtual->getNewSCThermal()->setHotFaceCoatingProperties
                ("No_Coating",
                 SCVirtual->getNewSCThermal()->getSCStructuralProperties()->Emmissivity,
                 SCVirtual->getNewSCThermal()->getSCStructuralProperties()->Absorptivity);
        qDebug()<< "-----------------------";
        qDebug()<< SCVirtual->getNewSCThermal()->getHotFaceCoatingProperties().Type;
    }
    else
    {
        if (ThermalGUIHotCoatingComboBox->currentText()== "User_Defined")
        {
            ThermalGUIHotEmmissivityLineEdit->setEnabled(1);
            ThermalGUIHotAbsorptivityLineEdit->setEnabled(1);            
            ThermalGUIHotEmmissivityLineEdit->setText("0.0");
            ThermalGUIHotAbsorptivityLineEdit->setText("0.0");
        }else
        {
            SCVirtual->getNewSCThermal()->setHotFaceCoatingProperties
                    (ThermalGUIHotCoatingComboBox->currentText());
        }
    }

    qDebug()<<"ThermalGUIHotCoatingComboBox->currentText()"<<ThermalGUIHotCoatingComboBox->currentText();

    ThermalGUIHotEmmissivityLineEdit->setText
            (QString::number(SCVirtual->getNewSCThermal()
                             ->getHotFaceCoatingProperties().Emmissivity));
    ThermalGUIHotAbsorptivityLineEdit->setText
            (QString::number(SCVirtual->getNewSCThermal()
                             ->getHotFaceCoatingProperties().Absorptivity));


	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ThermalGUI::on_ThermalGUIHotEmmissivityLineEdit_textEdited(const QString&)
{
    // insert the cold face properties
    double temp = SCVirtual->getNewSCThermal()->getHotFaceCoatingProperties().Absorptivity;
    SCVirtual->getNewSCThermal()->setHotFaceCoatingProperties
            ("UserDefined",
             ThermalGUIHotEmmissivityLineEdit->text().toDouble(),
             temp);
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ThermalGUI::on_ThermalGUIHotAbsorptivityLineEdit_textEdited(const QString&)
{
    // insert the cold face properties
    double temp = SCVirtual->getNewSCThermal()->getHotFaceCoatingProperties().Emmissivity;
    SCVirtual->getNewSCThermal()->setHotFaceCoatingProperties
            ("UserDefined",
             temp,
             ThermalGUIHotAbsorptivityLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ThermalGUI::on_ThermalGUIHeaterPowerLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ThermalGUI::on_ThermalGUIRadiatorLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ThermalGUI::on_ThermalGUICalculatePushButton_clicked()
{
    // prerequists of Heater and radiator calculations
    // calculate and set the SC temperatures
    SCVirtual->getNewSCThermal()->CalculateAndSetSCTemperatureRange();
//    SCVirtual->getNewSCThermal()->CalculateAndSetMaximumSCTemperature();
    qDebug()<<"GUI max temp"<<SCVirtual->getNewSCThermal()
            ->getMaximumSCTempWithRadiatorOrHeater();

    //calclate and display heater Power
    //the SC temperature has to be fix
    SCVirtual->getNewSCThermal()->CalculateAndSetNeededHeater();

    ThermalGUIHeaterPowerLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCThermal()->getNeededHeater()));

    //calculate and display radiator
    SCVirtual->getNewSCThermal()->CalculateAndSetNeededRadiator();

    ThermalGUIRadiatorLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCThermal()->getNeededRadiator()));

    //Display the final SC temperature
    ThermalGUISCMinTemperatureLineEdit->setText
            (QString::number(SCVirtual->getNewSCThermal()
                             ->getMinimumSCTempWithRadiatorOrHeater()));
    ThermalGUISCMaxTemperatureLineEdit->setText
            (QString::number(SCVirtual->getNewSCThermal()
                             ->getMaximumSCTempWithRadiatorOrHeater()));

    //set Alerts of Payload
    if (SCVirtual->getNewSCThermal()->IsPayloadInAlert(0))
        ThermalGUIPayload1WarningLabel->setEnabled(1);
    else
        ThermalGUIPayload1WarningLabel->setEnabled(0);

    if (SCVirtual->getNewSCThermal()->IsPayloadInAlert(1))
        ThermalGUIPayload2WarningLabel->setEnabled(1);
    else
        ThermalGUIPayload2WarningLabel->setEnabled(0);

    if (SCVirtual->getNewSCThermal()->IsPayloadInAlert(2))
        ThermalGUIPayload3WarningLabel->setEnabled(1);
    else
        ThermalGUIPayload3WarningLabel->setEnabled(0);

    if (SCVirtual->getNewSCThermal()->IsPayloadInAlert(3))
        ThermalGUIPayload4WarningLabel->setEnabled(1);
    else
        ThermalGUIPayload4WarningLabel->setEnabled(0);

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}


void ThermalGUI::on_ThermalGUISavePushButton_clicked()
{
    SCVirtual->PassThermalSubsystemOutputParameters();
    this->close();

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

