#include "structuregui.h"

StructureGUI::StructureGUI( SemMain * SC, QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
        setupUi(this);

        SCVirtual = SC;

        qWarning("TODO: %s	%d",__FILE__,__LINE__);

        // Calculate the volume and mass and display them in the GUI
        // effect of Battery has to be added after the powersubsystem implementation

        StructureGUISCVolumeLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getSCVolume()));

        StructureGUISCMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getSCMass()));

        // if the structure tool is used before put the existing values to GUI
        // Make the calculations and set the values as it is new window

        // Reset the ComboBoxes of Shape and Material
//        StructureGUIMaterialComboBox->currentIndex()

//        StructureGUIMaterialComboBox->set
//                (SCVirtual->getNewSCStructure()->getSCMaterial().Name);
        StructureGUISCShapeComboBox->setCurrentIndex
                (SCVirtual->getNewSCStructure()->getSCShape());

        //Calculate and set the SC dimensions
        StructureGUISCWidthLineEdit->setText(QString::number
             (SCVirtual->getNewSCStructure()->getSpacecraftDimension().x()));
        StructureGUISCHeightLineEdit->setText(QString::number
             (SCVirtual->getNewSCStructure()->getSpacecraftDimension().y()));
        StructureGUISCLengthLineEdit->setText(QString::number
             (SCVirtual->getNewSCStructure()->getSpacecraftDimension().z()));

        //visualize SecondMoment of inertia and display on GUI
        StructureGUIMomentOfAreaXaxisLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCStructure()->getSecondMomentsOfArea().x()));
        StructureGUIMomentOfAreaYaxisLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCStructure()->getSecondMomentsOfArea().y()));
        StructureGUIMomentOfAreaZaxisLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCStructure()->getSecondMomentsOfArea().z()));

        // visualize lateral & axial frequencies of SC (in terms of
        // cylinder it is assumed that it is on top of circles)

        StructureGUISCLateralFrequencyLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getLateralFrequency()));
        StructureGUISCAxialFrequencyLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getAxialFrequency()));

        // visiualize on GUI the moment of inertia
        StructureGUIMomentOfInertiaXaxisLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getMomentsOfInertia().x()));
        StructureGUIMomentOfInertiaYaxisLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getMomentsOfInertia().y()));
        StructureGUIMomentOfInertiaZaxisLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getMomentsOfInertia().z()));

        //set the mass of all the subsystems if the subsystem properties are not selected
        //it will put the estimations
        TotalPayloadMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getTotalPayloadMass()));
        StructureSubsystemMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getStructureSubsystemMass()));
        ThermalSubsystemMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getThermalSubsystemMass()));
        PowerSubsystemMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getPowerSubsystemMass()));
        TTCSubsystemMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getTTCSubsystemMass()));
        OBDHSubsystemMassLineEdit->setText
                (QString::number(SCVirtual->getNewSCStructure()->getOBDHSubsystemMass()));
//        ADCSMassLineEdit->setText
//                (QString::number(SCVirtual->getNewSCStructure()->g));

        //setting comboBox from a QString
        int i;
        for (i=0;i<StructureGUIMaterialComboBox->count();i++)
            if (StructureGUIMaterialComboBox->itemText(i)
                == SC->getNewSCStructure()->getSCMaterial().Name)
                StructureGUIMaterialComboBox->setCurrentIndex(i);
}

StructureGUI::~StructureGUI()
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void StructureGUI::on_StructureGUISCShapeComboBox_activated(const QString&)
{

    // set the shape of the SC
    switch (StructureGUISCShapeComboBox->currentIndex())
    {
    case 0:
        SCVirtual->getNewSCStructure()->setSCShape(Undefined);
        break;
    case 1:
        SCVirtual->getNewSCStructure()->setSCShape(Cube);
        break;
    case 2:
        SCVirtual->getNewSCStructure()->setSCShape(Cylindrical);
        break;
    case 3:
        SCVirtual->getNewSCStructure()->setSCShape(Spherical);
        break;
    }

    qWarning("TODO: %s	%d",__FILE__,__LINE__);

    // Calculate the dimensions of SC and fill them in the
    // GUI
    SCVirtual->getNewSCStructure()->CalculateAndSetSpacecraftDimension();

    StructureGUISCWidthLineEdit->setText(QString::number
         (SCVirtual->getNewSCStructure()->getSpacecraftDimension().x()));
    StructureGUISCHeightLineEdit->setText(QString::number
         (SCVirtual->getNewSCStructure()->getSpacecraftDimension().y()));
    StructureGUISCLengthLineEdit->setText(QString::number
         (SCVirtual->getNewSCStructure()->getSpacecraftDimension().z()));

    //Calculate and set SecondMoment of ýnertia and display on GUI
    SCVirtual->getNewSCStructure()->CalculateAndSetSecondMomentsOfArea();

    StructureGUIMomentOfAreaXaxisLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCStructure()->getSecondMomentsOfArea().x()));
    StructureGUIMomentOfAreaYaxisLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCStructure()->getSecondMomentsOfArea().y()));
    StructureGUIMomentOfAreaZaxisLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCStructure()->getSecondMomentsOfArea().z()));

    // calculate and set lateral & axial frequencies of SC (in terms of
    // cylinder it is assumed that it is on top of circles)
    SCVirtual->getNewSCStructure()->CalculateAndSetLateralFrequency();
    SCVirtual->getNewSCStructure()->CalculateAndSetAxialFrequency();

    StructureGUISCLateralFrequencyLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getLateralFrequency()));
    StructureGUISCAxialFrequencyLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getAxialFrequency()));

    // calculate and visiualize on GUI the moment of inertia
    SCVirtual->getNewSCStructure()->CalculateAndSetMomentsOfInertia();

    StructureGUIMomentOfInertiaXaxisLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getMomentsOfInertia().x()));
    StructureGUIMomentOfInertiaYaxisLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getMomentsOfInertia().y()));
    StructureGUIMomentOfInertiaZaxisLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getMomentsOfInertia().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void StructureGUI::on_StructureGUIMaterialComboBox_activated(const QString &)
{
    // set the shape of the SC
    SCVirtual->getNewSCStructure()->setMaterialProperties
            (StructureGUIMaterialComboBox->currentText());

    // calculate and set lateral & axial frequencies of SC (in terms of
    // cylinder it is assumed that it is on top of circles)
    SCVirtual->getNewSCStructure()->CalculateAndSetLateralFrequency();
    SCVirtual->getNewSCStructure()->CalculateAndSetAxialFrequency();

    StructureGUISCLateralFrequencyLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getLateralFrequency()));
    qDebug()<<SCVirtual->getNewSCStructure()->getLateralFrequency();

    StructureGUISCAxialFrequencyLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getAxialFrequency()));
    qDebug()<<SCVirtual->getNewSCStructure()->getAxialFrequency();

    //refresh the mass thestructure subsystems with the defined material
    StructureSubsystemMassLineEdit->setText
            (QString::number(SCVirtual->getNewSCStructure()->getStructureSubsystemMass()));
}

void StructureGUI::on_StructureGUISavePushButton_clicked()
{
    SCVirtual->PassStructureSubsystemOutputParameters();
    this->close();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

