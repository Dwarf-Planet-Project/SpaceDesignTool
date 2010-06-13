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

#include "GravityModel.h"

using namespace sta;
using namespace Eigen;


GravityModel::GravityModel(double mu) :
    m_mu(mu)
{
}


GravityModel::GravityModel(double mu, const VectorXd& zonals) :
    m_mu(mu),
    m_zonalHarmonics(zonals)
{
}


GravityModel::~GravityModel()
{
}


void
GravityModel::setZonals(const Eigen::VectorXd& zonals)
{
    m_zonalHarmonics = zonals;
}

