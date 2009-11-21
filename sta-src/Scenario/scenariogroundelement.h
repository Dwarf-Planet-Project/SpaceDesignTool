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
  // Patched by Guillermo June 14th 2009

 */

#ifndef _STA_SCENARIO_GROUNDELEMENT_H_
#define _STA_SCENARIO_GROUNDELEMENT_H_

#include "scenarioparticipant.h"

class ScenarioLocation;

/*! Abtract base class of all scennario ground element participants.
 */
class ScenarioGroundElement : public ScenarioParticipant
{
public:
    ScenarioGroundElement();
    virtual ~ScenarioGroundElement();
    
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioLocation* location() const { return m_location; }
    void setLocation(ScenarioLocation* location);
        
private:
    ScenarioLocation* m_location;
};


// TODO: Discuss whether the various ground element subclasses should
// be placed in their own files. It makes sense for consistency, but the
// classes are simple enough now that there's little practical benefit.

/*! Ground station participant.
 */
class ScenarioGroundStation : public ScenarioGroundElement
{
public:
    ScenarioGroundStation() {}
    virtual ~ScenarioGroundStation() {}
    
    QString elementName() const { return "GroundStation"; }
    QString displayName() const { return QObject::tr("Ground Station"); }
};


/*! Launch pad participant.
 */
class ScenarioLaunchPad : public ScenarioGroundElement
{
public:
    ScenarioLaunchPad() : m_clearingAltitude(0.0) {}
    virtual ~ScenarioLaunchPad() {}

    QString elementName() const { return "LaunchPad"; }
    QString displayName() const { return QObject::tr("Launch Pad"); }

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    double clearingAltitude() const { return m_clearingAltitude; }
    void setClearingAltitude(double clearingAltitude) { m_clearingAltitude = clearingAltitude; }

private:
    double m_clearingAltitude;
};


/*! Target participant.
 */
class ScenarioTarget : public ScenarioGroundElement
{
public:
    ScenarioTarget() {}
    virtual ~ScenarioTarget() {}
    
    //QString elementName() const { return "Target"; }
    //QString displayName() const { return QObject::tr("Target"); }
    QString elementName() const { return "Point"; }
    QString displayName() const { return QObject::tr("Point"); }
};

#endif // _STA_SCENARIO_GROUNDELEMENT_H_
