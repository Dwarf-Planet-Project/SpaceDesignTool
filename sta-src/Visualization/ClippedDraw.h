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

/*
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _STA_VISUALIZATION_CLIPPED_DRAW_H_
#define _STA_VISUALIZATION_CLIPPED_DRAW_H_

#include <Eigen/Geometry>
#include <QVector>
#include <QPointF>

// The following functions are used by the ground track view to draw geometry
// that is clipped to the edge of the view.

void ClippedLine(const Eigen::AlignedBox<float, 3>& clipBox,
                 float long0, float lat0,
                 float long1, float lat1,
                 QVector<QPointF>& points);

void ClippedLine(const Eigen::AlignedBox<float, 3>& clipBox,
                 const Eigen::Transform3f& projection,
                 float long0, float lat0, float alt0,
                 float long1, float lat1, float alt1,
                 QVector<QPointF>& points);

void ClippedWrappedLine(const Eigen::AlignedBox<float, 3>& clipBox,
                        float long0, float lat0,
                        float long1, float lat1,
                        QVector<QPointF>& points);


#endif // _STA_VISUALIZATION_CLIPPED_DRAW_H_
