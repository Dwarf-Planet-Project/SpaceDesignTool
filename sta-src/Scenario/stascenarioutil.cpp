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

#include "stascenarioutil.h"
#include <QTextStream>


bool parseBoolean(const QString& s)
{
    QString t = s.trimmed().toLower();
    return (t == "true" || t == "1" || t == "yes");
}


double parseDouble(const QString& s)
{
    return s.toDouble();
}


QString parseString(const QString& s)
{
    return s;
}


int parseInt(const QString& s)
{
    return s.toInt();
}


QDateTime parseDateTime(const QString& s)
{
    QDateTime datetime = QDateTime::fromString(s, Qt::ISODate);
    return datetime;
}


template<typename T> QList<T> parseItemList(const QString& s)
{
    QList<T> list;
    QTextStream in(const_cast<QString*>(&s), QIODevice::ReadOnly);
    bool readOk = true;

    while (readOk)
    {
        T d;
        in >> d;
        if (in.status() == QTextStream::Ok)
        {
            list << d;
        }
        else
        {
            readOk = false;
        }
    }

    return list;
}


QList<double> parseDoubleList(const QString& s)
{
    return parseItemList<double>(s);
}


QList<QString> parseStringList(const QString& s)
{
    return parseItemList<QString>(s);
}


QList<int> parseIntList(const QString& s)
{
    return parseItemList<int>(s);
}


QString convertToString(const QString& s)
{
    return s;
}

QString convertToString(double d)
{
    return QString::number(d, 'g', 16);
}

QString convertToString(int n)
{
    return QString::number(n);
}

QString convertToString(bool b)
{
    return b ? QString("true") : QString("false");
}

QString convertToString(const QDateTime& d)
{
    return d.toString(Qt::ISODate);
}

QString convertToString(const QList<QString>& l)
{
    QString s;
    foreach (QString s, l)
    {
        s += s + " ";
    }

    return s;
}

QString convertToString(const QList<double>& l)
{
    QString s;
    foreach (double d, l)
    {
        s += QString::number(d) + " ";
    }

    return s;
}

QString convertToString(const QList<int>& l)
{
    QString s;
    foreach (int n, l)
    {
        s += QString::number(n) + " ";
    }

    return s;
}


void AddScenarioNamespaceAttributes(QDomElement& scenarioElement)
{
    // TODO: Update these with the actual location of the space scenario schema once that
    // has been established.
    scenarioElement.setAttribute("xmlns:tns", "http://www.example.org/scenario");
    scenarioElement.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    scenarioElement.setAttribute("xsi:schemaLocation", "http://www.example.org/scenario scenario.xsd");
}
