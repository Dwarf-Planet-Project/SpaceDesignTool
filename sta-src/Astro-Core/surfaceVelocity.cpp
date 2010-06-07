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
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */

#include "surfaceVelocity.h"
#include "stabody.h"

surfaceVelocity::surfaceVelocity()
{
}

double surfaceVelocity::earthStationLinearVelocity(double latitude){

    const double earthRadius=STA_SOLAR_SYSTEM->lookup("Earth")->meanRadius();
    const double earthAngularSpeed=7.2921159E-5;

    double surfaceLinearVelocity=(earthRadius*earthAngularSpeed)*cos(latitude);

    return surfaceLinearVelocity;
}