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


#include "scenarioobject.h"
#include <QTreeWidget>


ScenarioObject::~ScenarioObject()
{
}


void
ScenarioObject::attach(ScenarioObject* parent)
{
    Q_ASSERT(!isAttached());  // Detach must be called first
    Q_ASSERT(parent != this); // Avoid infinite recursion
    m_parent = parent;
}


void
ScenarioObject::detach()
{
    Q_ASSERT(isAttached());  // Flag multiple detaches
    m_parent = NULL;
}


void
ScenarioObject::attachChild(ScenarioObject* child)
{
    if (child != NULL)
    {
        Q_ASSERT(!child->isAttached());  // Detach must be called first
        Q_ASSERT(child != this);         // Avoid infinite recursion
        child->m_parent = this;
        child->addRef();
    }
}


void
ScenarioObject::detachChild(ScenarioObject* child)
{
    if (child != NULL)
    {
        Q_ASSERT(child->parent() == this);
        child->m_parent = NULL;
        child->release();
    }
}


/*! Write this element to an XML stream. This method is meant to
 *  be overridden by subclasses. The default impelementation writes
 *  an element without attributes and calls writeContents.
 */
void ScenarioObject::writeElement(QXmlStreamWriter* out, QString xmlElementName)
{
    out->writeStartElement(xmlElementName.isEmpty() ? elementName() : xmlElementName);
    writeContents(out);
    out->writeEndElement();
}


/*! Write the contents of this element to an XML stream. The default
 *  implementation does nothing. ScenarioObject subclasses should
 *  override this method.
 */
void ScenarioObject::writeContents(QXmlStreamWriter* /* out */)
{
}


QTreeWidgetItem* ScenarioObject::createItem(QTreeWidgetItem* parent, QString itemName)
{
    QTreeWidgetItem* item = createItemSelf(parent, itemName);
    createItemContents(item);
        
    return item;
}


QTreeWidgetItem* ScenarioObject::createItemSelf(QTreeWidgetItem* parent, QString itemName)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, itemName.isEmpty() ? displayName() : itemName);
    
    // Store a pointer to the this ScenarioObject in the widget item.
    // This will be used to map a tree widget item to a ScearioObject.
    // IMPORTANT: The ScenarioObject must not be deleted before the tree
    // widget item that references it.
    QVariant scenarioObjectData = qVariantFromValue(reinterpret_cast<void*>(this));
    item->setData(0, ScenarioObjectRole, scenarioObjectData);
    
    // Make drag and drop enabled by default. Only very limited drag and drop
    // is allowed within a space scenario because in general the elements
    // of the scenario have a defined order. Only rearranging of participants
    // and trajectories is permitted.
    item->setFlags(item->flags() & ~(Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled));
    
    return item;
}

void ScenarioObject::createItemContents(QTreeWidgetItem* /* item */)
{
}


/*! Remove the child object at the specified index. This method should be
  * overridden by objects that maintain a list of subobjects. Returns
  * true if the object was removed successfully, and false if not (because
  * the index is out of range or the scenario object doesn't have removeable
  * children. The default implementaion always returns false.
  */
bool ScenarioObject::removeChild(unsigned int index)
{
    return false;
}


/*! Output an element with the specified name and numeric value. If the
 *  value units is not an empty string, an attribute called units will also
 *  be written.
 */
void ScenarioObject::writeNumericElement(QXmlStreamWriter* out, QString name, double value, QString units)
{
    out->writeStartElement(name);
    if (!units.isEmpty())
        out->writeAttribute("units", units);
    out->writeCharacters(QString::number(value));
    out->writeEndElement();
}


/*! Output an element with a boolean value. 1 is written if the value is true,
 *  0 if it is false.
 */
void ScenarioObject::writeBooleanElement(QXmlStreamWriter* out, QString name, bool value)
{
    out->writeTextElement(name, value ? "1" : "0");
}


/*! Output an element with the specified name and 3-vector value. If the
 *  value units is not an empty string, an attribute called units will also
 *  be written.
 */
void ScenarioObject::writeVectorElement(QXmlStreamWriter* out, QString name, const Eigen::Vector3d& value, QString units)
{
    out->writeStartElement(name);
    if (!units.isEmpty())
        out->writeAttribute("units", units);
    out->writeCharacters(QString("%1 %2 %3").arg(value.x()).arg(value.y()).arg(value.z()));
    out->writeEndElement();
}

/*! Output an element with the specified name and text.
 */
void ScenarioObject::writeStringElement(QXmlStreamWriter* out, QString name, QString text)
{
    out->writeStartElement(name);
    out->writeCharacters(text);
    out->writeEndElement();
}
