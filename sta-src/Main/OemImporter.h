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

#ifndef _STA_OEM_IMPORTER_H_
#define _STA_OEM_IMPORTER_H_

#include "Scenario/scenario.h"
#include <QTextStream>

/** OemImporter loads an OEM (Orbit Ephemeris Message) from a stream. The syntax
  * of an OEM file is given in the document "CCSDS Recommended Standard for Orbit
  * Data Messages", CCSDS 502.0-B-2.
  */
class OemImporter
{
public:
    OemImporter(QTextStream* stream);

    /** Get the human-readable error message. The error message is available
      * after calling loadScenario(). If the scenario was loaded successfully,
      * the error message will be empty.
      */
    QString errorMessage() const
    {
        return m_errorMessage;
    }

    /** Get the current line number. If there was an error while parsing the
      * OEM file, lineNumber returns the line on which the first error was
      * found.
      */
    unsigned int lineNumber() const
    {
        return m_lineNumber;
    }

    SpaceScenario* loadScenario();

private:
    enum OemParserState
    {
        Oem_Begin,
        Oem_AfterVersion,
        Oem_AfterCreationDate,
        Oem_AfterHeader,
        Oem_MetaStart,
        Oem_AfterObjectName,
        Oem_AfterObjectId,
        Oem_AfterCenterName,
        Oem_AfterRefFrame,
        Oem_AfterRefFrameEpoch,
        Oem_AfterTimeSystem,
        Oem_AfterStartTime,
        Oem_AfterUseableStartTime,
        Oem_AfterUseableStopTime,
        Oem_AfterStopTime,
        Oem_AfterInterpolation,
        Oem_AfterInterpolationDegree,
        Oem_AfterMetaStop,
        Oem_EphemerisData,
        Oem_End,
        Oem_Error,
    };

    void readHeader();
    void raiseError(const QString& errorMessage);

private:
    QTextStream* m_stream;
    unsigned int m_lineNumber;
    QString m_errorMessage;
    OemParserState m_parserState;
};

#endif // _STA_OEM_IMPORTER_H_
