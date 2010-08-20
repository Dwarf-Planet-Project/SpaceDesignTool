/*
 * $Revision: 447 $ $Date: 2010-08-20 15:36:47 -0700 (Fri, 20 Aug 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_CHRONOLOGY_H_
#define _VESTA_CHRONOLOGY_H_

#include "Object.h"
#include <vector>


namespace vesta
{

class Arc;

class Chronology : public Object
{
public:
    Chronology();
    ~Chronology();

    double beginning() const
    {
        return m_beginning;
    }

    void setBeginning(double t);

    double ending() const
    {
        return m_beginning + m_duration;
    }

    double duration() const
    {
        return m_duration;
    }

    Arc* activeArc(double t) const;
    Arc* firstArc() const;
    Arc* lastArc() const;
    Arc* arc(unsigned int index) const;
    unsigned int arcCount() const;

    void addArc(Arc* arc);

    /** Return true if this chronology contains any arcs, false if it does
      * not.
      */
    bool empty() const
    {
        return m_arcSequence.empty();
    }

    /** Return true if is between the beginning and ending times of this
      * chronology. The test is beginning <= t <= ending.
      */
    bool includesTime(double t) const
    {
        return !m_arcSequence.empty() && t >= beginning() && t <= ending();
    }

private:
    std::vector<counted_ptr<Arc> > m_arcSequence;
    double m_beginning;
    double m_duration;
};

} // namespace

#endif // _VESTA_ARC_H_
