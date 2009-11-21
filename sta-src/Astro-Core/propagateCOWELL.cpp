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
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/

/*
 ------------------ Author:       Tiziana Sabatini               -------------------
 ------------------ E-mail:       tiziana.sabatini@yahoo.it      -------------------
 -----------------------------------------------------------------------------------
 */


#include "orbitalTOcartesian.h"
#include "propagateCOWELL.h"
#include "perturbations.h"
#include "EODE/eode.h"
//#include <QDebug>

using namespace Eigen;

void derivCOWELL (VectorXd state, double time, VectorXd parameters, VectorXd& derivative)
{
    derivative(0) = state(3);
    derivative(1) = state(4);
    derivative(2) = state(5);
    derivative(3) = parameters(0) + parameters(3);
    derivative(4) = parameters(1) + parameters(4);
    derivative(5) = parameters(2) + parameters(5);
}

class CowellDerivativeCalculator : public DerivativeCalculator<6>
{
public:
    CowellDerivativeCalculator(double centralBodyMu, const QList<Perturbations*>& perturbationList, double deltat) :
        m_centralBodyMu(centralBodyMu),
        m_perturbationList(perturbationList),
        m_deltat(deltat)
    {
    }

    virtual void compute(const State& state, double t, State& derivatives) const
    {
        Vector3d position = state.segment<3>(0);
        Vector3d velocity = state.segment<3>(3);

        // Calculate acceleration from the central body
        double r = position.norm();
        Vector3d centralBodyAcceleration = -pow(r, -3.0) * m_centralBodyMu * position;

        // Calculating the perturbing accelerations
        Vector3d perturbingAcceleration = Vector3d::Zero();
        foreach (Perturbations* perturbation, m_perturbationList)
        {
            perturbingAcceleration += perturbation->calculateAcceleration(sta::StateVector(position, velocity), t, m_deltat);
        }

        derivatives.segment<3>(0) = velocity;
        derivatives.segment<3>(3) = centralBodyAcceleration + perturbingAcceleration;
    }

private:
    double m_centralBodyMu;
    const QList<Perturbations*>& m_perturbationList;
    double m_deltat;
};


sta::StateVector
propagateCOWELL(double mu,
                const sta::StateVector& initialState,
                double deltat,
                const QList<Perturbations*>& perturbations,
                double time,
                const QString& integrator,
                PropagationFeedback& propFeedback)
{
    CowellDerivativeCalculator derivativeCalculator(mu, perturbations, deltat);

    double r = initialState.position.norm();
    if (r <= 0)
    {
        propFeedback.raiseError(QObject::tr("Spacecraft reached planet's surface!"));
    }

    Matrix<double, 6, 1> statevector;
    statevector << initialState.position, initialState.velocity;

    rk4(statevector, time, deltat, &derivativeCalculator);

    return sta::StateVector(statevector.segment<3>(0), statevector.segment<3>(3));
}
