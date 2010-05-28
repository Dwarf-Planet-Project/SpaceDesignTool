/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/


//------------------ Author:       Chris Laurel                   -------------------
//-----------------------------------------------------------------------------------

#include <QWidget>
#include <QVector>


class PlotDataSource;

class PlotView : public QWidget
{
    Q_OBJECT
public:
    PlotView(QWidget* parent);
    ~PlotView();

    void addPlot(PlotDataSource* plotData);
    void removePlot(PlotDataSource* plotData);
    void removeAllPlots();

    QString title() const
    {
        return m_title;
    }

public slots:
    void setTitle(const QString& title);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    QVector<PlotDataSource*> m_plots;
    QString m_title;
};
