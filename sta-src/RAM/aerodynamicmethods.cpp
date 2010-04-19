#include "aerodynamicmethods.h"

AerodynamicMethodDialog::AerodynamicMethodDialog(ScenarioREVAeroThermodynamicsType* aerothermo, QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
        if(aerothermo->CoefficientType()!=3)
        {
            attitudeGroupBox->setEnabled(1);
        }
}

AerodynamicMethodDialog::~AerodynamicMethodDialog()
{

}

void AerodynamicMethodDialog::on_buttonBox_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void AerodynamicMethodDialog::on_buttonBox_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

