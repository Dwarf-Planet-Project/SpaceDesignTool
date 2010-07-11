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
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Guillermo Ortega  ----------------------------------------
 July 2010

 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "mainwindow.h"


/*! This class will handle the drop-in events of STA files into the application.
    Drop a file inside the application will be interpreted as "I want to open this file..."
*/
class EventListener : public QObject
{
	Q_OBJECT
	
#ifdef Q_WS_MAC
	public:
		EventListener() { pMainWindow = NULL; };
		inline QString file() { return pFile; };
		inline void setMainWin(MainWindow* mainWin)
		{
			pMainWindow = mainWin;
		};
	
	private:
		QString pFile;
		MainWindow* pMainWindow;
#endif
	
	protected:
		bool eventFilter(QObject*, QEvent* event);
};

#endif
