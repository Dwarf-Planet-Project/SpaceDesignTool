/*
 File name: scenarioperturbations.h
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
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#ifndef SCENARIOPERTURBATIONS_H
#define SCENARIOPERTURBATIONS_H

#include "scenarioobject.h"
#include "scenariobody.h"
#include <QList>


/**
 * Abstract base class of all scenario perturbations.
 */
class ScenarioPerturbations : public ScenarioObject
{
public:
    ScenarioPerturbations();
    virtual ~ScenarioPerturbations();

    virtual void writeElement(QXmlStreamWriter* out);
    virtual void writeContents(QXmlStreamWriter* out) = 0;
    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent, QString itemName);

    QString name() const { return m_name; }

private:
    QString m_name;

};

/**
 * Class for scenario perturbations due to the planet's gravity field.
 */
class ScenarioGravityPerturbations : public ScenarioPerturbations
{
public:
    ScenarioGravityPerturbations();
    virtual ~ScenarioGravityPerturbations(){}

    QString elementName() const { return "GravityFieldPerturbation"; }
    QString displayName() const { return QObject::tr("Gravity Field Perturbation"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the central celestial body. */
    const StaBody* centralBody() const { return m_body; }
    /*! Set the central celestial body. */
    const void setCentralBody(const StaBody* body) { m_body = body; }
    QString modelName() const { return m_modelName; }
    void setModelName(QString modelName) { m_modelName = modelName; }
    /*! Return the number of zonals. */
    unsigned int zonalCount() const { return m_zonalCount; }
    /*! Set the number of zonals. */
    void setZonalCount(unsigned int zonalCount) { m_zonalCount = zonalCount; }
    /*! Return the number of tesserals. */
    unsigned int tesseralCount() const { return m_tesseralCount; }
    /*! Set the number of tesserals. */
    void setTesseralCount(unsigned int tesseralCount) { m_tesseralCount = tesseralCount; }

private:
    const StaBody* m_body;
    QString m_modelName;
    unsigned int m_zonalCount;
    unsigned int m_tesseralCount;
};

/**
 * Class for scenario perturbations due to the planet's atmospheric drag.
 */
class ScenarioAtmosphericDragPerturbations : public ScenarioPerturbations
{
public:
    ScenarioAtmosphericDragPerturbations();
    virtual ~ScenarioAtmosphericDragPerturbations() {}

    /*! Return the string with the atmospheric model. */
    QString atmosphericModel() const { return m_atmosphericModel; }
    /*! Set the string with the atmospheric model. */
    void setAtmosphericModel(QString atmosphericModel) { m_atmosphericModel = atmosphericModel; }
    /*! Return the central celestial body. */
    const StaBody* centralBody() const { return m_body; }
    /*! Set the central celestial body. */
    const void setCentralBody(const StaBody* body) { m_body = body; }
    QString elementName() const { return "AtmosphericDragPerturbation"; }
    QString displayName() const { return QObject::tr("Atmospheric Drag Perturbation"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

private:
    QString m_atmosphericModel;
    const StaBody* m_body;

};

/**
 * Class for scenario perturbations due to the solar pressure.
 */
class ScenarioSolarPressurePerturbations : public ScenarioPerturbations
{
public:
    ScenarioSolarPressurePerturbations();
    virtual ~ScenarioSolarPressurePerturbations() {}

    /*! Return the central celestial body. */
    const StaBody* centralBody() const { return m_body; }
    /*! Set the central celestial body. */
    const void setCentralBody(const StaBody* body) { m_body = body; }
    /*! Return the reflectivity of the body. */
    double reflectivity() const { return m_reflectivity; }
    /*! Set the reflectivity of the body. */
    void setReflectivity(double reflectivity) { m_reflectivity = reflectivity; }
    /*! Return true if the albedo radiation is selected. */
    bool albedo() const { return m_albedo; }
    /*! Set the albedo radiation option. */
    void setAlbedo(bool albedo) { m_albedo = albedo; }
    /*! Return true if the infra-red radiation is selected. */
    bool ir() const { return m_ir; }
    /*! Set the infra-red radiation option. */
    void setIr(bool ir) { m_ir = ir; }
    QString elementName() const { return "SolarPressurePerturbation"; }
    QString displayName() const { return QObject::tr("Solar Pressure Perturbation"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

private:
    double m_reflectivity;
    bool m_albedo;
    bool m_ir;
    const StaBody* m_body;
};

/**
 * Class for scenario perturbations due to external bodies.
 */
class ScenarioExternalBodyPerturbations : public ScenarioPerturbations
{
public:
    ScenarioExternalBodyPerturbations();
    virtual ~ScenarioExternalBodyPerturbations();
    QString elementName() const { return "ExternalBodiesPerturbation"; }
    QString displayName() const { return QObject::tr("External Bodies Perturbation"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the central celestial body. */
    const StaBody* centralBody() const { return m_body; }
    /*! Set the central celestial body. */
    const void setCentralBody(const StaBody* body) { m_body = body; }
    /*! Return the list of perturbing bodies. */
    const QList<ScenarioBody*>& perturbingBodyList() const {return m_perturbingBodyList;}
    /*! Add a new celestial body to the list of perturbing bodies. */
    void addPerturbingBody(ScenarioBody* body);
    /*! Remove a celestial body from the list of perturbing bodies. */
    void removePerturbingBody(ScenarioBody* body);
private:
    QList<ScenarioBody*> m_perturbingBodyList;
    const StaBody* m_body;
};

/**
 * Class for scenario perturbations due to space debris collisions.
 */
class ScenarioDebrisPerturbations : public ScenarioPerturbations
{
public:
    ScenarioDebrisPerturbations();
    virtual ~ScenarioDebrisPerturbations() {}

    QString elementName() const { return "DebrisPerturbation"; }
    QString displayName() const { return QObject::tr("Space Debris Perturbation"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the central celestial body. */
    const StaBody* centralBody() const { return m_body; }
    /*! Set the central celestial body. */
    const void setCentralBody(const StaBody* body) { m_body = body; }

private:
    const StaBody* m_body;
};

#endif // SCENARIOPERTURBATIONS_H
