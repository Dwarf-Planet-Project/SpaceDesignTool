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
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include <cassert>
#include "scenarioview.h"
#include "scenariotree.h"


ScenarioView::ScenarioView(QWidget* parent) :
    QWidget(parent),
    m_scenarioTree(NULL)
{
    setupUi(this);
    
    // TODO: treeView is just a placeholder in Designer for the
    // real widget, ScenarioTree.
    layout()->removeWidget(treeView);
    delete treeView;
    treeView = NULL;

    m_scenarioTree = new ScenarioTree(this);
    layout()->addWidget(m_scenarioTree);

    // Guillermo on alternate background
    m_scenarioTree->setAlternatingRowColors(true);

}


ScenarioView::~ScenarioView()
{
}
