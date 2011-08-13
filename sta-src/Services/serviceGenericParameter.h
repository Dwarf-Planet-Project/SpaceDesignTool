/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl

*/

/*
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Guillermo Ortega  -------------------------------------------

*/

#ifndef SERVICEGENERICPARAMETER_H
#define SERVICEGENERICPARAMETER_H

#include "ui_serviceGenericParameter.h"
#include <QFrame>

class serviceGenericParameter : public QFrame , private Ui::DialogServiceGenericParameterFrame
{
Q_OBJECT
public:
	serviceGenericParameter( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~serviceGenericParameter();

public:
    QLineEdit *genericParameterLineEditWidget;  // This widget is required since it is the external expression of the QLineEdit called "anyParameterLineEdit"
    void setGenericParameterLineEditWidget(QString myGenericParameter);


public slots:
	void on_anyParameterLineEdit_textChanged(const QString&);
	
};
#endif //SERVICEGENERICPARAMETER_H
