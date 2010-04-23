#include "systemtempdialog.h"

SystemTempDialog::SystemTempDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

bool SystemTempDialog::loadValues(ScenarioReceiver* receiver){

    RxNoiseFigureLineEdit->setText(QString::number(receiver->SystemTemperature()->RxNoiseFigure()));
    //RxFeederTmpLineEdit->setText(QString::number(receiver->FeederLossRx()));
    //RxTempLineEdit
    //AntennaTempLineEdit


    return true;
}


SystemTempDialog::~SystemTempDialog()
{
}

void SystemTempDialog::on_SystemTempGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_RxNoiseFigureLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_RxFeederTmpLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_RxTempLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_ConstantTempRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_AntennaTempLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_CalculatedTempRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_OkButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SystemTempDialog::on_CancelButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

