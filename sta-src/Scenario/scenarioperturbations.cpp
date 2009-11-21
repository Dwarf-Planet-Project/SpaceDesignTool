/*
 File name: scenarioperturbations.cpp
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

#include "scenarioperturbations.h"
#include "scenariobody.h"
#include <QTreeWidgetItem>

ScenarioPerturbations::ScenarioPerturbations()
{
}


ScenarioPerturbations::~ScenarioPerturbations()
{
}

QTreeWidgetItem*
ScenarioPerturbations::createItemSelf(QTreeWidgetItem* parent, QString /* itemName */)
{
    QTreeWidgetItem* item = ScenarioObject::createItemSelf(parent);
    item->setText(1, name());

    return item;
}

void
ScenarioPerturbations::writeElement(QXmlStreamWriter* out)
{
    out->writeStartElement(elementName());
    writeContents(out);
    out->writeEndElement();
}

ScenarioGravityPerturbations::ScenarioGravityPerturbations() :
    m_modelName("Default"),
    m_zonalCount(0),
    m_tesseralCount(0),
    m_body(NULL)
{
}

void
ScenarioGravityPerturbations::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "ModelName", m_modelName);
    writeStringElement(out, "NumberOfZonals", QString::number(m_zonalCount));
    writeStringElement(out, "NumberOfTesserals", QString::number(m_tesseralCount));
}

void
ScenarioGravityPerturbations::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* modelNameItem = new QTreeWidgetItem(item);
    modelNameItem->setText(0, QObject::tr("Gravity Model"));
    modelNameItem->setText(1, m_modelName);
    QTreeWidgetItem* zonalItem = new QTreeWidgetItem(item);
    zonalItem->setText(0, QObject::tr("Zonals"));
    zonalItem->setText(1, QString::number(m_zonalCount));
    QTreeWidgetItem* tesseralItem = new QTreeWidgetItem(item);
    tesseralItem->setText(0, QObject::tr("Tesserals"));
    tesseralItem->setText(1, QString::number(m_tesseralCount));
}

ScenarioAtmosphericDragPerturbations::ScenarioAtmosphericDragPerturbations():
        m_atmosphericModel(NULL),
        m_body(NULL)
{
}

void
ScenarioAtmosphericDragPerturbations::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "ModelName", m_atmosphericModel);
}

void
ScenarioAtmosphericDragPerturbations::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* atmosphericModelItem = new QTreeWidgetItem(item);
    atmosphericModelItem->setText(0, QObject::tr("Atmospheric Model"));
    atmosphericModelItem->setText(1, m_atmosphericModel);
}

ScenarioSolarPressurePerturbations::ScenarioSolarPressurePerturbations() :
    m_reflectivity(0.0),
    m_albedo(0),
    m_ir(0),
    m_body(NULL)
{
}

void
ScenarioSolarPressurePerturbations::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "Reflectivity", QString::number(m_reflectivity));
    writeBooleanElement(out, "Albedo", m_albedo);
    writeBooleanElement(out, "Ir", m_ir);
}

void
ScenarioSolarPressurePerturbations::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* reflectivityItem = new QTreeWidgetItem(item);
    reflectivityItem->setText(0, QObject::tr("Reflectivity"));
    reflectivityItem->setText(1, QString::number(m_reflectivity));
    if (albedo() == true)
    {
        QTreeWidgetItem* albedoItem = new QTreeWidgetItem(item);
        albedoItem->setText(0, QObject::tr("Albedo"));
    }
    if (ir() == true)
    {
        QTreeWidgetItem* irItem = new QTreeWidgetItem(item);
        irItem->setText(0, QObject::tr("IR"));
    }
}

ScenarioExternalBodyPerturbations::ScenarioExternalBodyPerturbations() :
    m_body(NULL)
{
    QList<ScenarioBody*> perturbingBodyList;
    m_perturbingBodyList = perturbingBodyList;
}

ScenarioExternalBodyPerturbations::~ScenarioExternalBodyPerturbations()
{
    foreach (ScenarioBody* body, m_perturbingBodyList)
    {
        safeRelease(body);
    }
}

void
ScenarioExternalBodyPerturbations::addPerturbingBody(ScenarioBody* body)
{
    m_perturbingBodyList.append(body);
    attachChild(body);
}

void
ScenarioExternalBodyPerturbations::removePerturbingBody(ScenarioBody* body)
{
    m_perturbingBodyList.removeOne(body);
    detachChild(body);
}


void
ScenarioExternalBodyPerturbations::writeContents(QXmlStreamWriter* out)
{
    foreach (ScenarioBody* body, m_perturbingBodyList)
    {
        body->writeElement(out, "PerturbingBody");
    }
}

void
ScenarioExternalBodyPerturbations::createItemContents(QTreeWidgetItem* item)
{
    foreach (ScenarioBody* body, m_perturbingBodyList)
    {
        body->createItem(item, "Perturbing Body");
    }
}


ScenarioDebrisPerturbations::ScenarioDebrisPerturbations() :
    m_body(NULL)
{
}

void
ScenarioDebrisPerturbations::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "yes", "yes");
}

void
ScenarioDebrisPerturbations::createItemContents(QTreeWidgetItem* item)
{
}
