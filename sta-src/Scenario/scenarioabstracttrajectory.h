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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Modified by Guillermo as to include trajectories already propagated; October 2009
 */

#ifndef _STA_SCENARIO_ABSTRACT_TRAJECTORY_H_
#define _STA_SCENARIO_ABSTRACT_TRAJECTORY_H_

#include "scenarioobject.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stacoordsys.h"

class PropagationFeedback;
class VisualProperties;


class ScenarioAbstractTrajectory : public ScenarioObject
{
public:
    ScenarioAbstractTrajectory();
    ~ScenarioAbstractTrajectory();

    virtual sta::StateVector computeInitialStateVector() const = 0;

    /*! The propagate method is the workhorse for trajectories. It must be
     *  implemented by all Trajectory subclasses. Propagation begins with
     *  the specified initial state vector. Implementations must fill the
     *  lists of sampleTimes and samples with the results of each propagation
     *  step. Errors during propagation should be indicated by setting the
     *  error flag in the PropagationFeedback structure. Feedback should also
     *  be tested at each step for cancellation by the user (though this
     *  mechanism for user interruption will eventually be replaced by a
     *  multithreaded architecture.)
     *
     *  The return value of propagate is the final state after propagation
     *  is complete (i.e. the last entry in the samples list.)
     */
    virtual sta::StateVector propagate(PropagationFeedback& feedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples) = 0;

    /*
    virtual sta::StateVector propagate(PropagationFeedback& feedback,
                                       const QString FileNameOfPropagatedTrajectory,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples) = 0;
    */

    virtual const StaBody* centralBody() const;
    virtual const StaBody* firstBody() const;
    virtual const StaBody* secondBody() const;
    virtual const sta::CoordinateSystem coordinateSystem() const;
};

#endif // _STA_SCENARIO_ABSTRACT_TRAJECTORY_H_


