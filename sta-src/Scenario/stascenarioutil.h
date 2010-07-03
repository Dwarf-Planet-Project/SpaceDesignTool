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

// Author: G. Ortega


#ifndef _STA_SCENARIO_UTIL_H_
#define _STA_SCENARIO_UTIL_H_

#include <QString>
#include <QDateTime>
#include <QList>
#include <QDomElement>

extern bool parseBoolean(const QString& e);
extern double parseDouble(const QString& e);
extern QString parseString(const QString& e);
extern int parseInt(const QString& e);
extern QDateTime parseDateTime(const QString& e);
extern QList<double> parseDoubleList(const QString& e);
extern QList<QString> parseStringList(const QString& e);
extern QList<int> parseIntList(const QString& e);

extern QString convertToString(const QString& e);
extern QString convertToString(double d);
extern QString convertToString(int n);
extern QString convertToString(bool b);
extern QString convertToString(const QDateTime& d);
extern QString convertToString(const QList<double>& l);
extern QString convertToString(const QList<int>& l);
extern QString convertToString(const QList<QString>& l);

// Create a new element with a single text node as a child. The text node
// contains the string representation of the value.
template<typename T>
QDomElement createSimpleElement(QDomDocument& doc, const QString& tagName, T value)
{
    QDomElement child = doc.createElement(tagName);
    child.appendChild(doc.createTextNode(convertToString(value)));
    return child;
}

/** Add require namespace attributes for the root SpaceScenario element */
void AddScenarioNamespaceAttributes(QDomElement& scenarioElement);

#endif // _STA_SCENARIO_UTIL_H_
