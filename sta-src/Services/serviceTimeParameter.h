#ifndef DIALOGSERVICETIMEPARAMETERFRAME_H
#define DIALOGSERVICETIMEPARAMETERFRAME_H

#include "ui_serviceTimeParameter.h"
#include <QFrame>

class DialogServiceTimeParameterFrame : public QFrame , private Ui::DialogServiceTimeParameterFrame
{
Q_OBJECT
public:
	DialogServiceTimeParameterFrame( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~DialogServiceTimeParameterFrame();
protected slots:
	void on_comboBoxTimeParameterChoice_currentIndexChanged(int);
	
};
#endif //DIALOGSERVICETIMEPARAMETERFRAME_H
