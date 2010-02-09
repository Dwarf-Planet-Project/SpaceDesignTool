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
    return QString::number(d);
}

QString convertToString(int n)
{
    return QString::number(n);
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
