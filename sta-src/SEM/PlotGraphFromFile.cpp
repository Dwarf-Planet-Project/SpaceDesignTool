#include "PlotGraphFromFile.h"

#include <QFile>
#include <QTextStream>

#include <QDebug>


PlotGraphFromFile::PlotGraphFromFile()
{
}


unsigned int PlotGraphFromFile::getPointCount() const
{
    return m_points.size();
}

Eigen::Vector2d PlotGraphFromFile::getPoint(unsigned int index) const
{
    return m_points[index];
}

void PlotGraphFromFile::setPoints(QString FileNameWithExtension)
{
    // first column is the x-axis
    // second column is the y-axis

    QString path = "data/SystemsEngineeringReports/";
    path.append(FileNameWithExtension);

    QFile TimePlot(path);
    TimePlot.open(QIODevice::ReadOnly);
    qDebug()<<"TimePlot.fileName()"<<TimePlot.fileName();
    qDebug()<<"TimePlot.isOpen()"<<TimePlot.isOpen();
    QTextStream TimePlotStream(&TimePlot);

    m_points.clear();

    while (TimePlotStream.status() == QTextStream::Ok)
    {
        Vector2d p;
        TimePlotStream >> p.x();
        TimePlotStream >> p.y();
        qDebug() << p.x() << ", " << p.y();

        if (TimePlotStream.status() == QTextStream::Ok)
        {
            m_points << p;
        }
    }
}
