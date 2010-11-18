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
#include "Astro-Core/date.h"

#include <QStringList>
#include <QRegExp>
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
  * Not all features of the OEM format are supported by the OemImporter
  * class. An oem file that uses an unsupported feature will fail to load;
  * details of the problem are available via the errorMessage() method.
  *
  * The current implementation has the following limitations:
  *   - TIME_STANDARD must be TDB
  *   - REF_FRAME must be EME2000 or ECLIP2000
  *   - CENTER_NAME must be an object with an ephemeris
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
    sta::CoordinateSystemType coordSys = sta::COORDSYS_INVALID;

    QRegExp whiteSpaceRegExp("\\s+");


    // This is the state machine for parsing OEM files. Roughly, the structure
    // of an OEM file is:
    //    Header
    //
    //    Metadata
    //    Ephemeris data
    //    Metadata
    //    Ephemeris data
    //    ...
    //
    // Where the metadata and ephemeris data blocks alternate; any number
    // of metadata/ephemeris data pairs may appear in the file. For full
    // details see Recommended Standard CCSDS 502.0-B-2.
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
                        if (kv.value == "EME2000")
                        {
                            coordSys = sta::COORDSYS_EME_J2000;
                        }
                        else if (kv.value == "ECLIP2000")
                        {
                            coordSys = sta::COORDSYS_ECLIPTIC_J2000;
                        }
                        else
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

                    int trajectoryIndex = sc->SCMission()->TrajectoryPlan()->AbstractTrajectory().size();

                    trajectory->CentralBody()->setName(center->name());
                    trajectory->setCoordinateSystem(sta::CoordinateSystem(coordSys).name());
                    trajectory->ElementIdentifier()->setName(QString("%1 - %2").arg(objectName).arg(trajectoryIndex + 1));
                    trajectory->ElementIdentifier()->setColorName("Yellow");

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
                    QStringList values = line.split(whiteSpaceRegExp);
                    if (values.size() != 7)
                    {
                        raiseError("Bad or unsupported ephemeris data record");
                    }
                    else
                    {
                        QDateTime dateTime;
                        int secondsDecimal = values[0].lastIndexOf('.');
                        if (secondsDecimal > 0)
                        {
                            bool msecOk = false;
                            double sec = values[0].right(values[0].length() - secondsDecimal).toDouble(&msecOk);
                            if (msecOk)
                            {
                                dateTime = QDateTime::fromString(values[0].left(secondsDecimal), Qt::ISODate);
                                dateTime.setTimeSpec(Qt::UTC);
                                int msec = (int) (sec * 1000.0);
                                dateTime = dateTime.addMSecs((int) (sec * 1000.0));
                            }
                        }
                        else
                        {
                            dateTime = QDateTime::fromString(values[0], Qt::ISODate);
                        }

                        if (!dateTime.isValid())
                        {
                            raiseError(QString("Bad time value in ephemeris data (%1)").arg(values[0]));
                        }
                        else
                        {
                            bool stateValid = true;
                            double state[6];
                            for (int i = 0; i < 6; ++i)
                            {
                                bool ok = false;
                                state[i] = values[i + 1].toDouble(&ok);
                                if (!ok)
                                {
                                    stateValid = false;
                                }
                            }

                            if (!stateValid)
                            {
                                raiseError("Non-number in ephemeris data");
                            }
                            else
                            {
                                trajectory->TimeTags().append(dateTime);
                                for (int i = 0; i < 6; ++i)
                                {
                                    trajectory->States() << state[i];
                                }
                            }
                        }
                    }
                }
                break;

            default:
                m_parserState = Oem_Error;
                break;
            }
        }
    }

    // Validate trajectory by making sure that there are no overlapping segments or gaps
    if (sc)
    {
        for (int i = 1; i < sc->SCMission()->TrajectoryPlan()->AbstractTrajectory().size(); ++i)
        {
            ScenarioExternalType* prev = dynamic_cast<ScenarioExternalType*>(sc->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i - 1).data());
            ScenarioExternalType* curr = dynamic_cast<ScenarioExternalType*>(sc->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data());
            if (prev && curr && !prev->TimeTags().isEmpty() && !curr->TimeTags().isEmpty())
            {
                QDateTime prevEnd = prev->TimeTags().last();
                QDateTime currStart = curr->TimeTags().first();
                if (currStart < prevEnd)
                {
                    raiseError(QString("Ephemeris segments %1 and %2 overlap.").arg(i - 1).arg(i));
                }
                else if (currStart > prevEnd)
                {
                    raiseError(QString("Gap between ephemeris segments %1 and %2.").arg(i - 1).arg(i));
                }
            }
        }
    }

    if (m_parserState == Oem_Error)
    {
        qDebug() << "Error while importing OEM file: " << errorMessage();
        delete scenario;
        scenario = NULL;
    }
    else
    {
        scenario->setName(objectName);
        sc->setName(objectName);
        sc->ElementIdentifier()->setName(objectName);
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
