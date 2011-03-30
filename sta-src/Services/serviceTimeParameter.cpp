#include "serviceTimeParameter.h"

DialogServiceTimeParameterFrame::DialogServiceTimeParameterFrame( QWidget * parent, Qt::WindowFlags f) : QFrame(parent,f)
{
	setupUi(this);
}

DialogServiceTimeParameterFrame::~DialogServiceTimeParameterFrame()
{
}

void DialogServiceTimeParameterFrame::on_comboBoxTimeParameterChoice_currentIndexChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

