/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Claas Grohnfeldt, Steffen Peter  -------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) --------------------------------
 Modified by...
 */

#ifndef DISCRETIZATION_H
#define DISCRETIZATION_H

#include "Astro-Core/stabody.h"

struct DiscretizationPoint
{
    float longitude;
    float latitude;
    bool *seen;
    float rectangleWidth;
};
class DiscreteMesh
{
    public:
        DiscreteMesh( const StaBody* body, int n);
        ~DiscreteMesh();
        void generateMesh(int numberRows);
        const StaBody* centralBody;
        int numberRows;

        QList<DiscretizationPoint> *discreteMesh;
        QList<DiscretizationPoint> meshAsList;

};

#endif // DISCRETIZATION_H
