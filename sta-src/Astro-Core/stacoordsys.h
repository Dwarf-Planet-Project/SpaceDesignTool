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
 //Modified by Valentino Zuccarelli on 11th June 2009
 */

#ifndef _ASTROCORE_STACOORDSYS_H_
#define _ASTROCORE_STACOORDSYS_H_

#include "statevector.h"
#include <QHash>
#include <cmath>

#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/QR>
#include <Eigen/SVD>
#include <Eigen/Geometry>
#include <Eigen/LeastSquares>
#include <Eigen/Sparse>

// import most common Eigen types
USING_PART_OF_NAMESPACE_EIGEN

class StaBody;

namespace sta
{
    enum CoordinateSystemType
    {
        COORDSYS_ROT,             /*! Rotating r.f.*/
        COORDSYS_ROT_NORM,        /*! Rotating normalized r.f.*/
        COORDSYS_EME_J2000,       /*! Inertial, Earth mean equator of J2000 */
        COORDSYS_EME_B1950,       /*! Inertial, Earth mean equator of B1950 */
        COORDSYS_ECLIPTIC_J2000,  /*! Ecliptic, J2000 */
        COORDSYS_BODYFIXED,
        COORDSYS_INVALID,
    };

    class CoordinateSystem
    {
    public:
        /*! Create a default J2000 inertial coordinate system. */
        CoordinateSystem() : m_type(COORDSYS_EME_J2000) {}

        /*! Create a coordinate system of the specified type. */
        CoordinateSystem(CoordinateSystemType type) : m_type(type) {}

        CoordinateSystem(QString name);

        bool operator==(const CoordinateSystem& other) const
        {
            return other.m_type == m_type;
        }

        bool valid() const
        {
            return m_type != COORDSYS_INVALID;
        }

        bool isInertial() const
        {
            return m_type == COORDSYS_EME_J2000 ||
                   m_type == COORDSYS_EME_B1950 ||
                   m_type == COORDSYS_ECLIPTIC_J2000;
        }
    
        /*! Return the coordinate system type. */
        CoordinateSystemType type() const { return m_type; }

        QString name() const;

        StateVector toEmeJ2000(const StateVector& v) const;
        StateVector fromEmeJ2000(const StateVector& v) const;

        StateVector toEmeJ2000(const StateVector& v, const StaBody* center, double mjd) const;
        StateVector fromEmeJ2000(const StateVector& v, const StaBody* center, double mjd) const;

        static StateVector convert(const StateVector& state,
                                   double mjd,
                                   const StaBody* fromCenter,
                                   const CoordinateSystem& fromSys,
                                   const StaBody* toCenter,
                                   const CoordinateSystem& toSys);
        Matrix3d rotToEmeJ2000();
                Matrix3d rotFromEmeJ2000();

                Matrix3d rotToEmeJ2000(const StaBody* center, double mjd);
                Matrix3d rotFromEmeJ2000(const StaBody* center, double mjd);

                Vector3d omegaToEmeJ2000(const StaBody* center, double mjd);
                Vector3d omegaFromEmeJ2000(const StaBody* center, double mjd);

    private:
        CoordinateSystemType m_type;
    };
};

#endif // _ASTROCORE_STACOORDSYS_H_
