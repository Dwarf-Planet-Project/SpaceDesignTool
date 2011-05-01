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

// Authors: Class and Steffen


#include "cstudy.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/nedTOfixed.h"
#include <QCheckBox>
#include <QMessageBox>

ConstellationStudySpaceObject::ConstellationStudySpaceObject(SpaceObject* spaceObject):
        m_spaceObject(spaceObject),
        linksamples(QList<LinkSample>()),
        groundlinksamples(QList<GroundLinkSample>()),
        coveragesample(QList<CoverageSample>()),
        observation(QList<Antenna*>()),
        visibilityR(QList<Antenna*>()),
        visibilityT(QList<Antenna*>()),
        m_chosen(true)
{
    // Declaration of comm. antennas
    const QList<PSAntennaObject*> antennaObjectListR = spaceObject->receiver();
    foreach (PSAntennaObject* antennaObject, antennaObjectListR)
    {
        visibilityR.append(new Antenna(
                antennaObject->getAzimuth(),
                antennaObject->getElevation(),
                antennaObject->getConeAngle(),
                antennaObject->getConeShape()));
    }

    const QList<PSAntennaObject*> antennaObjectListT = spaceObject->transmitter();
    foreach (PSAntennaObject* antennaObject, antennaObjectListT)
    {
        if (!antennaObject->getObservationChecked())
        {
            visibilityT.append(new Antenna(
                    antennaObject->getAzimuth(),
                    antennaObject->getElevation(),
                    antennaObject->getConeAngle(),
                    antennaObject->getConeShape()));
        }
        else
        {
            observation.append(new Antenna(
                antennaObject->getAzimuth(),
                antennaObject->getElevation(),
                antennaObject->getConeAngle(),
                antennaObject->getConeShape()));
        }
    }

    if (!spaceObject->mission().isEmpty())
    {
        asocolor = spaceObject->mission().first()->arcTrajectoryColor();
    }
    //asocolor = spaceObject->trajectoryColor();//old version
    asoname = spaceObject->name();
}





ConstellationStudySpaceObject::~ConstellationStudySpaceObject()
{
}



void
ConstellationStudySpaceObject::choose(bool c)
{
    m_chosen = c;
}


ConstellationStudy::ConstellationStudy(PropagatedScenario* scenario, bool m_calcCoverage, bool m_calcSOLink, bool m_calcGOLink)
{
    // Initialization
    m_scenario = scenario;
    m_constellationStudySpaceObjectList = QList<ConstellationStudySpaceObject>();
    m_startTime =  2000000000.;
    m_endTime   = -2000000000.;
    m_timeStep  =   400000000.;

    // save Space Objects and determine time range
    foreach(SpaceObject* spaceObject, scenario->spaceObjects())
    {
        m_endTime = spaceObject->missionEndTime();
        m_startTime = spaceObject->missionStartTime();
        m_sampleTimes = spaceObject->mission().at(0)->sampleTimes(); // why not only for the first space object??
        m_constellationStudySpaceObjectList.append(ConstellationStudySpaceObject(spaceObject));
    }

    // todo: samples have to depend on the scenario (but how??)
    //int timesteps = 600; // faster cstudy propagation with less timesteps
    //m_timeStep = (m_endTime - m_startTime)/timesteps;
    //m_sampleTimes = QList<double>();

    //for (int k = 0; k < timesteps; k++)
    //{
    //    m_sampleTimes.append(m_startTime + k*m_timeStep);
    //}

    if(m_constellationStudySpaceObjectList.length()<=36)
    {
        // analyze for each Space Object
        for (int i = 0; i < m_constellationStudySpaceObjectList.length(); i++)
        {
            // creating discrete mesh
            // TODO define body globally for interplanetary missions
            m_body = m_constellationStudySpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
            if(i==0)
            {
                // TODO: is going to be set by the user
                int numberRows = 80;
                m_discreteMesh = new DiscreteMesh(m_body,numberRows);
            }
            //QMessageBox::warning(NULL,"",QObject::tr("%1").arg(m_sampleTimes.length()));
            foreach(double currentTime, m_sampleTimes)
            {
                CoverageSample coveragesample;
                CoverageSample coveragesampleTmp; //we need this if we have more than 1 observation Antenna
                if (m_calcCoverage)
                {
                    // add for each time sample and space object

                    // coverage
                    coveragesample.curtime = currentTime;
                    coveragesample.curpoints = QList<DiscretizationPoint>();
                    coveragesample.histpoints = QList<DiscretizationPoint>();

                    // for searching time optimization:
                    coveragesample.lowestRow = m_discreteMesh->numberRows;
                    coveragesample.highestRow = -1;
                    coveragesample.firstPoints = new int[m_discreteMesh->numberRows];
                    coveragesample.anyPointSeen = false;

                    coveragesampleTmp.histpoints.clear();

                    // for each observation Antenna do
                    foreach(Antenna* curObservationAntenna, m_constellationStudySpaceObjectList.at(i).observation)
                    //if(m_constellationStudySpaceObjectList.at(i).observation)
                    {

                        /*if (currentTime > m_sampleTimes.at(m_sampleTimes.length()-103))
                        {
                            QMessageBox::warning(NULL,"",QObject::tr("antenna"));
                        }*/
                    coveragesampleTmp.curtime = currentTime;
                    coveragesampleTmp.curpoints.clear();
                    coveragesampleTmp.lowestRow = m_discreteMesh->numberRows;
                    coveragesampleTmp.highestRow = -1;
                    coveragesampleTmp.firstPoints = new int[m_discreteMesh->numberRows];
                    coveragesampleTmp.anyPointSeen = false;


                    int ntemp = m_constellationStudySpaceObjectList.at(i).coveragesample.length();
                    // first time step: all points are checked whether they are in the observation
                    // cone of view of the actual space object
   // following, there are tree algorithms, the covered points can be computed.
   // the first two ones are quite slow, but fail-safe, the third one is much faster, but it
   // but it bases on the assumption that the satellite footprint, depending on the
   // observation antenna setting, is horizontally convex. I.e. in each mesh line/row there
   // is at most one connected domain discretized by mesh points.

              // Algorithm 1:
                    //search in meshAsList (slow, but fail-safe)>>>>>>>>>>>>
/*
                    if (ntemp == 0)
                    {
                        for(int k=0; k<m_discreteMesh->meshAsList.length(); k++)
                        {
                            if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->meshAsList.at(k), m_body, currentTime, curObservationAntenna))
                            {
                                coveragesampleTmp.curpoints.append(m_discreteMesh->meshAsList.at(k));
                                if(m_constellationStudySpaceObjectList.length()<=36)
                                {
                                    if(!m_discreteMesh->meshAsList.at(k).seen[i])
                                    {
                                        DiscretizationPoint p = m_discreteMesh->meshAsList.takeAt(k);
                                        p.seen[i]= true;
                                        m_discreteMesh->meshAsList.insert(k,p);
                                        coveragesampleTmp.histpoints.append(m_discreteMesh->meshAsList.at(k));
                                    }
                                }
                                else
                                {
                                    QMessageBox::warning(NULL,"","Number of satellites is bounded by 36");
                                }
                            }
                        }
                    }else{
                        if (coveragesampleTmp.histpoints.isEmpty())
                        {
                            coveragesampleTmp.histpoints.append(m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).histpoints);
                        }
                        for(int k=0; k<m_discreteMesh->meshAsList.length(); k++)
                        {

                            if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->meshAsList.at(k), m_body, currentTime, curObservationAntenna))
                            {
                                coveragesampleTmp.curpoints.append(m_discreteMesh->meshAsList.at(k));
                                if(!m_discreteMesh->meshAsList.at(k).seen[i])
                                {
                                    DiscretizationPoint p = m_discreteMesh->meshAsList.takeAt(k);
                                    p.seen[i]=true;
                                    m_discreteMesh->meshAsList.insert(k,p);
                                    coveragesampleTmp.histpoints.append(m_discreteMesh->meshAsList.at(k));
                                }
                            }

                        }

                    }*/


                    // <<<<<<<<<<< search in meshAsList (slow, but fail-safe)
                    /*
              // Algorithm 2:
                    // search directly in discreteMesh  (also slow, but fail-safe) >>>>>>>>>>
                    if (ntemp == 0)
                    {
                        for(int row=0; row<m_discreteMesh->numberRows; row++)
                        {
                            for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                            {
                                if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                {
                                    coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                }
                            }
                        }
                    }else{
                        if (coveragesampleTmp.histpoints.isEmpty())
                        {
                            coveragesampleTmp.histpoints.append(m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).histpoints);
                        }
                        for(int row=0; row<m_discreteMesh->numberRows; row++)
                        {
                            for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                            {
                                if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                {
                                    coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                }
                            }
                        }
                    }
                    */
                    // <<<<<<<<<<<<<<< search directly in discreteMesh (also slow, but fail-safe)

              // Algorithm 3:

                    // search directly in discreteMesh
                    // optimized -> faster    >>>>>>>>>>>>
                    if (ntemp == 0) // search in the whole mesh for covered points
                    {
                        bool lowestRowFound = false;
                        int row = 0;
                        bool rowInView = false;
                        while(!rowInView && row<m_discreteMesh->numberRows)
                        {
                            int column = 0;
                            while(column < m_discreteMesh->discreteMesh[row].count()
                                  && !visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                            {
                                column++;
                            }
                            if(column < m_discreteMesh->discreteMesh[row].count())
                            {
                                rowInView = true;
                                lowestRowFound = true;
                                coveragesampleTmp.lowestRow = row;
                            }else{
                                row++;
                            }
                        }
                        if(rowInView)
                        {
                            // satellite i covers at least one point
                            while(rowInView && row < m_discreteMesh->numberRows)
                            {
                                rowInView = false;
                                int column = 0;
                                while(column < m_discreteMesh->discreteMesh[row].count()
                                      && !visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                {
                                    column++;
                                }
                                if(column < m_discreteMesh->discreteMesh[row].count())
                                {
                                    rowInView = true;
                                    coveragesampleTmp.firstPoints[row] = column;
                                    if(column == 0)
                                    {
                                        int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                        while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, curObservationAntenna)
                                              && tempColumn > 0)
                                        {
                                            coveragesampleTmp.firstPoints[row] = tempColumn;
                                            column = tempColumn;
                                            tempColumn--;
                                        }
                                    }
                                    coveragesampleTmp.highestRow = row;
                                    coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                    column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    int columnCounter = 1;
                                    while (columnCounter < m_discreteMesh->discreteMesh[row].count()
                                           && visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna) )
                                    {
                                        coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        columnCounter++;
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    }
                                }
                                row++;
                            }
                        }
                        if (lowestRowFound) coveragesampleTmp.anyPointSeen = true;
                    }else{
                        if (coveragesampleTmp.histpoints.isEmpty())
                        {
                            coveragesampleTmp.histpoints.append(m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).histpoints);
                        }
                        bool lowestRowFound = false;
                        if(m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).anyPointSeen)
                        {
                            // first case: in the last time step there was at least one mesh point covered by space object i
                            int oldLowestRow = m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).lowestRow;
                            int oldHighestRow = m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).highestRow;
                            for(int row=oldLowestRow; row<=oldHighestRow; row++)
                            {
                                bool rowInView = false;
                                int column = m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).firstPoints[row];
                                int columnCounter = 0;
                                if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                { // search to the left
                                    rowInView = true;
                                    coveragesampleTmp.firstPoints[row]=column;
                                    if(!lowestRowFound)
                                    {
                                        coveragesampleTmp.lowestRow = row;
                                        lowestRowFound = true;
                                    }
                                    coveragesampleTmp.highestRow = row;
                                    coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                    column -= 1;
                                    if(column<0)
                                    {
                                        column += m_discreteMesh->discreteMesh[row].count();
                                    }
                                    while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna)
                                          && columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                    {
                                        columnCounter++;
                                        coveragesampleTmp.firstPoints[row]=column;
                                        coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column -= 1;
                                        if(column<0)
                                        {
                                            column += m_discreteMesh->discreteMesh[row].count();
                                        }
                                    }
                                    column = (m_constellationStudySpaceObjectList.at(i).coveragesample.at(ntemp-1).firstPoints[row]+1) % m_discreteMesh->discreteMesh[row].count();
                                    while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna) &&
                                          columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                    {
                                        columnCounter++;
                                        coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    }
                                }else{ // search to the right
                                    column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    columnCounter++;
                                    while(!visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna)
                                          && columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                    {
                                        columnCounter++;
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    }
                                    if(columnCounter < m_discreteMesh->discreteMesh[row].count())
                                    {
                                        rowInView = true;
                                        coveragesampleTmp.highestRow = row;
                                        coveragesampleTmp.firstPoints[row]=column;
                                        if(!lowestRowFound)
                                        {
                                            coveragesampleTmp.lowestRow = row;
                                            lowestRowFound = true;
                                        }
                                        coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                        columnCounter++;
                                        while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna) &&
                                              columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                        {
                                            coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                            column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                            columnCounter++;
                                        }
                                    }
                                }
                            }
                            // search new lowest and highest row respectively if not already found
                            if(lowestRowFound && coveragesampleTmp.highestRow == oldHighestRow && oldHighestRow < m_discreteMesh->numberRows-1)
                            {
                                // search in the next higher row
                                int row = oldHighestRow + 1;
                                bool rowInView = true;
                                while(rowInView && row<m_discreteMesh->numberRows)
                                {
                                    rowInView = false;
                                    for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                                    {
                                        if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                        {
                                            if(!rowInView)
                                            {
                                                rowInView = true;
                                                coveragesampleTmp.firstPoints[row] = column;
                                                if(column == 0)
                                                {
                                                    int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                                    while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, curObservationAntenna)
                                                          && tempColumn > 0)
                                                    {
                                                        coveragesampleTmp.firstPoints[row] = tempColumn;
                                                        tempColumn--;
                                                    }
                                                }
                                                coveragesampleTmp.highestRow = row;

                                            }
                                            coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                        }
                                    }
                                    row++;
                                }
                            }
                            if (lowestRowFound)
                            {
                                coveragesampleTmp.anyPointSeen = true;
                                if (coveragesampleTmp.lowestRow == oldLowestRow && oldLowestRow > 0){
                                    // search in the next lower row
                                    int row = oldLowestRow -1;
                                    bool rowInView = true;
                                    while(rowInView && row>=0)
                                    {
                                        rowInView = false;
                                        for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                                        {
                                            if(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                            {
                                                if(!rowInView)
                                                {
                                                    rowInView = true;
                                                    coveragesampleTmp.firstPoints[row] = column;
                                                    if(column == 0)
                                                    {
                                                        int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                                        while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, curObservationAntenna)
                                                              && tempColumn > 0)
                                                        {
                                                            coveragesampleTmp.firstPoints[row] = tempColumn;
                                                            tempColumn--;
                                                        }
                                                    }
                                                    coveragesampleTmp.lowestRow = row;
                                                }
                                                coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                                {
                                                    m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                    coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                }
                                            }
                                        }
                                        row--;
                                    }
                                }
                            }else{
                                //  search in the whole mesh for covered points if there is no point seen
                                //  by the above algorithm in the current time step
                                int row = 0;
                                bool rowInView = false;
                                while(!rowInView && row<m_discreteMesh->numberRows)
                                {
                                    int column = 0;
                                    while(column < m_discreteMesh->discreteMesh[row].count()
                                          && !visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                    {
                                        column++;
                                    }
                                    if(column < m_discreteMesh->discreteMesh[row].count())
                                    {
                                        rowInView = true;
                                        lowestRowFound = true;
                                        coveragesampleTmp.lowestRow = row;
                                    }else{
                                        row++;
                                    }
                                }
                                if(rowInView)
                                {
                                    // satellite i covers at least one point
                                    while(rowInView && row < m_discreteMesh->numberRows)
                                    {
                                        rowInView = false;
                                        int column = 0;
                                        while(column < m_discreteMesh->discreteMesh[row].count()
                                              && !visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                        {
                                            column++;
                                        }
                                        if(column < m_discreteMesh->discreteMesh[row].count())
                                        {
                                            rowInView = true;
                                            coveragesampleTmp.firstPoints[row] = column;
                                            if(column == 0)
                                            {
                                                int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                                while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, curObservationAntenna)
                                                      && tempColumn > 0)
                                                {
                                                    coveragesampleTmp.firstPoints[row] = tempColumn;
                                                    column = tempColumn;
                                                    tempColumn--;
                                                }
                                            }
                                            coveragesampleTmp.highestRow = row;
                                            coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                            column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                            int columnCounter = 1;
                                            while (columnCounter < m_discreteMesh->discreteMesh[row].count()
                                                   && visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna) )
                                            {
                                                coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                                {
                                                    m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                    coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                }
                                                columnCounter++;
                                                column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                            }
                                        }
                                        row++;
                                    }
                                }
                                if (lowestRowFound) coveragesampleTmp.anyPointSeen = true;
                            }
                        }
                        else{
                            // second case: in the last time step there where no points seen by space object i
                            //              -> search in the whole mesh for covered points
                            int row = 0;
                            bool rowInView = false;
                            while(!rowInView && row<m_discreteMesh->numberRows)
                            {
                                int column = 0;
                                while(column < m_discreteMesh->discreteMesh[row].count()
                                      && !visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                {
                                    column++;
                                }
                                if(column < m_discreteMesh->discreteMesh[row].count())
                                {
                                    rowInView = true;
                                    lowestRowFound = true;
                                    coveragesampleTmp.lowestRow = row;
                                }else{
                                    row++;
                                }
                            }
                            if(rowInView)
                            {
                                // satellite i covers at least one point
                                while(rowInView && row < m_discreteMesh->numberRows)
                                {
                                    rowInView = false;
                                    int column = 0;
                                    while(column < m_discreteMesh->discreteMesh[row].count()
                                          && !visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna))
                                    {
                                        column++;
                                    }
                                    if(column < m_discreteMesh->discreteMesh[row].count())
                                    {
                                        rowInView = true;
                                        coveragesampleTmp.firstPoints[row] = column;
                                        if(column == 0)
                                        {
                                            int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                            while(visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, curObservationAntenna)
                                                  && tempColumn > 0)
                                            {
                                                coveragesampleTmp.firstPoints[row] = tempColumn;
                                                column = tempColumn;
                                                tempColumn--;
                                            }
                                        }
                                        coveragesampleTmp.highestRow = row;
                                        coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                        int columnCounter = 1;
                                        while (columnCounter < m_discreteMesh->discreteMesh[row].count()
                                               && visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, curObservationAntenna) )
                                        {
                                            coveragesampleTmp.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesampleTmp.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                            columnCounter++;
                                            column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                        }
                                    }
                                    row++;
                                }
                            }
                            if (lowestRowFound) coveragesampleTmp.anyPointSeen = true;
                        }
                    }

                    // <<<<<<<<<<<<<<< search directly in discreteMesh
                    //                 optimized -> faster


                    // restore coveragesampleTmp and add values to coveragesample
                    /*if (currentTime > m_sampleTimes.at(m_sampleTimes.length()-103))
                    {
                        QMessageBox::warning(NULL,"",QObject::tr("restoring los"));
                    }*/
                    coveragesample.anyPointSeen &= coveragesampleTmp.anyPointSeen;
                    /*if (currentTime > m_sampleTimes.at(m_sampleTimes.length()-103))
                    {
                        QMessageBox::warning(NULL,"",QObject::tr("restoring los"));
                    }*/
                    coveragesample.curpoints.append(coveragesampleTmp.curpoints);
                    /*if (currentTime > m_sampleTimes.at(m_sampleTimes.length()-103))
                    {
                        QMessageBox::warning(NULL,"",QObject::tr("restoring los"));
                    }*/

                    //delete coveragesample.firstPoints;
                    //if (currentTime > m_sampleTimes.at(m_sampleTimes.length()-103))
                    //{
                    //    QMessageBox::warning(NULL,"",QObject::tr("%1").arg(coveragesampleTmp.histpoints.length()));
                    //}
                    //coveragesampleTmp.histpoints.clear();


                    }
                    coveragesample.histpoints.append(coveragesampleTmp.histpoints);

                    // covered area in per cent
                    coveragesample.coveredAreaInPerCent = 100.*( ((double)(coveragesample.curpoints.length())) / (double)m_discreteMesh->numberPoints  );
                    // covered area in square kilometers
                    coveragesample.coveredArea = (4.*mypi*m_body->meanRadius()*m_body->meanRadius())*coveragesample.coveredAreaInPerCent/100.;
//                    cout<<coveragesample.coveredAreaInPerCent<<currentTime<<endl;
//                    QMessageBox::warning(this,tr("Parameters"),tr("Middle radius must be larger than rear radius"));
//                    QMessageBox::warning(NULL,tr("covered area in per cent: '%1'").arg(coveragesample.coveredAreaInPerCent));

                    /*if (currentTime > m_sampleTimes.at(m_sampleTimes.length()-103))
                    {
                        QMessageBox::warning(NULL,"",QObject::tr("prozente ausgerechnet"));
                    }*/
                }
                //QMessageBox::warning(NULL,"","done2");
                // the existing connections to other space objects
                LinkSample linksample;
                if (m_calcSOLink)
                {
                    linksample.curtime = currentTime;
                    linksample.connection = QList<SpaceObject*>();
                    for (int j = 0; j < m_constellationStudySpaceObjectList.length(); j++)
                    {
                        if (i != j)
                        {
                            m_body = m_constellationStudySpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                            if (visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_constellationStudySpaceObjectList.at(j).m_spaceObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibilityT) &
                                visibility(m_constellationStudySpaceObjectList.at(j).m_spaceObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(j).visibilityR))
                            {

                                linksample.connection.append(m_constellationStudySpaceObjectList.at(j).m_spaceObject);
                            }
                        }
                    }
                }

                // add for each time sample and space Object
                // the existing connections to other ground objects
                GroundLinkSample groundlinksample;
                if (m_calcGOLink)
                {
                    groundlinksample.curtime = currentTime;
                    groundlinksample.connection = QList<GroundObject*>();
                    for (int j = 0; j < m_scenario->groundObjects().length(); j++)
                    {
                        GroundObject* groundObject = m_scenario->groundObjects().at(j);
                        QList<Antenna*> gReceiver = QList<Antenna*>();
                        //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("anzahl antennen %1").arg(groundObject->receiver().length()));
                        for (int k = 0; k<groundObject->receiver().length(); k++)
                        {
                            //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("antenne"));
                            PSAntennaObject* psa = groundObject->receiver().at(k);
                            //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("receiver antenne %1 %2 %3 %4").arg(psa->getAzimuth()).arg(psa->getElevation()).arg(psa->getConeShape()).arg(psa->getConeAngle()));
                            gReceiver.append(new Antenna(psa->getAzimuth(),psa->getElevation(),psa->getConeAngle(),psa->getConeShape()));
                        }
                        QList<Antenna*> gTransmitter = QList<Antenna*>();
                        for (int k = 0; k<groundObject->transmitter().length(); k++)
                        {
                            //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("antenne"));
                            PSAntennaObject* psa = groundObject->transmitter().at(k);
                            //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("transmitter antenne %1 %2 %3 %4").arg(psa->getAzimuth()).arg(psa->getElevation()).arg(psa->getConeShape()).arg(psa->getConeAngle()));
                            gTransmitter.append(new Antenna(psa->getAzimuth(),psa->getElevation(),psa->getConeAngle(),psa->getConeShape()));
                        }

                        m_body = m_constellationStudySpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        //Antenna* gAntenna = new Antenna(0.0, -3.14/2, 9*3.14/20,0);
                        bool a1 = visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibilityT);
                        bool a2 = visibility(groundObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gReceiver);
                        bool a3 = visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibilityR);
                        bool a4 = visibility(groundObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gTransmitter);
                        //if (a1 && a2 && a3 && a4) QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("verbindung"));
                        //if (a1 && a2) QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("verbindung 12"));
                        //if (a3 && a4) QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("verbindung 34"));



                        if (visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibilityT) &&
                            visibility(groundObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gReceiver) &&
                            visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibilityR) &&
                            visibility(groundObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gTransmitter))
                        {
                            groundlinksample.connection.append(groundObject);
                        }
                    }
                }
                ConstellationStudySpaceObject tmpASO = m_constellationStudySpaceObjectList.takeAt(i);
                if (m_calcCoverage){
                    tmpASO.coveragesample.append(coveragesample);
                }
                if (m_calcSOLink){
                    tmpASO.linksamples.append(linksample);
                }
                if (m_calcGOLink){
                    tmpASO.groundlinksamples.append(groundlinksample);
                }
                m_constellationStudySpaceObjectList.insert(i,tmpASO);
                //QMessageBox::warning(NULL,"","done3");

            }
        }
    }
}



ConstellationStudy::~ConstellationStudy()
{
}
bool linkSampleLessThan(const LinkSample &s1, const LinkSample &s2)
{
    return s1.curtime < s2.curtime;
}
bool groundlinkSampleLessThan(const GroundLinkSample &s1, const GroundLinkSample &s2)
{
    return s1.curtime < s2.curtime;
}
bool coverageSampleLessThan(const CoverageSample &s1, const CoverageSample &s2)
{
    return s1.curtime < s2.curtime;
}






















           /*

                // the existing connections to other space objects
                LinkSample linksample;
                if (m_calcSOLink){
                    linksample.curtime = currentTime;
                    linksample.connection = QList<SpaceObject*>();
                    for (int j = i+1; j < m_constellationStudySpaceObjectList.length(); j++)
                    {
                        //SpaceObject* spaceObject2 = m_scenario->spaceObjects().at(j);
                        // temporarily definition of antennas for spaceObject2
                        m_body = m_constellationStudySpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        if (visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_constellationStudySpaceObjectList.at(j).m_spaceObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibility) &
                            visibility(m_constellationStudySpaceObjectList.at(j).m_spaceObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(j).visibility))
                        {

                            linksample.connection.append(m_constellationStudySpaceObjectList.at(j).m_spaceObject);
                        }
                    }
                }
                // add for each time sample and space Object
                // the existing connections to other ground objects
                GroundLinkSample groundlinksample;
                if (m_calcGOLink){
                    groundlinksample.curtime = currentTime;
                    groundlinksample.connection = QList<GroundObject*>();
                    for (int j = 0; j < m_scenario->groundObjects().length(); j++)
                    {
                        GroundObject* groundObject = m_scenario->groundObjects().at(j);
                        m_body = m_constellationStudySpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        Antenna* gAntenna = new Antenna(0.0, -3.14/2, 9*3.14/20);
                        if (visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibility) &
                            visibility(groundObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gAntenna))
                        {
                            GroundObject* groundObject = m_scenario->groundObjects().at(j);
                            m_body = m_constellationStudySpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                            if (visibility(m_constellationStudySpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_constellationStudySpaceObjectList.at(i).visibility) &
                                visibility(groundObject, m_constellationStudySpaceObjectList.at(i).m_spaceObject, m_body, currentTime))
                            {
                                groundlinksample.connection.append(groundObject);
                            }
                        }
                    }
                    ConstellationStudy tmpASO = m_constellationStudySpaceObjectList.takeAt(i);
                    if (m_calcCoverage){
                        tmpASO.coveragesample.append(coveragesample);
                    }
                    if (m_calcSOLink){
                        tmpASO.linksamples.append(linksample);
                    }
                    if (m_calcGOLink){
                        tmpASO.groundlinksamples.append(groundlinksample);
                    }
                    m_constellationStudySpaceObjectList.insert(i,tmpASO);

                }
            }
        }
    }else
    {
        QMessageBox::warning(NULL,"","Number of satellites is bounded by 36");
    }
}
CStudy::~CStudy()
{
}
bool linkSampleLessThan(const LinkSample &s1, const LinkSample &s2)
{
    return s1.curtime < s2.curtime;
}
*/
