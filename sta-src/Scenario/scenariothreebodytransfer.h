
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
------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#ifndef SCENARIOTHREEBODYTRANSFER_H
#define SCENARIOTHREEBODYTRANSFER_H

#include "scenarioobject.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"

class ScenarioInitialStatePosition;
class ScenarioOptimizationParameters;
class ScenarioParkingOrbit;
class ScenarioHaloOrbit;


class ScenarioThreebodyTransfer : public ScenarioObject
{
public:
    ScenarioThreebodyTransfer();
    virtual ~ScenarioThreebodyTransfer();

    QString elementName() const { return "ThreebodyTransfer"; }
    QString displayName() const { return QObject::tr("Three-body transfer"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioParkingOrbit* parkingOrbit() const { return m_parkingOrbit; }
    void setParkingOrbit(ScenarioParkingOrbit* parkingOrbit);
    ScenarioHaloOrbit* haloOrbit() const { return m_haloOrbit; }
    void setHaloOrbit (ScenarioHaloOrbit* haloOrbit);
    ScenarioOptimizationParameters* optimizationParameters() const { return m_optimizationParameters; }
    void setOptimizationParameters (ScenarioOptimizationParameters* optimizationParameters);
    double startTime;
    void setStartTime(double time) { m_startTime = time; }

private:
    ScenarioParkingOrbit* m_parkingOrbit;
    ScenarioHaloOrbit* m_haloOrbit;
    ScenarioOptimizationParameters* m_optimizationParameters;
    double m_startTime;// End time is stored as a modified Julian date
};


class ScenarioParkingOrbit : public ScenarioObject
{
public:
    ScenarioParkingOrbit();
    virtual ~ScenarioParkingOrbit();

    QString elementName() const { return "ParkingOrbit"; }
    QString displayName() const { return QObject::tr("Parking Orbit"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioInitialStatePosition* pOrbit() const { return m_pOrbit; }
    void setInitialStatePosition(ScenarioInitialStatePosition* pOrbit);

private:
    ScenarioInitialStatePosition* m_pOrbit;
};


class ScenarioHaloOrbit : public ScenarioObject
{
public:
    ScenarioHaloOrbit();
    virtual ~ScenarioHaloOrbit();

    QString elementName() const { return "HaloOrbit"; }
    QString displayName() const { return QObject::tr("Halo Orbit"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioInitialStatePosition* hOrbit() const { return m_hOrbit; }
    void setInitialStatePosition(ScenarioInitialStatePosition* hOrbit);

private:
    ScenarioInitialStatePosition* m_hOrbit;
};


class ScenarioOptimizationParameters : public ScenarioObject
{
public:
    ScenarioOptimizationParameters();
    virtual ~ScenarioOptimizationParameters();

    QString elementName() const { return "OptimizationParameters"; }
    QString displayName() const { return QObject::tr("Optimization Parameters"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    bool timeofFlight() const { return m_timeofFlight; }
    void setTimeofFlight(bool tof) { m_timeofFlight = tof; }
    unsigned int maximumToF() const { return m_maximumToF; }
    void setMaximumToF (unsigned int maxtof) { m_maximumToF = maxtof; }
    bool propellant() const { return m_propellant; }
    void setPropellant(bool propel) { m_propellant = propel; }

private:
    bool m_timeofFlight;
    unsigned int m_maximumToF;
    bool m_propellant;
};

#endif // SCENARIOTHREEBODYTRANSFER_H
