#ifndef PLOTGRAPHFROMFILE_H
#define PLOTGRAPHFROMFILE_H

#include <QObject>
#include "Eigen/Core"
#include "Plotting/PlotDataSource.h"
#include <QString>

using namespace Eigen;

class PlotGraphFromFile: public PlotDataSource
{
    Q_OBJECT
public:
    /*when an Eigen object is the member of a class.
    But the workaround is easy: just put
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW in the definition
    of any class that uses Eigen objects*/
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    PlotGraphFromFile();

    virtual ~PlotGraphFromFile() {};
    virtual unsigned int getPointCount() const;
    virtual Eigen::Vector2d getPoint(unsigned int index) const;

    void setPoints(QString FileNameWithExtension);

protected:
    QList<Eigen::Vector2d> m_points;

};

#endif // PLOTGRAPHFROMFILE_H
