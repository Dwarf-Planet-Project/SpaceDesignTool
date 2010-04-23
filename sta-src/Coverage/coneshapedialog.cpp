#include "coneshapedialog.h"

ConeShapeDialog::ConeShapeDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

ConeShapeDialog::~ConeShapeDialog()
{
}

void ConeShapeDialog::on_OkButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ConeShapeDialog::on_CancelButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ConeShapeDialog::on_VertAngleLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ConeShapeDialog::on_HorAngleLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void ConeShapeDialog::on_ConeAngleLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

