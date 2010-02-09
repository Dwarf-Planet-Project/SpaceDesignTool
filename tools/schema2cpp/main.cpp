/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/

/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QDomDocument>
#include <QDomNodeList>
#include <QFile>
#include <QDir>
#include <QSharedPointer>
#include <QHash>
#include <QSet>
#include <iostream>

/*
  schema2cpp is a tool that converts STA's Space Scenario XML schema into C++ classes
  that can be used to read, write, and manipulate space scenarios. Only a subset of
  of XML Schema functionality is handled, for two reasons:
     1. Not all of the possible data models describable in and XML Schema can be
        conveniently represented in statically typed languages like C++.
     2. A data binding tool that can handle any XML Schema would simply take too
        long to develop.

   schema2cpp emits code that is Qt-friendly: QStrings are used to represent strings,
   QDateTime is used for dates, and the QtXML framework is used for converting to and
   from XML.
*/

const QString INDENT = "    ";
const QString INDENT2 = INDENT + INDENT;
const QString INDENT3 = INDENT2 + INDENT;

const QString CLASS_PREFIX = "Scenario";
const QString XMLNS = "tns";

#define SHOW_DEBUG_INFO 0

// String utility function
QString capitalizeFirst(const QString& s)
{
    if (s.isEmpty())
    {
        return s;
    }
    else
    {
        return s.at(0).toUpper() + s.right(s.length() - 1);
    }
}

QString lowercaseFirst(const QString& s)
{
    if (s.isEmpty())
    {
        return s;
    }
    else
    {
        return s.at(0).toLower() + s.right(s.length() - 1);
    }
}

QString quoteString(const QString& s)
{
    return "\"" + s + "\"";
}

QString stripNamespace(const QString& s)
{
    int position = s.indexOf(":");
    if (s > 0)
    {
        return s.right(s.length() - position - 1);
    }
    else
    {
        return s;
    }
}


class ComplexType;

class SchemaType
{
public:
    enum Type
    {
        Boolean,
        Double,
        Integer,
        String,
        DateTime,
        DoubleList,
        IntegerList,
        StringList,
        Complex
    };

    SchemaType(Type type) :
        m_type(type)
    {
    }

    SchemaType(QString complexTypeName) :
        m_type(Complex),
        m_complexTypeName(complexTypeName)
    {
    }

    SchemaType(const SchemaType& other) :
        m_type(other.m_type),
        m_complexTypeName(other.m_complexTypeName)
    {
    }

    SchemaType operator=(const SchemaType& other)
    {
        m_type = other.m_type;
        m_complexTypeName = other.m_complexTypeName;

        return *this;
    }

    Type type() const
    {
        return m_type;
    }

    bool isComplex() const
    {
        return type() == Complex;
    }

    bool isArrayType() const
    {
        switch (m_type)
        {
        case DoubleList:
        case StringList:
        case IntegerList:
            return true;
        default:
            return false;
        }
    }

    QString complexTypeName() const
    {
        return m_complexTypeName;
    }

    QString ctype() const;

    QString memberVariableType() const
    {
        if (isComplex())
        {
            return "QSharedPointer<" + ctype() + ">";
        }
        else
        {
            return ctype();
        }
    }

    QString memberVariableVectorType() const
    {
        if (isComplex())
        {
            return "QList<QSharedPointer<" + ctype() + "> >";
        }
        else
        {
            return "QList<" + ctype() + ">";
        }
    }

    QString parsingFunction() const
    {
        switch (m_type)
        {
        case Boolean:
            return "parseBoolean";
        case Double:
            return "parseDouble";
        case Integer:
            return "parseInt";
        case DateTime:
            return "parseDateTime";
        case String:
            return "";
        case DoubleList:
            return "parseDoubleList";
        case StringList:
            return "parseStringList";
        case IntegerList:
            return "parseIntList";
        default:
            return "";
        }
    }

    static SchemaType CreateFromTypeName(const QString& name);

private:
    Type m_type;
    QString m_complexTypeName;
};


class Property
{
public:
    Property(const QString& name, const SchemaType& type, int minOccurs = 1, int maxOccurs = 1, bool isReference = false) :
       m_type(type),
       m_name(name),
       m_minOccurs(minOccurs),
       m_maxOccurs(maxOccurs),
       m_isReference(isReference)
    {
    }

    Property(const Property& other) :
        m_type(other.m_type),
        m_name(other.m_name),
        m_minOccurs(other.m_minOccurs),
        m_maxOccurs(other.m_maxOccurs),
        m_isReference(other.m_isReference)
    {
    }

    Property& operator=(const Property& other)
    {
        m_type = other.m_type;
        m_name = other.m_name;
        m_minOccurs = other.m_minOccurs;
        m_maxOccurs = other.m_maxOccurs;
        m_isReference = other.m_isReference;

        return *this;
    }

    SchemaType type() const
    {
        return m_type;
    }

    QString name() const
    {
        return m_name;
    }

    QString memberVariableName() const
    {
        return "m_" + name();
    }

    QString accessorName() const
    {
        return name();
    }

    QString mutatorName() const
    {
        return "set" +  capitalizeFirst(name());
    }

    int minOccurs() const
    {
        return m_minOccurs;
    }

    int maxOccurs() const
    {
        return m_maxOccurs;
    }

    enum {
        UNBOUNDED = -1
    };

    bool multipleOccurrencesAllowed() const
    {
        return m_maxOccurs == UNBOUNDED || m_maxOccurs > 1;
    }

    bool isOptional() const
    {
        return m_minOccurs == 0;
    }

    bool isReference() const
    {
        return m_isReference;
    }

private:
    SchemaType m_type;
    QString m_name;
    int m_minOccurs;
    int m_maxOccurs;
    bool m_isReference;
};


class Element
{
public:
    Element(const QString& name, const SchemaType& type) :
        m_name(name),
        m_type(type),
        m_defaultValue(""),
        m_minOccurs(1),
        m_maxOccurs(1),
        m_substitutionGroup(NULL),
        m_isAbstract(false)
    {
    }

    Element(const Element& other) :
        m_name(other.m_name),
        m_type(other.m_type),
        m_defaultValue(other.m_defaultValue),
        m_minOccurs(other.m_minOccurs),
        m_maxOccurs(other.m_maxOccurs),
        m_substitutionGroup(other.m_substitutionGroup),
        m_isAbstract(other.m_isAbstract),
        m_substitutes(other.m_substitutes)
    {
    }

    Element& operator=(const Element& other)
    {
        m_name         = other.m_name;
        m_type         = other.m_type;
        m_defaultValue = other.m_defaultValue;
        m_minOccurs     = other.m_minOccurs;
        m_maxOccurs     = other.m_maxOccurs;
        m_substitutionGroup = other.m_substitutionGroup;
        m_isAbstract    = other.m_isAbstract;
        m_substitutes   = other.m_substitutes;

        return *this;
    }

    QString name() const
    {
        return m_name;
    }

    SchemaType type() const
    {
        return m_type;
    }

    QString defaultValue() const
    {
        return m_defaultValue;
    }

    void setDefaultValue(const QString& defaultValue)
    {
        m_defaultValue = defaultValue;
    }

    int minOccurs() const
    {
        return m_minOccurs;
    }

    void setMinOccurs(int minOccurs)
    {
        m_minOccurs = minOccurs;
    }

    int maxOccurs() const
    {
        return m_maxOccurs;
    }

    void setMaxOccurs(int maxOccurs)
    {
        m_maxOccurs = maxOccurs;
    }

    Element* substitutionGroup() const
    {
        return m_substitutionGroup;
    }

    void setSubstitutionGroup(Element* subgroup)
    {
        m_substitutionGroup = subgroup;
    }

    bool isAbstract() const
    {
        return m_isAbstract;
    }

    void setAbstract(bool isAbstract)
    {
        m_isAbstract = isAbstract;
    }

    QList<QSharedPointer<Element> > substitutes()
    {
        return m_substitutes;
    }

    void addSubstitute(QSharedPointer<Element> e)
    {
        m_substitutes << e;
    }

    enum {
        UNBOUNDED = -1
    };

private:
    QString m_name;
    SchemaType m_type;
    QString m_defaultValue;
    int m_minOccurs;
    int m_maxOccurs;
    Element* m_substitutionGroup;
    bool m_isAbstract;
    QList<QSharedPointer<Element> > m_substitutes;
};


class ComplexType
{
public:
    ComplexType(const QString& name, const QSharedPointer<SchemaType> baseType = QSharedPointer<SchemaType>()) :
        m_name(name),
        m_baseType(baseType),
        m_isAbstract(false)
    {
    }

    const QList<Property>& properties() const
    {
        return m_properties;
    }

    QString name() const
    {
        return m_name;
    }

    QSharedPointer<SchemaType> baseType() const
    {
        return m_baseType;
    }

    QString className() const
    {
        return CLASS_PREFIX + name();
    }

    QString baseClassName() const
    {
        if (baseType().isNull())
        {
            return CLASS_PREFIX + "Object";
        }
        else
        {
            return baseType()->ctype();
        }
    }

    bool isEmpty() const
    {
        return m_properties.isEmpty();
    }

    void addProperty(const Property& property)
    {
        m_properties << property;
    }

    const QList<QSharedPointer<ComplexType> > subclasses() const
    {
        return m_subclasses;
    }

    void addSubclass(QSharedPointer<ComplexType> subclass)
    {
        m_subclasses << subclass;
    }

    bool isAbstract() const
    {
        return m_isAbstract;
    }

    void setAbstract(bool isAbstract)
    {
        m_isAbstract = isAbstract;
    }

    void writeClassDefinition(QTextStream& out);
    void writeMethodDefinitions(QTextStream& out);
    void writeForwardDeclaration(QTextStream& out);

private:
    void writeMemberVariables(QTextStream& out);
    void writePublicInterface(QTextStream& out);
    void writeDefaultConstructor(QTextStream& out);
    void writeDomFactory(QTextStream& out);
    void writeDomLoader(QTextStream& out);
    void writeDomSaver(QTextStream& out);

private:
    QString m_name;
    QSharedPointer<SchemaType> m_baseType;
    QList<Property> m_properties;
    QList<QSharedPointer<ComplexType> > m_subclasses;
    bool m_isAbstract;
};


QHash<QString, QSharedPointer<SchemaType> > g_GlobalTypes;
QHash<QString, QSharedPointer<ComplexType> > g_GlobalComplexTypes;
QHash<QString, QSharedPointer<Element> > g_GlobalElements;
QList<QSharedPointer<ComplexType> > g_AllSchemaTypes;


QString
SchemaType::ctype() const
{
    switch (m_type)
    {
    case Boolean:
        return "bool";
    case Double:
        return "double";
    case Integer:
        return "int";
    case DateTime:
        return "QDateTime";
    case String:
        return "QString";
    case DoubleList:
        return "QList<double>";
    case StringList:
        return "QList<QString>";
    case IntegerList:
        return "QList<int>";
    case Complex:
        return CLASS_PREFIX + m_complexTypeName;
    default:
        Q_ASSERT(0);
        return "BAD";
    }
}


SchemaType
SchemaType::CreateFromTypeName(const QString& name)
{
    SchemaType::Type propType = SchemaType::Complex;
    QString complexTypeName;

    if (name == "double")
    {
        propType = SchemaType::Double;
    }
    else if (name == "string")
    {
        propType = SchemaType::String;
    }
    else if (name == "boolean")
    {
        propType = SchemaType::Boolean;
    }
    else if (name == "int")
    {
        propType = SchemaType::Integer;
    }
    else if (name == "dateTime")
    {
        propType = SchemaType::DateTime;
    }
    else if (name == "ListOfDouble")
    {
        propType = SchemaType::DoubleList;
    }
    else if (name == "ListOfString")
    {
        propType = SchemaType::StringList;
    }
    else if (name == "ListOfInt")
    {
        propType = SchemaType::IntegerList;
    }
    else
    {
        complexTypeName = name;
        propType = SchemaType::Complex;
    }

    if (propType == SchemaType::Complex)
    {
        return SchemaType(complexTypeName);
    }
    else
    {
        return SchemaType(propType);
    }
}


void
ComplexType::writeForwardDeclaration(QTextStream& out)
{
    out << "class " << className() << ";\n";
}


void
ComplexType::writeClassDefinition(QTextStream& out)
{
    out << "class " << className() << " : public " << baseClassName() << "\n";
    out << "{\n";

    writePublicInterface(out);
    out << "\n";
    writeMemberVariables(out);

    out << "};\n";
}


void
ComplexType::writeMethodDefinitions(QTextStream& out)
{
    writeDefaultConstructor(out);
    out << "\n";
    writeDomFactory(out);
    out << "\n";
    writeDomLoader(out);
    out << "\n";
    writeDomSaver(out);
    out << "\n\n";
}


void
ComplexType::writeMemberVariables(QTextStream& out)
{
    out << "private:\n";
    foreach (Property p, m_properties)
    {
        if (p.multipleOccurrencesAllowed())
        {
            out << INDENT << p.type().memberVariableVectorType() << " " << p.memberVariableName() << ";\n";
        }
        else
        {
            out << INDENT << p.type().memberVariableType() << " " << p.memberVariableName() << ";\n";
        }
    }
}


void
ComplexType::writePublicInterface(QTextStream& out)
{
    out << "public:\n";

    // Default constructor
    out << INDENT << className() << "();\n";

    // Factory method
    out << INDENT << "static " << className() << "* create(const QDomElement& e);\n";

    // DOM Node loader
    out << INDENT << "virtual bool " << "load(const QDomElement& e, QDomElement* nextElement);\n";

    // DOM Node writer
    out << INDENT << "virtual QDomElement toDomElement(QDomDocument&) const;\n";
    out << "\n";

    // Accessors and mutators are all defined inline
    foreach (Property p, m_properties)
    {
        // accessor
        if (p.type().isArrayType())
        {
            // Create separate const and non-const array accessors
            out << INDENT << "const " << p.type().memberVariableType() << "& " << p.accessorName() << "() const\n";
            out << INDENT << "{ " << "return " << p.memberVariableName() << "; }\n";
            out << INDENT << p.type().memberVariableType() << "& " << p.accessorName() << "()\n";
            out << INDENT << "{ " << "return " << p.memberVariableName() << "; }\n";
        }
        else if (p.multipleOccurrencesAllowed())
        {
            // Create separate const and non-const array accessors
            out << INDENT << "const " << p.type().memberVariableVectorType() << "& " << p.accessorName() << "() const\n";
            out << INDENT << "{ " << "return " << p.memberVariableName() << "; }\n";
            out << INDENT << p.type().memberVariableVectorType() << "& " << p.accessorName() << "()\n";
            out << INDENT << "{ " << "return " << p.memberVariableName() << "; }\n";
        }
        else
        {
            out << INDENT << p.type().memberVariableType() << " " << p.accessorName() << "() const\n";
            out << INDENT << "{ " << "return " << p.memberVariableName() << "; }\n";
        }

        // mutator
        if (p.multipleOccurrencesAllowed())
        {
            out << INDENT << "void " << p.mutatorName() << "(" << p.type().memberVariableVectorType() << " " << p.name() << ")\n";
            out << INDENT << "{ " << p.memberVariableName() << " = " << p.name() << "; }\n";
        }
        else
        {
            out << INDENT << "void " << p.mutatorName() << "(" << p.type().memberVariableType() << " " << p.name() << ")\n";
            out << INDENT << "{ " << p.memberVariableName() << " = " << p.name() << "; }\n";
        }
    }
}


void
ComplexType::writeDefaultConstructor(QTextStream& out)
{
    out << className() << "::" << className() << "()";

    bool initializerList = false;
    foreach (Property p, m_properties)
    {
        QString defaultValue;

        switch (p.type().type())
        {
        case SchemaType::Double:
            defaultValue = "0.0";
            break;
        case SchemaType::Integer:
            defaultValue = "0";
            break;
        default:
            break;
        }

        if (!defaultValue.isEmpty())
        {
            if (!initializerList)
            {
                initializerList = true;
                out << " :\n";
            }
            else
            {
                out << ",\n";
            }

            out << INDENT << p.memberVariableName() << "(" << defaultValue << ")";
        }
    }

    out << "\n";
    out << "{\n";
    out << "}\n";
}


void
ComplexType::writeDomFactory(QTextStream& out)
{
    QString returnType = className() + "*";

    out << returnType << " " << className() << "::create(const QDomElement& e)\n";
    out << "{\n";
    out << INDENT << returnType << " v;\n";
    if (!isAbstract())
    {
        out << INDENT << "if (e.tagName() == " << quoteString(XMLNS + ":" + this->name()) << ")\n";
        out << INDENT << "{\n";
        out << INDENT2 << "v = new " << className() << ";\n";
        out << INDENT2 << "QDomElement nextElement = e.firstChildElement();\n";
        out << INDENT2 << "v->load(e, &nextElement);\n";
        out << INDENT2 << "return v;\n";
        out << INDENT << "}\n";
    }

    foreach (QSharedPointer<ComplexType> subType, subclasses())
    {
        out << INDENT << "if (e.tagName() == " << quoteString(XMLNS + ":" + subType->name()) << ")\n";
        out << INDENT2 << "return " << subType->className() << "::create(e);\n";
    }

    out << INDENT << "return NULL;\n";
    out << "}\n";
}


void
ComplexType::writeDomLoader(QTextStream& out)
{
    const QString ADVANCE = "*next = next->nextSiblingElement()";

    out << "bool " << className() << "::load(const QDomElement& e, QDomElement* next)\n";
    out << "{\n";

    out << INDENT << baseClassName() << "::load(e, next);\n";

#if SHOW_DEBUG_INFO
    out << INDENT << "cerr << " << quoteString(name()) << " << endl;\n";
#endif

    foreach (Property p, m_properties)
    {
        if (p.type().isComplex())
        {
            if (p.multipleOccurrencesAllowed())
            {
                out << INDENT << "for (;;)\n";
                out << INDENT << "{\n";
                out << INDENT2 << p.type().memberVariableType() << " v(" << p.type().ctype() << "::create(*next));\n";
                out << INDENT2 << "if (v.isNull()) break; else {\n";
                out << INDENT3 << p.memberVariableName() << " << v;\n";
                out << INDENT3 << ADVANCE << ";\n";
                out << INDENT2 << "}\n";
                out << INDENT << "}\n";
            }
            else
            {
                out << INDENT << p.memberVariableName() << " = " <<
                                 p.type().memberVariableType() << "(" << p.type().ctype() << "::create(*next));\n";
                if (p.isOptional())
                {
                    // Only advance if the read was successful
                    out << "if (!" << p.memberVariableName() << ".isNull())\n" << INDENT;
                }
                out << INDENT << ADVANCE << ";\n";
            }
        }
        else
        {
            if (p.isOptional())
            {
                out << INDENT << "if (next->tagName() == " << quoteString(XMLNS + ":" + p.name()) << ")\n";
                out << INDENT << "{\n";
            }

            //out << "cerr << \"OPT:\" << next->text().toAscii().data() << endl;";
            out << INDENT2 << p.memberVariableName() << " = " << p.type().parsingFunction() << "(next->firstChild().toText().data());\n";
            out << INDENT2 << ADVANCE << ";\n";

            if (p.isOptional())
            {
                out << INDENT << "}\n";
            }
        }
    }

    out << INDENT << "return true;\n";
    out << "}\n";
}


void
ComplexType::writeDomSaver(QTextStream& out)
{
    out << "QDomElement " << className() << "::toDomElement(QDomDocument& doc) const\n";
    out << "{\n";

    out << INDENT << "QDomElement e = " << baseClassName() << "::toDomElement(doc);\n";
    out << INDENT << "e.setTagName(" << quoteString(name()) << ");\n";
    foreach (Property p, m_properties)
    {
        if (p.type().isComplex())
        {
            if (p.multipleOccurrencesAllowed())
            {
                out << INDENT << "foreach (" << p.type().memberVariableType() << " p, " << p.memberVariableName() << ")\n";
                out << INDENT << "{\n";
                out << INDENT2 << "e.appendChild(" << "p->toDomElement(doc));\n";
                out << INDENT << "}\n";
            }
            else
            {
                out << INDENT  << "if (!" << p.memberVariableName() << ".isNull())\n";
                out << INDENT  << "{\n";
                out << INDENT2 << "QDomElement child = " << p.memberVariableName() << "->toDomElement(doc);\n";
                if (!p.isReference())
                {
                    out << INDENT2 << "child.setTagName(" << quoteString(p.name()) << ");\n";
                }
                out << INDENT2 << "e.appendChild(child);\n";
                out << INDENT  << "}\n";
            }
        }
        else
        {
            out << INDENT  << "{\n";
            out << INDENT2 << "QDomElement child = doc.createElement(" << quoteString(p.name()) << ");\n";
            out << INDENT2 << "child.appendChild(doc.createTextNode(convertToString(" << p.memberVariableName() << ")));\n";
            out << INDENT2 << "e.appendChild(child);\n";
            out << INDENT  << "}\n";
        }
    }

    out << INDENT << "return e;\n";
    out << "}\n";
}


ComplexType* CreateComplexType(QDomElement e)
{
    QDomElement parentElement = e.parentNode().toElement();
    Q_ASSERT(!parentElement.isNull());

    bool parentIsElement = parentElement.tagName() == "element";

    QString typeName = e.attributes().namedItem("name").nodeValue();
    if (typeName.isEmpty())
    {
        // If no name for the complexType is specified, use the name of the
        // parent element.
        if (parentIsElement)
        {
            typeName = parentElement.attributes().namedItem("name").nodeValue();
        }
    }

    // No name means that there's a problem in the schema
    if (typeName.isEmpty())
    {
        std::cerr << "Missing name for complexType.\n";
        return NULL;
    }

    QDomElement first = e.firstChildElement();
    QDomElement sequence;
    QString base;
    if (first.tagName() == "sequence")
    {
        sequence = first;
    }
    else if (first.tagName() == "complexContent")
    {
        first = first.firstChildElement();
        if (first.tagName() == "sequence")
        {
            sequence = first;
        }
        else if (first.tagName() == "extension")
        {
            base = stripNamespace(first.attribute("base"));
            sequence = first.firstChildElement();
            if (sequence.tagName() != "sequence" && !sequence.isNull())
            {
                std::cerr << "Unrecognized complexType definition pattern for " << typeName.toAscii().data() << "\n";
                return NULL;
            }
        }
        else
        {
            std::cerr << "Unrecognized complexType definition pattern for " << typeName.toAscii().data() << "\n";
            return NULL;
        }
    }

    QSharedPointer<SchemaType> baseType;
    if (!base.isEmpty())
    {
        if (g_GlobalTypes.contains(base))
        {
            baseType = g_GlobalTypes[base];
            if (!baseType->isComplex())
            {
                std::cerr << "Extension of simple types is not supported.\n";
                return NULL;
            }            
        }
        else
        {
            std::cerr << "Base type " << base.toAscii().data() << " is not defined.\n";
            return NULL;
        }
    }

    ComplexType* complexType = new ComplexType(typeName, baseType);


    QDomNodeList children = sequence.childNodes();
    for (uint i = 0; i < children.length(); ++i)
    {
        QDomNode child = children.item(i);
        if (child.isElement())
        {
            QDomElement e = child.toElement();
            if (e.tagName() == "element")
            {
                QString name = e.attribute("name");
                QString ref = e.attribute("ref");

                int minOccurs = 1;
                int maxOccurs = 1;
                if (e.hasAttribute("minOccurs"))
                {
                    minOccurs = e.attribute("minOccurs").toInt();
                }
                if (e.hasAttribute("maxOccurs"))
                {
                    if (e.attribute("maxOccurs").toLower() == "unbounded")
                    {
                        maxOccurs = Property::UNBOUNDED;
                    }
                    else
                    {
                        maxOccurs = e.attribute("maxOccurs").toInt();
                    }
                }

                if (!ref.isEmpty())
                {
                    QString refElementName = stripNamespace(ref);
                    if (!g_GlobalElements.contains(refElementName))
                    {
                        std::cerr << "Undefined element " << refElementName.toAscii().data() << " in ref.\n";
                    }
                    else
                    {
                        QSharedPointer<Element> refElement = g_GlobalElements[refElementName];
                        Property property(refElement->name(), refElement->type(), minOccurs, maxOccurs, true);
                        complexType->addProperty(property);
                    }
                }
                else if (!name.isEmpty())
                {
                    QString type = stripNamespace(e.attribute("type"));
                    if (type.isEmpty())
                    {
                        // Anonymous type; use element name as type.
                        type = name;
                    }
                    SchemaType schemaType = SchemaType::CreateFromTypeName(type);
                    Property property(name, schemaType, minOccurs, maxOccurs);
                    complexType->addProperty(property);
                }
            }
        }
    }

    return complexType;
}


QSet<QString> g_LoadedSchemaFiles;

bool ParseSchemaFile(const QString& filename)
{
    QFile in(filename);
    if (!in.open(QIODevice::ReadOnly))
    {
        std::cerr << "Error opening schema file " << filename.toAscii().data() << std::endl;
        return false;
    }

    QDomDocument doc("STA Schema");
    if (!doc.setContent(&in))
    {
        std::cerr << "Error parsing schema file " << filename.toAscii().data() << std::endl;
        return false;
    }

    QDomNodeList includes = doc.elementsByTagName("include");
    for (uint i = 0; i < includes.length(); ++i)
    {
        QDomElement e = includes.item(i).toElement();
        QString schemaLocation = e.attribute("schemaLocation");
        if (!schemaLocation.isEmpty())
        {
            QDir currentFileDir = QFileInfo(in).dir();
            QString includeFile = currentFileDir.canonicalPath() + "/" + schemaLocation;
            if (!g_LoadedSchemaFiles.contains(includeFile))
            {
                std::cerr << "Include: " << includeFile.toAscii().data() << std::endl;
                g_LoadedSchemaFiles.insert(includeFile);
                bool ok = ParseSchemaFile(includeFile);
                if (!ok)
                {
                    return false;
                }
            }
        }
    }


    QDomNodeList elements;

    // Update the global elements table
    elements = doc.elementsByTagName("element");
    for (uint i = 0; i < elements.length(); ++i)
    {
        QDomElement e = elements.item(i).toElement();
        if (e.parentNode().toElement().tagName() == "schema")
        {
            QString name = e.attribute("name");
            if (g_GlobalElements.contains(name))
            {
                std::cerr << "Element " << name.toAscii().data() << " is defined more than once.\n";
            }
            else
            {
                QString type = stripNamespace(e.attribute("type"));
                if (type.isEmpty())
                {
                    // The element uses an anonymous type; assign the type the same
                    // name as the element.
                    type = name;
                }

                SchemaType schemaType = SchemaType::CreateFromTypeName(type);
                QSharedPointer<Element> element(new Element(name, schemaType));

                QString substitutionGroup = stripNamespace(e.attribute("substitutionGroup"));
                if (!substitutionGroup.isEmpty())
                {
                    if (g_GlobalElements.contains(substitutionGroup))
                    {
                        QSharedPointer<Element> subElement = g_GlobalElements[substitutionGroup];
                        element->setSubstitutionGroup(subElement.data());
                        subElement->addSubstitute(element);                        
                    }
                    else
                    {
                        std::cerr << "Substitution group " << substitutionGroup.toAscii().data() << " not defined!\n";
                    }
                }
                element->setAbstract(e.attribute("abstract") == "true");

                g_GlobalElements.insert(name, element);
                //std::cerr << "ELEMENT: " << name.toAscii().data() << " => " << type.toAscii().data() << std::endl;
            }
        }
    }

    elements = doc.elementsByTagName("complexType");
    for (uint i = 0; i < elements.length(); ++i)
    {
        QDomElement e = elements.item(i).toElement();

        QSharedPointer<ComplexType> complexType(CreateComplexType(e));
        if (!complexType.isNull())
        {
            g_AllSchemaTypes << complexType;
            QSharedPointer<SchemaType> baseType = complexType->baseType();
            if (!baseType.isNull())
            {
                QString complexBaseName = baseType->complexTypeName();
                if (complexBaseName.isEmpty())
                {
                    std::cerr << "Invalid base type\n";
                }
                else
                {
                    if (!g_GlobalComplexTypes.contains(complexBaseName))
                    {
                        std::cerr << "Undefined base type " << complexBaseName.toAscii().data() << std::endl;
                    }
                    else
                    {
                        QSharedPointer<ComplexType> complexBase = g_GlobalComplexTypes[complexBaseName];
                        complexBase->addSubclass(complexType);
                    }
                }
            }
        }

        if (e.parentNode().toElement().tagName() == "schema")
        {
            QString name = e.attribute("name");
            QSharedPointer<SchemaType> schemaType(new SchemaType(name));
            g_GlobalTypes.insert(name, schemaType);
            if (!complexType.isNull())
            {
                g_GlobalComplexTypes.insert(name, complexType);
            }
        }
    }

    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();

    QStringList schemaFiles = args;
    schemaFiles.removeAt(0);

    // Parse all the schema files given on the command line
    foreach (QString filename, schemaFiles)
    {
        ParseSchemaFile(filename);
    }

    QString outputFile = "staschema";
    QString headerFileName = outputFile + ".h";
    QString sourceFileName = outputFile + ".cpp";
    QFile headerFile(headerFileName);
    QFile sourceFile(sourceFileName);

    if (!headerFile.open(QIODevice::WriteOnly))
    {
        std::cerr << "Error creating header file.\n";
        return 1;
    }

    if (!sourceFile.open(QIODevice::WriteOnly))
    {
        std::cerr << "Error creating source file.\n";
        return 1;
    }


    // Create the header file
    QTextStream header(&headerFile);

    // Write the header file preamble
    header << "class QDomElement;\n";

    header << "#include \"stascenarioutil.h\"\n";
    header << "\n";

    header << "class ScenarioObject\n";
    header << "{\n";
    header << INDENT << "public:\n";
    header << INDENT << "virtual bool load(const QDomElement& e, QDomElement* nextElement)\n";
    header << INDENT << "{\n";
    header << INDENT2 << "*nextElement = e.firstChildElement();\n";
    header << INDENT2 << "return true;\n";
    header << INDENT << "}\n";
    header << "\n";
    header << INDENT << "virtual QDomElement toDomElement(QDomDocument& doc) const\n";
    header << INDENT << "{\n";
    header << INDENT2 << "return doc.createElement(\"Object\");\n";
    header << INDENT << "}\n";
    header << "};\n";
    header << "\n";

    header << "// Forward declarations\n\n";
    foreach (QSharedPointer<ComplexType> e, g_AllSchemaTypes)
    {
        e->writeForwardDeclaration(header);
    }
    header << "\n\n";

    header << "// Space scenario class definitions\n\n";
    header << "// Automatically generated by schema2cpp - DO NOT EDIT!\n\n";

    foreach (QSharedPointer<ComplexType> e, g_AllSchemaTypes)
    {
        header << "// " << e->className() << "\n";
        e->writeClassDefinition(header);
        header << "\n\n";
    }

    headerFile.close();
    // Header complete


    // Create the source file
    QTextStream source(&sourceFile);

    // Write the preamble
    source << "// Space scenario class implementations\n\n";
    source << "// Automatically generated by schema2cpp - DO NOT EDIT!\n\n";

    source << "#include <QtXml>\n";
    source << "#include " << quoteString(headerFileName) << "\n";
    source << "\n";

#if SHOW_DEBUG_INFO
    source << "#include <iostream>\n";
    source << "using namespace std;\n";
#endif

    foreach (QSharedPointer<ComplexType> e, g_AllSchemaTypes)
    {
        source << "// " << e->className() << "\n";
        e->writeMethodDefinitions(source);
        source << "\n\n";
    }

    sourceFile.close();
    // Source file complete

    //return app.exec();

    return 0;
}

