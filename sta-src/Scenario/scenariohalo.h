
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#ifndef SCENARIOHALO_H
#define SCENARIOHALO_H

#include "scenarioobject.h"
#include "scenariomanifold.h"


class ScenarioInitialStatePosition;
class ScenarioAmplitudes;
class ScenarioManifolds;

class ScenarioHalo : public ScenarioObject
{
public:
    ScenarioHalo();
    virtual ~ScenarioHalo();

    QString elementName() const { return "HaloOrbit"; }
    QString displayName() const { return QObject::tr("Halo Orbit"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    QString Lpoint() const {return m_lpoint;}
    void setLpoint (QString lpoint) {m_lpoint = lpoint;}
    ScenarioInitialStatePosition* initialStatePosition() const { return m_initialStatePosition; }
    void setInitialStatePosition(ScenarioInitialStatePosition* initialStatePosition);
    ScenarioAmplitudes* amplitudes() const { return m_amplitudes; }
    void setAmplitudes(ScenarioAmplitudes* amplitudes);
    ScenarioManifolds* manifolds() const { return m_manifolds; }
    void setManifolds(ScenarioManifolds* manifolds);

private:
    QString m_lpoint;
    ScenarioInitialStatePosition* m_initialStatePosition;
    ScenarioAmplitudes* m_amplitudes;
    ScenarioManifolds* m_manifolds;
    //ScenarioManifolds* m_rightUnstable;
    //ScenarioManifolds* m_leftStable;
    //ScenarioManifolds* m_leftUnstable;

};


class ScenarioAmplitudes: public ScenarioObject
{
public:
    ScenarioAmplitudes();
    virtual ~ScenarioAmplitudes(){}

    QString elementName() const { return "Amplitudes"; }
    QString displayName() const { return QObject::tr("Amplitudes"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

     /*! Return the x-amplitude in km. */
    double Ax() const { return m_ax; }
    /*! Set the x-amplitude in km */
    void setAx(double kilometers) { m_ax = kilometers; }

     /*! Return the z-amplitude in km. */
    double Az() const { return m_az; }
    /*! Set the z-amplitude in km */
    void setAz(double kilometers) { m_az = kilometers; }

private:
    double m_ax;
    double m_az;

};



#endif // SCENARIOHALO_H
