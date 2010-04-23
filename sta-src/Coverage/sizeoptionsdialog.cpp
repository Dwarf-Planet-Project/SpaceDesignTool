#include "sizeoptionsdialog.h"

SizeOptionsDialog::SizeOptionsDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

        QDoubleValidator* positiveDoubleValidator= new QDoubleValidator (this);
        positiveDoubleValidator->setBottom(0.0);
        GainLineEdit->setValidator(positiveDoubleValidator);
        DiameterLineEdit->setValidator(positiveDoubleValidator);
        BeamLineEdit->setValidator(positiveDoubleValidator);


}

bool SizeOptionsDialog::loadValues(ScenarioAntennaType* antenna){


    GainLineEdit->setText(QString::number(antenna->EMproperties()->GainMax()));
    DiameterLineEdit->setText(QString::number(antenna->EMproperties()->Diameter()));
    BeamLineEdit->setText(QString::number(antenna->EMproperties()->AngularBeamWidth()));

    return true;
}

bool SizeOptionsDialog::saveValues(ScenarioAntennaType* antenna){

    antenna->EMproperties()->setGainMax(GainLineEdit->text().toDouble());
    antenna->EMproperties()->setDiameter(DiameterLineEdit->text().toDouble());
    antenna->EMproperties()->setAngularBeamWidth(BeamLineEdit->text().toDouble());

    return true;
}


SizeOptionsDialog::~SizeOptionsDialog()
{
}

void SizeOptionsDialog::on_AntennaSizeGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);

}

void SizeOptionsDialog::on_GainMaxRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SizeOptionsDialog::on_DiameterRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SizeOptionsDialog::on_BeamWidthRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SizeOptionsDialog::on_GainLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SizeOptionsDialog::on_DiameterLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SizeOptionsDialog::on_BeamLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SizeOptionsDialog::on_OkButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);

}

void SizeOptionsDialog::on_CancelButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

