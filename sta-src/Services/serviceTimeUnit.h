#ifndef DIALOGSERVICETIMEUNITFRAME_H
#define DIALOGSERVICETIMEUNITFRAME_H

#include "ui_serviceTimeUnit.h"
#include <QFrame>

class DialogServiceTimeUnitFrame : public QFrame , private Ui::DialogServiceTimeUnitFrame
{
Q_OBJECT
public:
	DialogServiceTimeUnitFrame( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~DialogServiceTimeUnitFrame();
protected slots:
	void on_comboBoxTimeUnitsChoice_currentIndexChanged(int);
	
};
#endif //DIALOGSERVICETIMEUNITFRAME_H
