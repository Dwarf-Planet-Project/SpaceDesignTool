/*
*/

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Modified by Guillermo on October 2009 to clean up the interface
 Patched extensively by Guillermo on June 2010 to comply with new scenario
 Modified by Guillermo on November 2010 to include ASPECTs
 */

#include "locationeditor.h"
#include "Scenario/scenario.h"

#include "ui_missionAspectDialog.h"


/* Dialog box for editing location parameters: central body,
 * latitude, longitude, and altitude.
 */
LocationEditorDialog::LocationEditorDialog(QWidget* parent) :
        QDialog(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(-360.0);
    angleValidator->setTop(360.0);
    
    latitudeEdit->setValidator(angleValidator);
    longitudeEdit->setValidator(angleValidator);
    altitudeEdit->setValidator(doubleValidator);
}


LocationEditorDialog::~LocationEditorDialog()
{  
}


// Load the edit box values from the current node values
bool LocationEditorDialog::loadValues(const ScenarioLocationType* location)
{
    QSharedPointer<ScenarioAbstract3DOFPositionType> position = location->Abstract3DOFPosition();
    QSharedPointer<ScenarioGroundPositionType> groundPosition = qSharedPointerDynamicCast<ScenarioGroundPositionType>(position);

    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(location->CentralBody());

    latitudeEdit->setText(QString::number(groundPosition->latitude()));
    longitudeEdit->setText(QString::number(groundPosition->longitude()));
    altitudeEdit->setText(QString::number(groundPosition->altitude()));

    //qDebug() << centralBody->name() << endl;

    // The list of planets in teh comboBox are as follows
    // Earth, Moon, Sun, Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto

    if (centralBody->name() == "Earth")
    {
        centralBodyComboBox->setCurrentIndex(0);
    }
    else if (centralBody->name() == "Moon")
    {
        centralBodyComboBox->setCurrentIndex(1);
    }
    else if (centralBody->name() == "Mercury")
    {
        centralBodyComboBox->setCurrentIndex(2);
    }
    else if (centralBody->name() == "Venus")
    {
        centralBodyComboBox->setCurrentIndex(3);
    }
    else if (centralBody->name() == "Mars")
    {
        centralBodyComboBox->setCurrentIndex(4);
    }
    else if (centralBody->name() == "Jupiter")
    {
        centralBodyComboBox->setCurrentIndex(5);
    }
    else if (centralBody->name() == "Saturn")
    {
        centralBodyComboBox->setCurrentIndex(6);
    }
    else if (centralBody->name() == "Uranus")
    {
        centralBodyComboBox->setCurrentIndex(7);
    }
    else if (centralBody->name() == "Neptune")
    {
        centralBodyComboBox->setCurrentIndex(8);
    }
    else if (centralBody->name() == "Pluto")
    {
        centralBodyComboBox->setCurrentIndex(9);
    }
    else
    {
    	centralBodyComboBox->setCurrentIndex(0);
    }

    //ScenarioElementIdentifierType* locationIdentifier    = location->ElementIdentifier().data();

    return true;
}





bool LocationEditorDialog::loadValues(ScenarioElementIdentifierType* arcIdentifier)
{
    QString theArcName = arcIdentifier->Name();
    locationAspect.loadValueArcName(theArcName);

    QString theArcColor = arcIdentifier->colorName();
    locationAspect.loadValueArcColor(theArcColor);

    QString theArcModel = arcIdentifier->modelName();
    locationAspect.loadValueArcModel(theArcModel);

    return true;
}



// Save edit box values to a tree of DOM nodes
bool LocationEditorDialog::saveValues(ScenarioLocationType* location)
{
    QSharedPointer<ScenarioAbstract3DOFPositionType> position = location->Abstract3DOFPosition();
    QSharedPointer<ScenarioGroundPositionType> groundPosition = qSharedPointerDynamicCast<ScenarioGroundPositionType>(position);

    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(location->CentralBody());

    groundPosition->setAltitude(altitudeEdit->text().toDouble());
    groundPosition->setLatitude(latitudeEdit->text().toDouble());
    groundPosition->setLongitude(longitudeEdit->text().toDouble());

    location->setCentralBody(centralBodyComboBox->currentText());

    location->setAbstract3DOFPosition(groundPosition);

    return true;
}






bool LocationEditorDialog::saveValues(ScenarioElementIdentifierType* arcIdentifier)
{
    // The arc name
    QString theArcName = locationAspect.saveValueArcName();
    arcIdentifier->setName(theArcName);

    // The color
    QString theColorName = locationAspect.saveValueArcColor();
    arcIdentifier->setColorName(theColorName);

    // The model
    QString theModelName = locationAspect.saveValueArcModel();
    arcIdentifier->setModelName(theModelName);

    return true;
}




void LocationEditorDialog::on_pushButtonAspect_clicked()
{
    locationAspect.exec();
}

