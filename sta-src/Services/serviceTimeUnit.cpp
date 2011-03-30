#include "serviceTimeUnit.h"

DialogServiceTimeUnitFrame::DialogServiceTimeUnitFrame( QWidget * parent, Qt::WindowFlags f) : QFrame(parent,f)
{
	setupUi(this);
}

DialogServiceTimeUnitFrame::~DialogServiceTimeUnitFrame()
{
}

void DialogServiceTimeUnitFrame::on_comboBoxTimeUnitsChoice_currentIndexChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

