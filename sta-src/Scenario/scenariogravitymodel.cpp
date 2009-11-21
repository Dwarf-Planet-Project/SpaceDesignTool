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

#include <QTreeWidgetItem>
#include "scenariogravitymodel.h"


ScenarioGravityModel::ScenarioGravityModel() :
    m_modelName("Default"),
    m_zonalCount(0),
    m_tesseralCount(0)
{
}


ScenarioGravityModel::~ScenarioGravityModel()
{
}


void
ScenarioGravityModel::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "ModelName", m_modelName);
    writeStringElement(out, "NumberOfZonals", QString::number(m_zonalCount));
    writeStringElement(out, "NumberOfTesserals", QString::number(m_tesseralCount));
}


void
ScenarioGravityModel::createItemContents(QTreeWidgetItem* item)
{
    item->setText(1, m_modelName);
    
    QTreeWidgetItem* zonalItem = new QTreeWidgetItem(item);
    zonalItem->setText(0, QObject::tr("Zonals"));
    zonalItem->setText(1, QString::number(m_zonalCount));
    QTreeWidgetItem* tesseralItem = new QTreeWidgetItem(item);
    tesseralItem->setText(0, QObject::tr("Tesserals"));
    tesseralItem->setText(1, QString::number(m_tesseralCount));
}
