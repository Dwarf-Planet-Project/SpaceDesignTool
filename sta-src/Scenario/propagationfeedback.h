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

#ifndef _STA_SCENARIO_PROPAGATIONFEEDBACK_H_
#define _STA_SCENARIO_PROPAGATIONFEEDBACK_H_


/*! An instance of the propagation feedback class is supplied when
 *  propagating a trajectory. It is used to report progress and possibly
 *  errors during a propagation.
 */
class PropagationFeedback
{
public:
    enum Status
    {
        PropagationOk,
        PropagationCanceled,
        PropagationError,
    };
    
    PropagationFeedback() : m_status(PropagationOk) {}
    virtual ~PropagationFeedback() {}
    
    Status status() const { return m_status; }
    QString errorString() const { return m_errorString; }
    bool ok() const { return m_status == PropagationOk; }
    
    void raiseError(const QString& errorString)
    {
        m_errorString = errorString;
        m_status = PropagationError;
    }
    
private:
    Status m_status;
    QString m_errorString;
};

#endif // _STA_SCENARIO_PROPAGATIONFEEDBACK_H_
