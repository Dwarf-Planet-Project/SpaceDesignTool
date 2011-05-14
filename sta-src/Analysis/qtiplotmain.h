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

#include "Scenario/staschema.h"
#include "Main/propagatedscenario.h"



class ApplicationWindow;
class SpaceScenario;

/** AnalyisResult is a simple container for data produced during an
  * analysis. After creation, the AnalysisResult structure is passed
  * to QtiPlot.
  */
class AnalysisResult
{
public:
    AnalysisResult(unsigned int columnCount);

    void appendRow(const QVariantList& rowValues);
    int columnCount() const;
    int rowCount() const;

    QString title() const
    {
        return m_title;
    }
    void setTitle(const QString& title);

    QString columnName(int column) const;
    void setColumnName(int column, const QString& name);

    QList<QVariant> data() const
    {
        return m_data;
    }

private:
    QString m_title;
    QList<QString> m_columnNames;
    QVariantList m_data;
};


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
    void qtiClosed();

private:
    void initQtiPlot();

private:
    ApplicationWindow* m_appWindow;
    SpaceScenario* m_scenario;
    PropagatedScenario* m_propagatedScenario;


};

