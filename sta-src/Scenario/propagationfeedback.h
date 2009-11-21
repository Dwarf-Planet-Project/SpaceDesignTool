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
