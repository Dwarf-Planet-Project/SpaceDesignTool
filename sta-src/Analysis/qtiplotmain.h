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
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------

 */



#include <QMainWindow>
#include <QVariantList>
#include <QSignalMapper>
#include <QAction>
#include <QTextEdit>
#include <QLabel>

#include "analysisParametersChoice.h"
#include "Scenario/staschema.h"
#include "Main/propagatedscenario.h"



class ApplicationWindow;
class SpaceScenario;

class QtiPlotMain: public QMainWindow
{
    Q_OBJECT
public:
    QtiPlotMain(bool factorySettings, QWidget *parent = 0);
    void passTheSTAscenarioToQtiPlotMain(SpaceScenario* scenario, PropagatedScenario* propagatedScenario);
    void createReport(AnalysisResult* analysis);

    ~QtiPlotMain();

public:
     QSignalMapper *signalMapper;


private slots:
     void createTable();
     void qtiClosed();

private:
    void initQtiPlot();

private:
    SpaceScenario* m_scenario;
    PropagatedScenario* m_propagatedScenario;
    ApplicationWindow* m_appWindow;
    analysisParametersChoice* m_analysisParameterChooser;
};

