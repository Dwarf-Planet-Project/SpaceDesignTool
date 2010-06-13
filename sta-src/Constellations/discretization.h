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

// Authors: Class and Steffen

/*

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
