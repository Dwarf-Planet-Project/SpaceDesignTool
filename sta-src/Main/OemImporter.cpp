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

#include "OemImporter.h"

#include <QStringList>
#include <QDebug>


struct OemKeyValue
{
    QString key;
    QString value;
};

static OemKeyValue ParseKeyValue(const QString& line)
{
    OemKeyValue kv;

    QStringList s = line.split('=', QString::SkipEmptyParts);
    if (s.size() == 2)
    {
        kv.key = s[0].trimmed();
        kv.value = s[1].trimmed();
    }

    return kv;
}


OemImporter::OemImporter(QTextStream* stream) :
    m_stream(stream),
    m_lineNumber(1),
    m_parserState(Oem_Begin)
{
}


/** Load an OEM file and convert it to an STA Space Scenario.
  *
  * \return a pointer to a complete space scenario, or NULL if
  * there was an error loading the scenario.
  */
SpaceScenario*
OemImporter::loadScenario()
{
    readHeader();
    SpaceScenario* scenario = new SpaceScenario();
    scenario->setName("New scenario");

    QSharedPointer<ScenarioSC> sc(new ScenarioSC());
    scenario->AbstractParticipant().append(sc);

    ScenarioExternalType* trajectory;

    bool skip = false;
    QString line;

    QString objectName;
    StaBody* center = NULL;


    while (m_parserState != Oem_End && m_parserState != Oem_Error)
    {
        if (skip)
        {
            skip = false;
        }
        else
        {
            line = m_stream->readLine().trimmed();
            m_lineNumber++;
        }

        if (m_stream->atEnd())
        {
            m_parserState = Oem_End;
        }
        else if (line.isEmpty())
        {
            // Nothing to do
        }
        else
        {
            switch (m_parserState)
            {
            case Oem_Begin:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "CCSDS_OEM_VERS")
                    {
                        m_parserState = Oem_AfterVersion;
                    }
                    else
                    {
                        raiseError("Version expected in header");
                    }
                }
                break;

            case Oem_AfterVersion:
                if (line.startsWith("COMMENT"))
                {
                    // Skip and do nothing
                }
                else
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "CREATION_DATE")
                    {
                        m_parserState = Oem_AfterCreationDate;
                    }
                    else
                    {
                        raiseError("Creation date expected in header");
                    }
                }
                break;

            case Oem_AfterCreationDate:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "ORIGINATOR")
                    {
                        m_parserState = Oem_AfterHeader;
                    }
                    else
                    {
                        raiseError("Creation date expected in header");
                    }
                }
                break;

            case Oem_AfterHeader:
                if (line.startsWith("COMMENT"))
                {
                    // Skip and do nothing
                }
                else if (line == "META_START")
                {
                    m_parserState = Oem_MetaStart;
                }
                else
                {
                    raiseError("Metadata block expected");
                }
                break;

            case Oem_MetaStart:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "OBJECT_NAME")
                    {
                        m_parserState = Oem_AfterObjectName;
                        objectName = kv.value;
                    }
                    else
                    {
                        raiseError("Object name expected");
                    }
                }
                break;

            case Oem_AfterObjectName:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "OBJECT_ID")
                    {
                        m_parserState = Oem_AfterObjectId;
                    }
                    else
                    {
                        raiseError("Object ID expected");
                    }
                }
                break;

            case Oem_AfterObjectId:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "CENTER_NAME")
                    {
                        m_parserState = Oem_AfterCenterName;
                        center = STA_SOLAR_SYSTEM->lookup(kv.value);
                        if (!center)
                        {
                            raiseError("Unknown central body " + kv.value);
                        }
                    }
                    else
                    {
                        raiseError("Center name expected");
                    }
                }
                break;

            case Oem_AfterCenterName:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "REF_FRAME")
                    {
                        m_parserState = Oem_AfterRefFrame;
                        if (kv.value != "EME2000")
                        {
                            raiseError("Unsupported reference frame " + kv.value);
                        }
                    }
                    else
                    {
                        raiseError("Reference frame expected");
                    }
                }
                break;

            case Oem_AfterRefFrame:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "REF_FRAME_EPOCH")
                    {
                    }
                    else
                    {
                        skip = true;
                    }
                    m_parserState = Oem_AfterRefFrameEpoch;
                }
                break;

            case Oem_AfterRefFrameEpoch:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "TIME_SYSTEM")
                    {
                        m_parserState = Oem_AfterTimeSystem;
                        if (kv.value != "TDB")
                        {
                            raiseError("Unsupported time system " + kv.value);
                        }
                    }
                    else
                    {
                        raiseError("Time system expected");
                    }
                }
                break;

            case Oem_AfterTimeSystem:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "START_TIME")
                    {
                        m_parserState = Oem_AfterStartTime;
                    }
                    else
                    {
                        raiseError("Start time expected");
                    }
                }
                break;

            case Oem_AfterStartTime:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "USEABLE_START_TIME")
                    {
                    }
                    else
                    {
                        skip = true;
                    }
                    m_parserState = Oem_AfterUseableStartTime;
                }
                break;

            case Oem_AfterUseableStartTime:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "USEABLE_STOP_TIME")
                    {
                    }
                    else
                    {
                        skip = true;
                    }
                    m_parserState = Oem_AfterUseableStopTime;
                }
                break;

            case Oem_AfterUseableStopTime:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "STOP_TIME")
                    {
                        m_parserState = Oem_AfterStopTime;
                    }
                    else
                    {
                        raiseError("Stop time expected");
                    }
                }
                break;

            case Oem_AfterStopTime:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "INTERPOLATION")
                    {
                    }
                    else
                    {
                        skip = true;
                    }
                    m_parserState = Oem_AfterInterpolation;
                }
                break;

            case Oem_AfterInterpolation:
                {
                    OemKeyValue kv = ParseKeyValue(line);
                    if (kv.key == "INTERPOLATION_DEGREE")
                    {
                    }
                    else
                    {
                        skip = true;
                    }
                    m_parserState = Oem_AfterInterpolationDegree;
                }
                break;

            case Oem_AfterInterpolationDegree:
                if (line == "META_STOP")
                {
                    m_parserState = Oem_AfterMetaStop;
                    trajectory = new ScenarioExternalType();

                    trajectory->CentralBody()->setName(center->name());
                    sc->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(trajectory));
                }
                else
                {
                    raiseError("End of metadata (META_STOP) expected");
                }
                break;

            case Oem_AfterMetaStop:
                if (line.startsWith("COMMENT"))
                {
                    // Ignore the comment
                }
                else
                {
                    skip = true;
                    m_parserState = Oem_EphemerisData;
                }
                break;

            case Oem_EphemerisData:
                if (line == "META_START")
                {
                    m_parserState = Oem_MetaStart;
                }
                else
                {
                    // Parse the ephemeris line
                }
                break;
            }
        }
    }

    if (m_parserState == Oem_Error)
    {
        delete scenario;
        scenario = NULL;
    }
    else
    {
        scenario->setName(objectName);
        sc->setName(objectName);
    }

    return scenario;
}


void
OemImporter::readHeader()
{
}


void
OemImporter::raiseError(const QString& errorMessage)
{
    m_errorMessage = errorMessage;
    m_parserState = Oem_Error;
}
