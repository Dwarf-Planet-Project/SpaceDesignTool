#include "canalysis.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/nedTOfixed.h"
#include <QCheckBox>

AnaSpaceObject::AnaSpaceObject(SpaceObject* spaceObject, Antenna* obsAnt):
        m_spaceObject(spaceObject),
        linksamples(QList<LinkSample>()),
        groundlinksamples(QList<GroundLinkSample>()),
        coveragesample(QList<CoverageSample>()),
        observation(obsAnt), //Claas: observation Antenna is set initially (if you don't want one, set obsAnt == NULL in function call)
        visibility(QList<Antenna*>()),
        m_chosen(true)
{
    // Declaration of comm. antennas (temporarily here)
    visibility.append(new Antenna(0.0, 0.0, 3.14/4));
    visibility.append(new Antenna(3.14, 0.0, 3.14/4));
    asocolor = spaceObject->trajectoryColor();
    asoname = spaceObject->name();
}





AnaSpaceObject::~AnaSpaceObject()
{
}



void
AnaSpaceObject::choose(bool c)
{
    m_chosen = c;
}


Analysis::Analysis(PropagatedScenario* scenario, bool m_calcCoverage, bool m_calcSOLink, bool m_calcGOLink):
        m_askfeature(new AnalysisPropDialog(NULL))
{
    // analysis features dialog
    //m_askfeature->exec();
    //m_askfeature->getCheckVal(m_calcCoverage, m_calcSOLink, m_calcGOLink  );

    // Initialization
    m_scenario = scenario;
    m_anaSpaceObjectList = QList<AnaSpaceObject>();
    m_startTime =  2000000000.;
    m_endTime   = -2000000000.;
    m_timeStep  =   400000000.;

    // save Space Objects and determine time range
    foreach(SpaceObject* spaceObject, scenario->spaceObjects())
    {
        m_endTime = spaceObject->missionEndTime();
        m_startTime = spaceObject->missionStartTime();
        m_anaSpaceObjectList.append(AnaSpaceObject(spaceObject, new Antenna(2.76, 3*3.14/8, 3.14/16)));
    }

    // todo: samples have to depend on the scenario (but how??)
    int timesteps = 600; // faster analysis propagation with less timesteps
    m_timeStep = (m_endTime - m_startTime)/timesteps;
    m_samples = QList<double>();

    for (int k = 0; k < timesteps; k++)
    {
        m_samples.append(m_startTime + k*m_timeStep);
    }
    if(m_anaSpaceObjectList.length()<=36)
    {
        // analyze for each Space Object
        for (int i = 0; i < m_anaSpaceObjectList.length(); i++)
        {
            // creating discrete mesh
            // TODO define body global
            m_body = m_anaSpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
            if(i==0)
            {
                // TODO: is going to be set by the user
                int numberRows = 80;
                m_discreteMesh = new DiscreteMesh(m_body,numberRows);
            }
            foreach(double currentTime, m_samples)
            {
                CoverageSample coveragesample;
                if (m_calcCoverage){
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

                    int ntemp = m_anaSpaceObjectList.at(i).coveragesample.length();
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
                            if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->meshAsList.at(k), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                            {
                                coveragesample.curpoints.append(m_discreteMesh->meshAsList.at(k));
                                if(m_anaSpaceObjectList.length()<=36)
                                {
                                    if(!m_discreteMesh->meshAsList.at(k).seen[i])
                                    {
                                        DiscretizationPoint p = m_discreteMesh->meshAsList.takeAt(k);
                                        p.seen[i]= true;
                                        m_discreteMesh->meshAsList.insert(k,p);
                                        coveragesample.histpoints.append(m_discreteMesh->meshAsList.at(k));
                                    }
                                }else
                                {
                                    QMessageBox::warning(NULL,"","Number of satellites is bounded by 36");
                                }
                            }
                        }
                    }else{
                        coveragesample.histpoints.append(m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).histpoints);
                        for(int k=0; k<m_discreteMesh->meshAsList.length(); k++)
                        {
                            if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->meshAsList.at(k), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                            {
                                coveragesample.curpoints.append(m_discreteMesh->meshAsList.at(k));
                                if(!m_discreteMesh->meshAsList.at(k).seen[i])
                                {
                                    DiscretizationPoint p = m_discreteMesh->meshAsList.takeAt(k);
                                    p.seen[i]=true;
                                    m_discreteMesh->meshAsList.insert(k,p);
                                    coveragesample.histpoints.append(m_discreteMesh->meshAsList.at(k));
                                }
                            }
                        }
                    }
                    */
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
                                if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                {
                                    coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                }
                            }
                        }
                    }else{
                        coveragesample.histpoints.append(m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).histpoints);
                        for(int row=0; row<m_discreteMesh->numberRows; row++)
                        {
                            for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                            {
                                if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                {
                                    coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
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
                                  && !visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                            {
                                column++;
                            }
                            if(column < m_discreteMesh->discreteMesh[row].count())
                            {
                                rowInView = true;
                                lowestRowFound = true;
                                coveragesample.lowestRow = row;
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
                                      && !visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                {
                                    column++;
                                }
                                if(column < m_discreteMesh->discreteMesh[row].count())
                                {
                                    rowInView = true;
                                    coveragesample.firstPoints[row] = column;
                                    if(column == 0)
                                    {
                                        int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                        while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                              && tempColumn > 0)
                                        {
                                            coveragesample.firstPoints[row] = tempColumn;
                                            column = tempColumn;
                                            tempColumn--;
                                        }
                                    }
                                    coveragesample.highestRow = row;
                                    coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                    column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    int columnCounter = 1;
                                    while (columnCounter < m_discreteMesh->discreteMesh[row].count()
                                           && visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation) )
                                    {
                                        coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        columnCounter++;
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    }
                                }
                                row++;
                            }
                        }
                        if (lowestRowFound) coveragesample.anyPointSeen = true;
                    }else{
                        coveragesample.histpoints.append(m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).histpoints);
                        bool lowestRowFound = false;
                        if(m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).anyPointSeen)
                        {
                            // first case: in the last time step there was at least one mesh point covered by space object i
                            int oldLowestRow = m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).lowestRow;
                            int oldHighestRow = m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).highestRow;
                            for(int row=oldLowestRow; row<=oldHighestRow; row++)
                            {
                                bool rowInView = false;
                                int column = m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).firstPoints[row];
                                int columnCounter = 0;
                                if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                { // search to the left
                                    rowInView = true;
                                    coveragesample.firstPoints[row]=column;
                                    if(!lowestRowFound)
                                    {
                                        coveragesample.lowestRow = row;
                                        lowestRowFound = true;
                                    }
                                    coveragesample.highestRow = row;
                                    coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                    {
                                        m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                        coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                    }
                                    column -= 1;
                                    if(column<0)
                                    {
                                        column += m_discreteMesh->discreteMesh[row].count();
                                    }
                                    while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                          && columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                    {
                                        columnCounter++;
                                        coveragesample.firstPoints[row]=column;
                                        coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column -= 1;
                                        if(column<0)
                                        {
                                            column += m_discreteMesh->discreteMesh[row].count();
                                        }
                                    }
                                    column = (m_anaSpaceObjectList.at(i).coveragesample.at(ntemp-1).firstPoints[row]+1) % m_discreteMesh->discreteMesh[row].count();
                                    while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation) &&
                                          columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                    {
                                        columnCounter++;
                                        coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    }
                                }else{ // search to the right
                                    column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    columnCounter++;
                                    while(!visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                          && columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                    {
                                        columnCounter++;
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                    }
                                    if(columnCounter < m_discreteMesh->discreteMesh[row].count())
                                    {
                                        rowInView = true;
                                        coveragesample.highestRow = row;
                                        coveragesample.firstPoints[row]=column;
                                        if(!lowestRowFound)
                                        {
                                            coveragesample.lowestRow = row;
                                            lowestRowFound = true;
                                        }
                                        coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                        columnCounter++;
                                        while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation) &&
                                              columnCounter < m_discreteMesh->discreteMesh[row].count() )
                                        {
                                            coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                            column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                            columnCounter++;
                                        }
                                    }
                                }
                            }
                            // search new lowest and highest row respectively if not already found
                            if(lowestRowFound && coveragesample.highestRow == oldHighestRow && oldHighestRow < m_discreteMesh->numberRows-1)
                            {
                                // search in the next higher row
                                int row = oldHighestRow + 1;
                                bool rowInView = true;
                                while(rowInView && row<m_discreteMesh->numberRows)
                                {
                                    rowInView = false;
                                    for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                                    {
                                        if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                        {
                                            if(!rowInView)
                                            {
                                                rowInView = true;
                                                coveragesample.firstPoints[row] = column;
                                                if(column == 0)
                                                {
                                                    int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                                    while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                                          && tempColumn > 0)
                                                    {
                                                        coveragesample.firstPoints[row] = tempColumn;
                                                        tempColumn--;
                                                    }
                                                }
                                                coveragesample.highestRow = row;

                                            }
                                            coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                        }
                                    }
                                    row++;
                                }
                            }
                            if (lowestRowFound)
                            {
                                coveragesample.anyPointSeen = true;
                                if (coveragesample.lowestRow == oldLowestRow && oldLowestRow > 0){
                                    // search in the next lower row
                                    int row = oldLowestRow -1;
                                    bool rowInView = true;
                                    while(rowInView && row>=0)
                                    {
                                        rowInView = false;
                                        for(int column=0; column<m_discreteMesh->discreteMesh[row].count(); column++)
                                        {
                                            if(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                            {
                                                if(!rowInView)
                                                {
                                                    rowInView = true;
                                                    coveragesample.firstPoints[row] = column;
                                                    if(column == 0)
                                                    {
                                                        int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                                        while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                                              && tempColumn > 0)
                                                        {
                                                            coveragesample.firstPoints[row] = tempColumn;
                                                            tempColumn--;
                                                        }
                                                    }
                                                    coveragesample.lowestRow = row;
                                                }
                                                coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                                {
                                                    m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                    coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
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
                                          && !visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                    {
                                        column++;
                                    }
                                    if(column < m_discreteMesh->discreteMesh[row].count())
                                    {
                                        rowInView = true;
                                        lowestRowFound = true;
                                        coveragesample.lowestRow = row;
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
                                              && !visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                        {
                                            column++;
                                        }
                                        if(column < m_discreteMesh->discreteMesh[row].count())
                                        {
                                            rowInView = true;
                                            coveragesample.firstPoints[row] = column;
                                            if(column == 0)
                                            {
                                                int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                                while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                                      && tempColumn > 0)
                                                {
                                                    coveragesample.firstPoints[row] = tempColumn;
                                                    column = tempColumn;
                                                    tempColumn--;
                                                }
                                            }
                                            coveragesample.highestRow = row;
                                            coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                            column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                            int columnCounter = 1;
                                            while (columnCounter < m_discreteMesh->discreteMesh[row].count()
                                                   && visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation) )
                                            {
                                                coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                                {
                                                    m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                    coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                                }
                                                columnCounter++;
                                                column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                            }
                                        }
                                        row++;
                                    }
                                }
                                if (lowestRowFound) coveragesample.anyPointSeen = true;
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
                                      && !visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                {
                                    column++;
                                }
                                if(column < m_discreteMesh->discreteMesh[row].count())
                                {
                                    rowInView = true;
                                    lowestRowFound = true;
                                    coveragesample.lowestRow = row;
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
                                          && !visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation))
                                    {
                                        column++;
                                    }
                                    if(column < m_discreteMesh->discreteMesh[row].count())
                                    {
                                        rowInView = true;
                                        coveragesample.firstPoints[row] = column;
                                        if(column == 0)
                                        {
                                            int tempColumn = m_discreteMesh->discreteMesh[row].count() - 1;
                                            while(visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(tempColumn), m_body, currentTime, m_anaSpaceObjectList.at(i).observation)
                                                  && tempColumn > 0)
                                            {
                                                coveragesample.firstPoints[row] = tempColumn;
                                                column = tempColumn;
                                                tempColumn--;
                                            }
                                        }
                                        coveragesample.highestRow = row;
                                        coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                        {
                                            m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                            coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                        }
                                        column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                        int columnCounter = 1;
                                        while (columnCounter < m_discreteMesh->discreteMesh[row].count()
                                               && visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_discreteMesh->discreteMesh[row].at(column), m_body, currentTime, m_anaSpaceObjectList.at(i).observation) )
                                        {
                                            coveragesample.curpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            if(!m_discreteMesh->discreteMesh[row].at(column).seen[i])
                                            {
                                                m_discreteMesh->discreteMesh[row].at(column).seen[i]=true;
                                                coveragesample.histpoints.append(m_discreteMesh->discreteMesh[row].at(column));
                                            }
                                            columnCounter++;
                                            column = (column+1) % m_discreteMesh->discreteMesh[row].count();
                                        }
                                    }
                                    row++;
                                }
                            }
                            if (lowestRowFound) coveragesample.anyPointSeen = true;
                        }
                    }
                    // <<<<<<<<<<<<<<< search directly in discreteMesh
                    //                 optimized -> faster
                }

                // the existing connections to other space objects
                LinkSample linksample;
                if (m_calcSOLink){
                    linksample.curtime = currentTime;
                    linksample.connection = QList<SpaceObject*>();
                    for (int j = i+1; j < m_anaSpaceObjectList.length(); j++)
                    {
                        //SpaceObject* spaceObject2 = m_scenario->spaceObjects().at(j);
                        // temporarily definition of antennas for spaceObject2
                        m_body = m_anaSpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        if (visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_anaSpaceObjectList.at(j).m_spaceObject, m_body, currentTime, m_anaSpaceObjectList.at(i).visibility) &
                            visibility(m_anaSpaceObjectList.at(j).m_spaceObject, m_anaSpaceObjectList.at(i).m_spaceObject, m_body, currentTime, m_anaSpaceObjectList.at(j).visibility))
                        {

                            linksample.connection.append(m_anaSpaceObjectList.at(j).m_spaceObject);
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
                        m_body = m_anaSpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        Antenna* gAntenna = new Antenna(0.0, -3.14/2, 9*3.14/20);
                        if (visibility(m_anaSpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_anaSpaceObjectList.at(i).visibility) &
                            visibility(groundObject, m_anaSpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gAntenna))
                        {
                            groundlinksample.connection.append(groundObject);
                        }
                    }
                }
                AnaSpaceObject tmpASO = m_anaSpaceObjectList.takeAt(i);
                if (m_calcCoverage){
                    tmpASO.coveragesample.append(coveragesample);
                }
                if (m_calcSOLink){
                    tmpASO.linksamples.append(linksample);
                }
                if (m_calcGOLink){
                    tmpASO.groundlinksamples.append(groundlinksample);
                }
                m_anaSpaceObjectList.insert(i,tmpASO);

            }
        }
    }
    m_askfeature=NULL;
}



Analysis::~Analysis()
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
                    for (int j = i+1; j < m_anaSpaceObjectList.length(); j++)
                    {
                        //SpaceObject* spaceObject2 = m_scenario->spaceObjects().at(j);
                        // temporarily definition of antennas for spaceObject2
                        m_body = m_anaSpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        if (visibility(m_anaSpaceObjectList.at(i).m_spaceObject, m_anaSpaceObjectList.at(j).m_spaceObject, m_body, currentTime, m_anaSpaceObjectList.at(i).visibility) &
                            visibility(m_anaSpaceObjectList.at(j).m_spaceObject, m_anaSpaceObjectList.at(i).m_spaceObject, m_body, currentTime, m_anaSpaceObjectList.at(j).visibility))
                        {

                            linksample.connection.append(m_anaSpaceObjectList.at(j).m_spaceObject);
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
                        m_body = m_anaSpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                        Antenna* gAntenna = new Antenna(0.0, -3.14/2, 9*3.14/20);
                        if (visibility(m_anaSpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_anaSpaceObjectList.at(i).visibility) &
                            visibility(groundObject, m_anaSpaceObjectList.at(i).m_spaceObject, m_body, currentTime, gAntenna))
                        {
                            GroundObject* groundObject = m_scenario->groundObjects().at(j);
                            m_body = m_anaSpaceObjectList.at(i).m_spaceObject->mission().at(0)->centralBody();
                            if (visibility(m_anaSpaceObjectList.at(i).m_spaceObject, groundObject, m_body, currentTime, m_anaSpaceObjectList.at(i).visibility) &
                                visibility(groundObject, m_anaSpaceObjectList.at(i).m_spaceObject, m_body, currentTime))
                            {
                                groundlinksample.connection.append(groundObject);
                            }
                        }
                    }
                    AnaSpaceObject tmpASO = m_anaSpaceObjectList.takeAt(i);
                    if (m_calcCoverage){
                        tmpASO.coveragesample.append(coveragesample);
                    }
                    if (m_calcSOLink){
                        tmpASO.linksamples.append(linksample);
                    }
                    if (m_calcGOLink){
                        tmpASO.groundlinksamples.append(groundlinksample);
                    }
                    m_anaSpaceObjectList.insert(i,tmpASO);

                }
            }
        }
        m_askfeature=NULL;
    }else
    {
        QMessageBox::warning(NULL,"","Number of satellites is bounded by 36");
    }
}
Analysis::~Analysis()
{
}
bool linkSampleLessThan(const LinkSample &s1, const LinkSample &s2)
{
    return s1.curtime < s2.curtime;
}
*/
