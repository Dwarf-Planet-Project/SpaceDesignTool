#ifndef _STA_SCENARIO_UTIL_H_
#define _STA_SCENARIO_UTIL_H_

#include <QString>
#include <QDateTime>
#include <QList>

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
extern QString convertToString(const QDateTime& d);
extern QString convertToString(const QList<double>& l);
extern QString convertToString(const QList<int>& l);
extern QString convertToString(const QList<QString>& l);

#endif // _STA_SCENARIO_UTIL_H_
