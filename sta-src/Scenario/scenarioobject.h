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

#ifndef _STA_SCENARIO_OBJECT_H_
#define _STA_SCENARIO_OBJECT_H_

#include <QString>
#include <QObject>
#include <QXmlStreamWriter>
#include <Eigen/Geometry>

class QTreeWidgetItem;

/*! ScenarioObject is the base class for all objects that may appear in an
 *  STA scenario.
 */
class ScenarioObject
{
public:
    ScenarioObject() : m_parent(0), m_refCount(0) {}
    virtual ~ScenarioObject();
    
    ScenarioObject* parent() const
    {
        return m_parent;
    }
    
    bool isAttached() const
    {
        return m_parent != 0;
    }
    
    /*! Add a reference to this scenario object. */
    void addRef()
    {
        m_refCount++;
    }
    
    /*! Release a reference to this scenario object. The object will
     *  be deleted when the reference count reaches zero.
     */
    void release()
    {
        Q_ASSERT(m_refCount > 0);
        m_refCount--;
        if (m_refCount == 0)
            delete this;
    }
    
    /*! Return the number of references to this object.
     */
    int refCount() const
    {
        return m_refCount;
    }
    
    virtual void writeElement(QXmlStreamWriter* out, QString xmlElementName = "");
    virtual void writeContents(QXmlStreamWriter* out);
    
    QTreeWidgetItem* createItem(QTreeWidgetItem* parent, QString itemName = "");
    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent, QString itemName = "");
    virtual void createItemContents(QTreeWidgetItem* item);
    
    virtual bool removeChild(unsigned int index);

    virtual QString elementName() const { return ""; }
    virtual QString displayName() const { return elementName(); }
    
    static void safeRelease(ScenarioObject* object)
    {
        if (object)
            object->release();
    }
    
    static void writeNumericElement(QXmlStreamWriter* out, QString name, double value, QString units = "");
    static void writeBooleanElement(QXmlStreamWriter* out, QString name, bool value);
    static void writeVectorElement(QXmlStreamWriter* out, QString name, const Eigen::Vector3d& value, QString units = "");
    static void writeStringElement(QXmlStreamWriter* out, QString name, QString text);
    
protected:
    void attach(ScenarioObject* parent);
    void detach();
    void attachChild(ScenarioObject* child);
    void detachChild(ScenarioObject* child);
    
private:
    ScenarioObject* m_parent;
    int m_refCount;
};


// For use in the tree view
enum { ScenarioObjectRole = Qt::UserRole + 100 };


#endif // _STA_SCENARIO_OBJECT_H_
