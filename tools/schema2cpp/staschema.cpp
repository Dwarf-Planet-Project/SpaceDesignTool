#include <QtXml>
#include "staschema.h"

// ScenarioParticipantsType
ScenarioParticipantsType::ScenarioParticipantsType()
{
}

void ScenarioParticipantsType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Name = (e.firstChildElement("Name").text());
}

QDomElement ScenarioParticipantsType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("Name");
        child.appendChild(doc.createTextNode(convertToString(m_Name);
        e.appendChild(child);
    }
}




// ScenarioOptVarDouble
ScenarioOptVarDouble::ScenarioOptVarDouble() :
    m_min(0.0),
    m_value(0.0),
    m_max(0.0)
{
}

void ScenarioOptVarDouble::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_min = parseDouble(e.firstChildElement("min").text());
    m_value = parseDouble(e.firstChildElement("value").text());
    m_max = parseDouble(e.firstChildElement("max").text());
}

QDomElement ScenarioOptVarDouble::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("min");
        child.appendChild(doc.createTextNode(convertToString(m_min);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("value");
        child.appendChild(doc.createTextNode(convertToString(m_value);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("max");
        child.appendChild(doc.createTextNode(convertToString(m_max);
        e.appendChild(child);
    }
}




// ScenarioOptVarInt
ScenarioOptVarInt::ScenarioOptVarInt() :
    m_value(0)
{
}

void ScenarioOptVarInt::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_list = parseIntList(e.firstChildElement("list").text());
    m_value = parseInt(e.firstChildElement("value").text());
}

QDomElement ScenarioOptVarInt::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("list");
        child.appendChild(doc.createTextNode(convertToString(m_list);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("value");
        child.appendChild(doc.createTextNode(convertToString(m_value);
        e.appendChild(child);
    }
}




// ScenarioOptVarBool
ScenarioOptVarBool::ScenarioOptVarBool()
{
}

void ScenarioOptVarBool::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_value = parseBoolean(e.firstChildElement("value").text());
}

QDomElement ScenarioOptVarBool::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("value");
        child.appendChild(doc.createTextNode(convertToString(m_value);
        e.appendChild(child);
    }
}




// ScenarioOptVarString
ScenarioOptVarString::ScenarioOptVarString()
{
}

void ScenarioOptVarString::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_list = parseStringList(e.firstChildElement("list").text());
    m_value = (e.firstChildElement("value").text());
}

QDomElement ScenarioOptVarString::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("list");
        child.appendChild(doc.createTextNode(convertToString(m_list);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("value");
        child.appendChild(doc.createTextNode(convertToString(m_value);
        e.appendChild(child);
    }
}




// ScenarioTimeLine
ScenarioTimeLine::ScenarioTimeLine() :
    m_StepTime(0.0)
{
}

void ScenarioTimeLine::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_StartTime = parseDateTime(e.firstChildElement("StartTime").text());
    m_StepTime = parseDouble(e.firstChildElement("StepTime").text());
    m_EndTime = parseDateTime(e.firstChildElement("EndTime").text());
}

QDomElement ScenarioTimeLine::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("StartTime");
        child.appendChild(doc.createTextNode(convertToString(m_StartTime);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("StepTime");
        child.appendChild(doc.createTextNode(convertToString(m_StepTime);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("EndTime");
        child.appendChild(doc.createTextNode(convertToString(m_EndTime);
        e.appendChild(child);
    }
}




// ScenarioPropagation
ScenarioPropagation::ScenarioPropagation()
{
}

void ScenarioPropagation::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_propagator = (e.firstChildElement("propagator").text());
}

QDomElement ScenarioPropagation::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("propagator");
        child.appendChild(doc.createTextNode(convertToString(m_propagator);
        e.appendChild(child);
    }
}




// ScenarioEnvironmentType
ScenarioEnvironmentType::ScenarioEnvironmentType()
{
}

void ScenarioEnvironmentType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_CentralBody = QSharedPointer<ScenarioCentralBodyType>(new ScenarioCentralBodyType());
    m_CentralBody->load(e.firstChildElement("CentralBody"));
}

QDomElement ScenarioEnvironmentType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_CentralBody.isNull())
    {
        QDomElement child = m_CentralBody->toDomElement(doc);
        child.setTagName("CentralBody");
        e.appendChild(child);
    }
}




// ScenarioCentralBodyType
ScenarioCentralBodyType::ScenarioCentralBodyType()
{
}

void ScenarioCentralBodyType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_GravityModel = QSharedPointer<ScenarioGravityModel>(new ScenarioGravityModel());
    m_GravityModel->load(e.firstChildElement("GravityModel"));
    m_atmosphere = (e.firstChildElement("atmosphere").text());
    m_ellipticity = parseBoolean(e.firstChildElement("ellipticity").text());
}

QDomElement ScenarioCentralBodyType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_GravityModel.isNull())
    {
        QDomElement child = m_GravityModel->toDomElement(doc);
        child.setTagName("GravityModel");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("atmosphere");
        child.appendChild(doc.createTextNode(convertToString(m_atmosphere);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("ellipticity");
        child.appendChild(doc.createTextNode(convertToString(m_ellipticity);
        e.appendChild(child);
    }
}




// ScenarioGravityModel
ScenarioGravityModel::ScenarioGravityModel() :
    m_numberOfZonals(0),
    m_numberOfTesserals(0)
{
}

void ScenarioGravityModel::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_modelName = (e.firstChildElement("modelName").text());
    m_numberOfZonals = parseInt(e.firstChildElement("numberOfZonals").text());
    m_numberOfTesserals = parseInt(e.firstChildElement("numberOfTesserals").text());
}

QDomElement ScenarioGravityModel::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("modelName");
        child.appendChild(doc.createTextNode(convertToString(m_modelName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("numberOfZonals");
        child.appendChild(doc.createTextNode(convertToString(m_numberOfZonals);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("numberOfTesserals");
        child.appendChild(doc.createTextNode(convertToString(m_numberOfTesserals);
        e.appendChild(child);
    }
}




// ScenarioAbstract3DOFPositionType
ScenarioAbstract3DOFPositionType::ScenarioAbstract3DOFPositionType()
{
}

void ScenarioAbstract3DOFPositionType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioAbstract3DOFPositionType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioAbstract6DOFPositionType
ScenarioAbstract6DOFPositionType::ScenarioAbstract6DOFPositionType()
{
}

void ScenarioAbstract6DOFPositionType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioAbstract6DOFPositionType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioGroundPositionType
ScenarioGroundPositionType::ScenarioGroundPositionType() :
    m_latitude(0.0),
    m_longitude(0.0),
    m_altitude(0.0)
{
}

void ScenarioGroundPositionType::load(const QDomElement& e)
{
    ScenarioAbstract3DOFPositionType::load(e);
    m_latitude = parseDouble(e.firstChildElement("latitude").text());
    m_longitude = parseDouble(e.firstChildElement("longitude").text());
    m_altitude = parseDouble(e.firstChildElement("altitude").text());
}

QDomElement ScenarioGroundPositionType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract3DOFPositionType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("latitude");
        child.appendChild(doc.createTextNode(convertToString(m_latitude);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("longitude");
        child.appendChild(doc.createTextNode(convertToString(m_longitude);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("altitude");
        child.appendChild(doc.createTextNode(convertToString(m_altitude);
        e.appendChild(child);
    }
}




// ScenarioStateVectorType
ScenarioStateVectorType::ScenarioStateVectorType() :
    m_x(0.0),
    m_y(0.0),
    m_z(0.0),
    m_vx(0.0),
    m_vy(0.0),
    m_vz(0.0)
{
}

void ScenarioStateVectorType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFPositionType::load(e);
    m_x = parseDouble(e.firstChildElement("x").text());
    m_y = parseDouble(e.firstChildElement("y").text());
    m_z = parseDouble(e.firstChildElement("z").text());
    m_vx = parseDouble(e.firstChildElement("vx").text());
    m_vy = parseDouble(e.firstChildElement("vy").text());
    m_vz = parseDouble(e.firstChildElement("vz").text());
}

QDomElement ScenarioStateVectorType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFPositionType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("x");
        child.appendChild(doc.createTextNode(convertToString(m_x);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("y");
        child.appendChild(doc.createTextNode(convertToString(m_y);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("z");
        child.appendChild(doc.createTextNode(convertToString(m_z);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("vx");
        child.appendChild(doc.createTextNode(convertToString(m_vx);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("vy");
        child.appendChild(doc.createTextNode(convertToString(m_vy);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("vz");
        child.appendChild(doc.createTextNode(convertToString(m_vz);
        e.appendChild(child);
    }
}




// ScenarioKeplerianElementsType
ScenarioKeplerianElementsType::ScenarioKeplerianElementsType() :
    m_semiMajorAxis(0.0),
    m_eccentricity(0.0),
    m_inclination(0.0),
    m_RAAN(0.0),
    m_argumentOfPeriapsis(0.0),
    m_trueAnomaly(0.0)
{
}

void ScenarioKeplerianElementsType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFPositionType::load(e);
    m_semiMajorAxis = parseDouble(e.firstChildElement("semiMajorAxis").text());
    m_eccentricity = parseDouble(e.firstChildElement("eccentricity").text());
    m_inclination = parseDouble(e.firstChildElement("inclination").text());
    m_RAAN = parseDouble(e.firstChildElement("RAAN").text());
    m_argumentOfPeriapsis = parseDouble(e.firstChildElement("argumentOfPeriapsis").text());
    m_trueAnomaly = parseDouble(e.firstChildElement("trueAnomaly").text());
}

QDomElement ScenarioKeplerianElementsType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFPositionType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("semiMajorAxis");
        child.appendChild(doc.createTextNode(convertToString(m_semiMajorAxis);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("eccentricity");
        child.appendChild(doc.createTextNode(convertToString(m_eccentricity);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("inclination");
        child.appendChild(doc.createTextNode(convertToString(m_inclination);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("RAAN");
        child.appendChild(doc.createTextNode(convertToString(m_RAAN);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("argumentOfPeriapsis");
        child.appendChild(doc.createTextNode(convertToString(m_argumentOfPeriapsis);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("trueAnomaly");
        child.appendChild(doc.createTextNode(convertToString(m_trueAnomaly);
        e.appendChild(child);
    }
}




// ScenarioSphericalCoordinatesType
ScenarioSphericalCoordinatesType::ScenarioSphericalCoordinatesType() :
    m_radialDistance(0.0),
    m_longitude(0.0),
    m_latitude(0.0),
    m_flightPathVelocity(0.0),
    m_flightPathAngle(0.0),
    m_headingAngle(0.0)
{
}

void ScenarioSphericalCoordinatesType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFPositionType::load(e);
    m_radialDistance = parseDouble(e.firstChildElement("radialDistance").text());
    m_longitude = parseDouble(e.firstChildElement("longitude").text());
    m_latitude = parseDouble(e.firstChildElement("latitude").text());
    m_flightPathVelocity = parseDouble(e.firstChildElement("flightPathVelocity").text());
    m_flightPathAngle = parseDouble(e.firstChildElement("flightPathAngle").text());
    m_headingAngle = parseDouble(e.firstChildElement("headingAngle").text());
}

QDomElement ScenarioSphericalCoordinatesType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFPositionType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("radialDistance");
        child.appendChild(doc.createTextNode(convertToString(m_radialDistance);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("longitude");
        child.appendChild(doc.createTextNode(convertToString(m_longitude);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("latitude");
        child.appendChild(doc.createTextNode(convertToString(m_latitude);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("flightPathVelocity");
        child.appendChild(doc.createTextNode(convertToString(m_flightPathVelocity);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("flightPathAngle");
        child.appendChild(doc.createTextNode(convertToString(m_flightPathAngle);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("headingAngle");
        child.appendChild(doc.createTextNode(convertToString(m_headingAngle);
        e.appendChild(child);
    }
}




// ScenarioAbstract6DOFAttitudeType
ScenarioAbstract6DOFAttitudeType::ScenarioAbstract6DOFAttitudeType()
{
}

void ScenarioAbstract6DOFAttitudeType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioAbstract6DOFAttitudeType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioEulerBLVLHType
ScenarioEulerBLVLHType::ScenarioEulerBLVLHType() :
    m_phi(0.0),
    m_theta(0.0),
    m_psi(0.0),
    m_phiDot(0.0),
    m_thetaDot(0.0),
    m_psiDot(0.0)
{
}

void ScenarioEulerBLVLHType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFAttitudeType::load(e);
    m_phi = parseDouble(e.firstChildElement("phi").text());
    m_theta = parseDouble(e.firstChildElement("theta").text());
    m_psi = parseDouble(e.firstChildElement("psi").text());
    m_phiDot = parseDouble(e.firstChildElement("phiDot").text());
    m_thetaDot = parseDouble(e.firstChildElement("thetaDot").text());
    m_psiDot = parseDouble(e.firstChildElement("psiDot").text());
}

QDomElement ScenarioEulerBLVLHType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFAttitudeType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("phi");
        child.appendChild(doc.createTextNode(convertToString(m_phi);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("theta");
        child.appendChild(doc.createTextNode(convertToString(m_theta);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("psi");
        child.appendChild(doc.createTextNode(convertToString(m_psi);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("phiDot");
        child.appendChild(doc.createTextNode(convertToString(m_phiDot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("thetaDot");
        child.appendChild(doc.createTextNode(convertToString(m_thetaDot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("psiDot");
        child.appendChild(doc.createTextNode(convertToString(m_psiDot);
        e.appendChild(child);
    }
}




// ScenarioEulerBIType
ScenarioEulerBIType::ScenarioEulerBIType() :
    m_phi(0.0),
    m_theta(0.0),
    m_psi(0.0),
    m_phiDot(0.0),
    m_thetaDot(0.0),
    m_psiDot(0.0)
{
}

void ScenarioEulerBIType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFAttitudeType::load(e);
    m_phi = parseDouble(e.firstChildElement("phi").text());
    m_theta = parseDouble(e.firstChildElement("theta").text());
    m_psi = parseDouble(e.firstChildElement("psi").text());
    m_phiDot = parseDouble(e.firstChildElement("phiDot").text());
    m_thetaDot = parseDouble(e.firstChildElement("thetaDot").text());
    m_psiDot = parseDouble(e.firstChildElement("psiDot").text());
}

QDomElement ScenarioEulerBIType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFAttitudeType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("phi");
        child.appendChild(doc.createTextNode(convertToString(m_phi);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("theta");
        child.appendChild(doc.createTextNode(convertToString(m_theta);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("psi");
        child.appendChild(doc.createTextNode(convertToString(m_psi);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("phiDot");
        child.appendChild(doc.createTextNode(convertToString(m_phiDot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("thetaDot");
        child.appendChild(doc.createTextNode(convertToString(m_thetaDot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("psiDot");
        child.appendChild(doc.createTextNode(convertToString(m_psiDot);
        e.appendChild(child);
    }
}




// ScenarioqBLVLHType
ScenarioqBLVLHType::ScenarioqBLVLHType() :
    m_q1(0.0),
    m_q2(0.0),
    m_q3(0.0),
    m_q4(0.0),
    m_q1Dot(0.0),
    m_q2Dot(0.0),
    m_q3Dot(0.0),
    m_q4Dot(0.0)
{
}

void ScenarioqBLVLHType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFAttitudeType::load(e);
    m_q1 = parseDouble(e.firstChildElement("q1").text());
    m_q2 = parseDouble(e.firstChildElement("q2").text());
    m_q3 = parseDouble(e.firstChildElement("q3").text());
    m_q4 = parseDouble(e.firstChildElement("q4").text());
    m_q1Dot = parseDouble(e.firstChildElement("q1Dot").text());
    m_q2Dot = parseDouble(e.firstChildElement("q2Dot").text());
    m_q3Dot = parseDouble(e.firstChildElement("q3Dot").text());
    m_q4Dot = parseDouble(e.firstChildElement("q4Dot").text());
}

QDomElement ScenarioqBLVLHType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFAttitudeType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("q1");
        child.appendChild(doc.createTextNode(convertToString(m_q1);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q2");
        child.appendChild(doc.createTextNode(convertToString(m_q2);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q3");
        child.appendChild(doc.createTextNode(convertToString(m_q3);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q4");
        child.appendChild(doc.createTextNode(convertToString(m_q4);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q1Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q1Dot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q2Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q2Dot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q3Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q3Dot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q4Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q4Dot);
        e.appendChild(child);
    }
}




// ScenarioqBIType
ScenarioqBIType::ScenarioqBIType() :
    m_q1(0.0),
    m_q2(0.0),
    m_q3(0.0),
    m_q4(0.0),
    m_q1Dot(0.0),
    m_q2Dot(0.0),
    m_q3Dot(0.0),
    m_q4Dot(0.0)
{
}

void ScenarioqBIType::load(const QDomElement& e)
{
    ScenarioAbstract6DOFAttitudeType::load(e);
    m_q1 = parseDouble(e.firstChildElement("q1").text());
    m_q2 = parseDouble(e.firstChildElement("q2").text());
    m_q3 = parseDouble(e.firstChildElement("q3").text());
    m_q4 = parseDouble(e.firstChildElement("q4").text());
    m_q1Dot = parseDouble(e.firstChildElement("q1Dot").text());
    m_q2Dot = parseDouble(e.firstChildElement("q2Dot").text());
    m_q3Dot = parseDouble(e.firstChildElement("q3Dot").text());
    m_q4Dot = parseDouble(e.firstChildElement("q4Dot").text());
}

QDomElement ScenarioqBIType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract6DOFAttitudeType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("q1");
        child.appendChild(doc.createTextNode(convertToString(m_q1);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q2");
        child.appendChild(doc.createTextNode(convertToString(m_q2);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q3");
        child.appendChild(doc.createTextNode(convertToString(m_q3);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q4");
        child.appendChild(doc.createTextNode(convertToString(m_q4);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q1Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q1Dot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q2Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q2Dot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q3Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q3Dot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("q4Dot");
        child.appendChild(doc.createTextNode(convertToString(m_q4Dot);
        e.appendChild(child);
    }
}




// ScenarioAbstract12DOFStateType
ScenarioAbstract12DOFStateType::ScenarioAbstract12DOFStateType()
{
}

void ScenarioAbstract12DOFStateType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioAbstract12DOFStateType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioState12DOF
ScenarioState12DOF::ScenarioState12DOF()
{
}

void ScenarioState12DOF::load(const QDomElement& e)
{
    ScenarioAbstract12DOFStateType::load(e);
    m_Abstract6DOFPosition = QSharedPointer<ScenarioAbstract6DOFPositionType>(new ScenarioAbstract6DOFPositionType());
    m_Abstract6DOFPosition->load(e.firstChildElement("Abstract6DOFPosition"));
    m_Abstract6DOFAttitude = QSharedPointer<ScenarioAbstract6DOFAttitudeType>(new ScenarioAbstract6DOFAttitudeType());
    m_Abstract6DOFAttitude->load(e.firstChildElement("Abstract6DOFAttitude"));
}

QDomElement ScenarioState12DOF::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstract12DOFStateType::toDomElement(doc);
    if (!m_Abstract6DOFPosition.isNull())
    {
        QDomElement child = m_Abstract6DOFPosition->toDomElement(doc);
        child.setTagName("Abstract6DOFPosition");
        e.appendChild(child);
    }
    if (!m_Abstract6DOFAttitude.isNull())
    {
        QDomElement child = m_Abstract6DOFAttitude->toDomElement(doc);
        child.setTagName("Abstract6DOFAttitude");
        e.appendChild(child);
    }
}




// ScenarioOptimization
ScenarioOptimization::ScenarioOptimization() :
    m_nVariables(0),
    m_nObjectives(0),
    m_nConstraints(0)
{
}

void ScenarioOptimization::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_nVariables = parseInt(e.firstChildElement("nVariables").text());
    m_variables = parseStringList(e.firstChildElement("variables").text());
    m_variablesMatrix = parseDoubleList(e.firstChildElement("variablesMatrix").text());
    m_nObjectives = parseInt(e.firstChildElement("nObjectives").text());
    m_objectives = parseStringList(e.firstChildElement("objectives").text());
    m_objectivesMatrix = parseDoubleList(e.firstChildElement("objectivesMatrix").text());
    m_nConstraints = parseInt(e.firstChildElement("nConstraints").text());
    m_constraints = parseStringList(e.firstChildElement("constraints").text());
    m_constraintsMatrix = parseDoubleList(e.firstChildElement("constraintsMatrix").text());
    m_algorithm = (e.firstChildElement("algorithm").text());
    m_algorithmParameters = parseDoubleList(e.firstChildElement("algorithmParameters").text());
}

QDomElement ScenarioOptimization::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("nVariables");
        child.appendChild(doc.createTextNode(convertToString(m_nVariables);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("variables");
        child.appendChild(doc.createTextNode(convertToString(m_variables);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("variablesMatrix");
        child.appendChild(doc.createTextNode(convertToString(m_variablesMatrix);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nObjectives");
        child.appendChild(doc.createTextNode(convertToString(m_nObjectives);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("objectives");
        child.appendChild(doc.createTextNode(convertToString(m_objectives);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("objectivesMatrix");
        child.appendChild(doc.createTextNode(convertToString(m_objectivesMatrix);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nConstraints");
        child.appendChild(doc.createTextNode(convertToString(m_nConstraints);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("constraints");
        child.appendChild(doc.createTextNode(convertToString(m_constraints);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("constraintsMatrix");
        child.appendChild(doc.createTextNode(convertToString(m_constraintsMatrix);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("algorithm");
        child.appendChild(doc.createTextNode(convertToString(m_algorithm);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("algorithmParameters");
        child.appendChild(doc.createTextNode(convertToString(m_algorithmParameters);
        e.appendChild(child);
    }
}




// ScenarioOutputFiles
ScenarioOutputFiles::ScenarioOutputFiles()
{
}

void ScenarioOutputFiles::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_paretoFrontVarsFileName = (e.firstChildElement("paretoFrontVarsFileName").text());
    m_paretoFrontCstrsFileName = (e.firstChildElement("paretoFrontCstrsFileName").text());
    m_paretoFrontObjsFileName = (e.firstChildElement("paretoFrontObjsFileName").text());
    m_geometryFileName = (e.firstChildElement("geometryFileName").text());
    m_trajPostionFileName = (e.firstChildElement("trajPostionFileName").text());
    m_trajAttitudeFileName = (e.firstChildElement("trajAttitudeFileName").text());
    m_trajMiscFileName = (e.firstChildElement("trajMiscFileName").text());
    m_successProbabilityFileName = (e.firstChildElement("successProbabilityFileName").text());
}

QDomElement ScenarioOutputFiles::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("paretoFrontVarsFileName");
        child.appendChild(doc.createTextNode(convertToString(m_paretoFrontVarsFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("paretoFrontCstrsFileName");
        child.appendChild(doc.createTextNode(convertToString(m_paretoFrontCstrsFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("paretoFrontObjsFileName");
        child.appendChild(doc.createTextNode(convertToString(m_paretoFrontObjsFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("geometryFileName");
        child.appendChild(doc.createTextNode(convertToString(m_geometryFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("trajPostionFileName");
        child.appendChild(doc.createTextNode(convertToString(m_trajPostionFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("trajAttitudeFileName");
        child.appendChild(doc.createTextNode(convertToString(m_trajAttitudeFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("trajMiscFileName");
        child.appendChild(doc.createTextNode(convertToString(m_trajMiscFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("successProbabilityFileName");
        child.appendChild(doc.createTextNode(convertToString(m_successProbabilityFileName);
        e.appendChild(child);
    }
}




// ScenarioAbstractConeType
ScenarioAbstractConeType::ScenarioAbstractConeType()
{
}

void ScenarioAbstractConeType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioAbstractConeType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioCircularCone
ScenarioCircularCone::ScenarioCircularCone() :
    m_coneAngle(0.0)
{
}

void ScenarioCircularCone::load(const QDomElement& e)
{
    ScenarioAbstractConeType::load(e);
    m_coneAngle = parseDouble(e.firstChildElement("coneAngle").text());
}

QDomElement ScenarioCircularCone::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractConeType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("coneAngle");
        child.appendChild(doc.createTextNode(convertToString(m_coneAngle);
        e.appendChild(child);
    }
}




// ScenarioRectangularConeType
ScenarioRectangularConeType::ScenarioRectangularConeType() :
    m_coneAngle1(0.0),
    m_coneAngle2(0.0)
{
}

void ScenarioRectangularConeType::load(const QDomElement& e)
{
    ScenarioAbstractConeType::load(e);
    m_coneAngle1 = parseDouble(e.firstChildElement("coneAngle1").text());
    m_coneAngle2 = parseDouble(e.firstChildElement("coneAngle2").text());
}

QDomElement ScenarioRectangularConeType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractConeType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("coneAngle1");
        child.appendChild(doc.createTextNode(convertToString(m_coneAngle1);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("coneAngle2");
        child.appendChild(doc.createTextNode(convertToString(m_coneAngle2);
        e.appendChild(child);
    }
}




// ScenarioOvalConeType
ScenarioOvalConeType::ScenarioOvalConeType()
{
}

void ScenarioOvalConeType::load(const QDomElement& e)
{
    ScenarioAbstractConeType::load(e);
}

QDomElement ScenarioOvalConeType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractConeType::toDomElement(doc);
}




// ScenarioAntenna
ScenarioAntenna::ScenarioAntenna()
{
}

void ScenarioAntenna::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_AbstractCone = QSharedPointer<ScenarioAbstractConeType>(new ScenarioAbstractConeType());
    m_AbstractCone->load(e.firstChildElement("AbstractCone"));
    m_PointingDirection = QSharedPointer<ScenarioPointingDirection>(new ScenarioPointingDirection());
    m_PointingDirection->load(e.firstChildElement("PointingDirection"));
}

QDomElement ScenarioAntenna::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_AbstractCone.isNull())
    {
        QDomElement child = m_AbstractCone->toDomElement(doc);
        child.setTagName("AbstractCone");
        e.appendChild(child);
    }
    if (!m_PointingDirection.isNull())
    {
        QDomElement child = m_PointingDirection->toDomElement(doc);
        child.setTagName("PointingDirection");
        e.appendChild(child);
    }
}




// ScenarioPointingDirection
ScenarioPointingDirection::ScenarioPointingDirection() :
    m_azimuth(0.0),
    m_elevation(0.0),
    m_azimuthDot(0.0),
    m_elevationDot(0.0)
{
}

void ScenarioPointingDirection::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_referenceSystem = (e.firstChildElement("referenceSystem").text());
    m_azimuth = parseDouble(e.firstChildElement("azimuth").text());
    m_elevation = parseDouble(e.firstChildElement("elevation").text());
    m_azimuthDot = parseDouble(e.firstChildElement("azimuthDot").text());
    m_elevationDot = parseDouble(e.firstChildElement("elevationDot").text());
}

QDomElement ScenarioPointingDirection::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("referenceSystem");
        child.appendChild(doc.createTextNode(convertToString(m_referenceSystem);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("azimuth");
        child.appendChild(doc.createTextNode(convertToString(m_azimuth);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("elevation");
        child.appendChild(doc.createTextNode(convertToString(m_elevation);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("azimuthDot");
        child.appendChild(doc.createTextNode(convertToString(m_azimuthDot);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("elevationDot");
        child.appendChild(doc.createTextNode(convertToString(m_elevationDot);
        e.appendChild(child);
    }
}




// ScenarioCommunicationAntenna
ScenarioCommunicationAntenna::ScenarioCommunicationAntenna()
{
}

void ScenarioCommunicationAntenna::load(const QDomElement& e)
{
    ScenarioAntenna::load(e);
}

QDomElement ScenarioCommunicationAntenna::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAntenna::toDomElement(doc);
}




// ScenarioObservationAntenna
ScenarioObservationAntenna::ScenarioObservationAntenna()
{
}

void ScenarioObservationAntenna::load(const QDomElement& e)
{
    ScenarioAntenna::load(e);
}

QDomElement ScenarioObservationAntenna::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAntenna::toDomElement(doc);
}




// ScenarioLocationType
ScenarioLocationType::ScenarioLocationType()
{
}

void ScenarioLocationType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_CentralBody = (e.firstChildElement("CentralBody").text());
    m_Abstract3DOFPosition = QSharedPointer<ScenarioAbstract3DOFPositionType>(new ScenarioAbstract3DOFPositionType());
    m_Abstract3DOFPosition->load(e.firstChildElement("Abstract3DOFPosition"));
}

QDomElement ScenarioLocationType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("CentralBody");
        child.appendChild(doc.createTextNode(convertToString(m_CentralBody);
        e.appendChild(child);
    }
    if (!m_Abstract3DOFPosition.isNull())
    {
        QDomElement child = m_Abstract3DOFPosition->toDomElement(doc);
        child.setTagName("Abstract3DOFPosition");
        e.appendChild(child);
    }
}




// ScenarioGroundStation
ScenarioGroundStation::ScenarioGroundStation()
{
}

void ScenarioGroundStation::load(const QDomElement& e)
{
    ScenarioParticipantsType::load(e);
    m_Location = QSharedPointer<ScenarioLocationType>(new ScenarioLocationType());
    m_Location->load(e.firstChildElement("Location"));
    m_CommunicationAntenna = QSharedPointer<ScenarioCommunicationAntenna>(new ScenarioCommunicationAntenna());
    m_CommunicationAntenna->load(e.firstChildElement("CommunicationAntenna"));
}

QDomElement ScenarioGroundStation::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioParticipantsType::toDomElement(doc);
    if (!m_Location.isNull())
    {
        QDomElement child = m_Location->toDomElement(doc);
        child.setTagName("Location");
        e.appendChild(child);
    }
    if (!m_CommunicationAntenna.isNull())
    {
        QDomElement child = m_CommunicationAntenna->toDomElement(doc);
        child.setTagName("CommunicationAntenna");
        e.appendChild(child);
    }
}




// ScenarioLaunchPad
ScenarioLaunchPad::ScenarioLaunchPad() :
    m_clearingAltitude(0.0)
{
}

void ScenarioLaunchPad::load(const QDomElement& e)
{
    ScenarioParticipantsType::load(e);
    m_Location = QSharedPointer<ScenarioLocationType>(new ScenarioLocationType());
    m_Location->load(e.firstChildElement("Location"));
    m_clearingAltitude = parseDouble(e.firstChildElement("clearingAltitude").text());
}

QDomElement ScenarioLaunchPad::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioParticipantsType::toDomElement(doc);
    if (!m_Location.isNull())
    {
        QDomElement child = m_Location->toDomElement(doc);
        child.setTagName("Location");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("clearingAltitude");
        child.appendChild(doc.createTextNode(convertToString(m_clearingAltitude);
        e.appendChild(child);
    }
}




// ScenarioPoint
ScenarioPoint::ScenarioPoint()
{
}

void ScenarioPoint::load(const QDomElement& e)
{
    ScenarioParticipantsType::load(e);
    m_Location = QSharedPointer<ScenarioLocation>(new ScenarioLocation());
    m_Location->load(e.firstChildElement("Location"));
}

QDomElement ScenarioPoint::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioParticipantsType::toDomElement(doc);
    if (!m_Location.isNull())
    {
        QDomElement child = m_Location->toDomElement(doc);
        child.setTagName("Location");
        e.appendChild(child);
    }
}




// ScenarioLocation
ScenarioLocation::ScenarioLocation()
{
}

void ScenarioLocation::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioLocation::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioLV
ScenarioLV::ScenarioLV()
{
}

void ScenarioLV::load(const QDomElement& e)
{
    ScenarioParticipantsType::load(e);
    m_LVProgram = QSharedPointer<ScenarioLVProgram>(new ScenarioLVProgram());
    m_LVProgram->load(e.firstChildElement("LVProgram"));
    m_LVMission = QSharedPointer<ScenarioLVMission>(new ScenarioLVMission());
    m_LVMission->load(e.firstChildElement("LVMission"));
    m_System = QSharedPointer<ScenarioLVSystemType>(new ScenarioLVSystemType());
    m_System->load(e.firstChildElement("System"));
    m_Optimization = QSharedPointer<ScenarioOptimization>(new ScenarioOptimization());
    m_Optimization->load(e.firstChildElement("Optimization"));
    m_OutputFiles = QSharedPointer<ScenarioOutputFiles>(new ScenarioOutputFiles());
    m_OutputFiles->load(e.firstChildElement("OutputFiles"));
    m_appearance = (e.firstChildElement("appearance").text());
}

QDomElement ScenarioLV::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioParticipantsType::toDomElement(doc);
    if (!m_LVProgram.isNull())
    {
        QDomElement child = m_LVProgram->toDomElement(doc);
        child.setTagName("LVProgram");
        e.appendChild(child);
    }
    if (!m_LVMission.isNull())
    {
        QDomElement child = m_LVMission->toDomElement(doc);
        child.setTagName("LVMission");
        e.appendChild(child);
    }
    if (!m_System.isNull())
    {
        QDomElement child = m_System->toDomElement(doc);
        child.setTagName("System");
        e.appendChild(child);
    }
    if (!m_Optimization.isNull())
    {
        QDomElement child = m_Optimization->toDomElement(doc);
        child.setTagName("Optimization");
        e.appendChild(child);
    }
    if (!m_OutputFiles.isNull())
    {
        QDomElement child = m_OutputFiles->toDomElement(doc);
        child.setTagName("OutputFiles");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("appearance");
        child.appendChild(doc.createTextNode(convertToString(m_appearance);
        e.appendChild(child);
    }
}




// ScenarioPropulsionSystem
ScenarioPropulsionSystem::ScenarioPropulsionSystem()
{
}

void ScenarioPropulsionSystem::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_System = QSharedPointer<ScenarioSystem>(new ScenarioSystem());
    m_System->load(e.firstChildElement("System"));
    m_LiquidTanks = QSharedPointer<ScenarioLiquidTanks>(new ScenarioLiquidTanks());
    m_LiquidTanks->load(e.firstChildElement("LiquidTanks"));
    m_SolidGrain = QSharedPointer<ScenarioSolidGrain>(new ScenarioSolidGrain());
    m_SolidGrain->load(e.firstChildElement("SolidGrain"));
    m_FeedSystem = QSharedPointer<ScenarioFeedSystem>(new ScenarioFeedSystem());
    m_FeedSystem->load(e.firstChildElement("FeedSystem"));
    m_CombustionChamber = QSharedPointer<ScenarioCombustionChamber>(new ScenarioCombustionChamber());
    m_CombustionChamber->load(e.firstChildElement("CombustionChamber"));
    m_Nozzle = QSharedPointer<ScenarioNozzle>(new ScenarioNozzle());
    m_Nozzle->load(e.firstChildElement("Nozzle"));
    m_Performance = QSharedPointer<ScenarioPerformance>(new ScenarioPerformance());
    m_Performance->load(e.firstChildElement("Performance"));
}

QDomElement ScenarioPropulsionSystem::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_System.isNull())
    {
        QDomElement child = m_System->toDomElement(doc);
        child.setTagName("System");
        e.appendChild(child);
    }
    if (!m_LiquidTanks.isNull())
    {
        QDomElement child = m_LiquidTanks->toDomElement(doc);
        child.setTagName("LiquidTanks");
        e.appendChild(child);
    }
    if (!m_SolidGrain.isNull())
    {
        QDomElement child = m_SolidGrain->toDomElement(doc);
        child.setTagName("SolidGrain");
        e.appendChild(child);
    }
    if (!m_FeedSystem.isNull())
    {
        QDomElement child = m_FeedSystem->toDomElement(doc);
        child.setTagName("FeedSystem");
        e.appendChild(child);
    }
    if (!m_CombustionChamber.isNull())
    {
        QDomElement child = m_CombustionChamber->toDomElement(doc);
        child.setTagName("CombustionChamber");
        e.appendChild(child);
    }
    if (!m_Nozzle.isNull())
    {
        QDomElement child = m_Nozzle->toDomElement(doc);
        child.setTagName("Nozzle");
        e.appendChild(child);
    }
    if (!m_Performance.isNull())
    {
        QDomElement child = m_Performance->toDomElement(doc);
        child.setTagName("Performance");
        e.appendChild(child);
    }
}




// ScenarioSystem
ScenarioSystem::ScenarioSystem() :
    m_totalEngineDryMass(0.0),
    m_engineCoGLongPosition(0.0)
{
}

void ScenarioSystem::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_propType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_propType->load(e.firstChildElement("propType"));
    m_fuelType = (e.firstChildElement("fuelType").text());
    m_oxType = (e.firstChildElement("oxType").text());
    m_numberOfEngines = QSharedPointer<ScenarioOptVarInt>(new ScenarioOptVarInt());
    m_numberOfEngines->load(e.firstChildElement("numberOfEngines"));
    m_nominalThrust = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_nominalThrust->load(e.firstChildElement("nominalThrust"));
    m_totalEngineDryMass = parseDouble(e.firstChildElement("totalEngineDryMass").text());
    m_propMass = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_propMass->load(e.firstChildElement("propMass"));
    m_engineCoGLongPosition = parseDouble(e.firstChildElement("engineCoGLongPosition").text());
    m_lengthOverDiam = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_lengthOverDiam->load(e.firstChildElement("lengthOverDiam"));
    m_offTheShelf = QSharedPointer<ScenarioOptVarBool>(new ScenarioOptVarBool());
    m_offTheShelf->load(e.firstChildElement("offTheShelf"));
    m_offTheShelfEngineType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_offTheShelfEngineType->load(e.firstChildElement("offTheShelfEngineType"));
    m_feedType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_feedType->load(e.firstChildElement("feedType"));
    m_coolingType = (e.firstChildElement("coolingType").text());
    m_mixtureRatio = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_mixtureRatio->load(e.firstChildElement("mixtureRatio"));
    m_nozzleOptAlt = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_nozzleOptAlt->load(e.firstChildElement("nozzleOptAlt"));
}

QDomElement ScenarioSystem::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_propType.isNull())
    {
        QDomElement child = m_propType->toDomElement(doc);
        child.setTagName("propType");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fuelType");
        child.appendChild(doc.createTextNode(convertToString(m_fuelType);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("oxType");
        child.appendChild(doc.createTextNode(convertToString(m_oxType);
        e.appendChild(child);
    }
    if (!m_numberOfEngines.isNull())
    {
        QDomElement child = m_numberOfEngines->toDomElement(doc);
        child.setTagName("numberOfEngines");
        e.appendChild(child);
    }
    if (!m_nominalThrust.isNull())
    {
        QDomElement child = m_nominalThrust->toDomElement(doc);
        child.setTagName("nominalThrust");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalEngineDryMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalEngineDryMass);
        e.appendChild(child);
    }
    if (!m_propMass.isNull())
    {
        QDomElement child = m_propMass->toDomElement(doc);
        child.setTagName("propMass");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("engineCoGLongPosition");
        child.appendChild(doc.createTextNode(convertToString(m_engineCoGLongPosition);
        e.appendChild(child);
    }
    if (!m_lengthOverDiam.isNull())
    {
        QDomElement child = m_lengthOverDiam->toDomElement(doc);
        child.setTagName("lengthOverDiam");
        e.appendChild(child);
    }
    if (!m_offTheShelf.isNull())
    {
        QDomElement child = m_offTheShelf->toDomElement(doc);
        child.setTagName("offTheShelf");
        e.appendChild(child);
    }
    if (!m_offTheShelfEngineType.isNull())
    {
        QDomElement child = m_offTheShelfEngineType->toDomElement(doc);
        child.setTagName("offTheShelfEngineType");
        e.appendChild(child);
    }
    if (!m_feedType.isNull())
    {
        QDomElement child = m_feedType->toDomElement(doc);
        child.setTagName("feedType");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("coolingType");
        child.appendChild(doc.createTextNode(convertToString(m_coolingType);
        e.appendChild(child);
    }
    if (!m_mixtureRatio.isNull())
    {
        QDomElement child = m_mixtureRatio->toDomElement(doc);
        child.setTagName("mixtureRatio");
        e.appendChild(child);
    }
    if (!m_nozzleOptAlt.isNull())
    {
        QDomElement child = m_nozzleOptAlt->toDomElement(doc);
        child.setTagName("nozzleOptAlt");
        e.appendChild(child);
    }
}




// ScenarioLiquidTanks
ScenarioLiquidTanks::ScenarioLiquidTanks() :
    m_oxTankLength(0.0),
    m_oxTankDiam(0.0),
    m_oxTankPressure(0.0),
    m_fuelTankLength(0.0),
    m_fuelTankDiam(0.0),
    m_fuelTankPressure(0.0)
{
}

void ScenarioLiquidTanks::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_tanksPressureNorm = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_tanksPressureNorm->load(e.firstChildElement("tanksPressureNorm"));
    m_oxTankLength = parseDouble(e.firstChildElement("oxTankLength").text());
    m_oxTankDiam = parseDouble(e.firstChildElement("oxTankDiam").text());
    m_oxTankPressure = parseDouble(e.firstChildElement("oxTankPressure").text());
    m_fuelTankLength = parseDouble(e.firstChildElement("fuelTankLength").text());
    m_fuelTankDiam = parseDouble(e.firstChildElement("fuelTankDiam").text());
    m_fuelTankPressure = parseDouble(e.firstChildElement("fuelTankPressure").text());
}

QDomElement ScenarioLiquidTanks::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_tanksPressureNorm.isNull())
    {
        QDomElement child = m_tanksPressureNorm->toDomElement(doc);
        child.setTagName("tanksPressureNorm");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("oxTankLength");
        child.appendChild(doc.createTextNode(convertToString(m_oxTankLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("oxTankDiam");
        child.appendChild(doc.createTextNode(convertToString(m_oxTankDiam);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("oxTankPressure");
        child.appendChild(doc.createTextNode(convertToString(m_oxTankPressure);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fuelTankLength");
        child.appendChild(doc.createTextNode(convertToString(m_fuelTankLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fuelTankDiam");
        child.appendChild(doc.createTextNode(convertToString(m_fuelTankDiam);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fuelTankPressure");
        child.appendChild(doc.createTextNode(convertToString(m_fuelTankPressure);
        e.appendChild(child);
    }
}




// ScenarioSolidGrain
ScenarioSolidGrain::ScenarioSolidGrain() :
    m_grainLength(0.0),
    m_grainDiam(0.0),
    m_igniterLength(0.0),
    m_igniterDiam(0.0),
    m_igniterMass(0.0),
    m_sliverMass(0.0),
    m_cavityVolume(0.0)
{
}

void ScenarioSolidGrain::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_grainLength = parseDouble(e.firstChildElement("grainLength").text());
    m_grainDiam = parseDouble(e.firstChildElement("grainDiam").text());
    m_igniterLength = parseDouble(e.firstChildElement("igniterLength").text());
    m_igniterDiam = parseDouble(e.firstChildElement("igniterDiam").text());
    m_igniterMass = parseDouble(e.firstChildElement("igniterMass").text());
    m_sliverMass = parseDouble(e.firstChildElement("sliverMass").text());
    m_cavityVolume = parseDouble(e.firstChildElement("cavityVolume").text());
}

QDomElement ScenarioSolidGrain::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("grainLength");
        child.appendChild(doc.createTextNode(convertToString(m_grainLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("grainDiam");
        child.appendChild(doc.createTextNode(convertToString(m_grainDiam);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("igniterLength");
        child.appendChild(doc.createTextNode(convertToString(m_igniterLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("igniterDiam");
        child.appendChild(doc.createTextNode(convertToString(m_igniterDiam);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("igniterMass");
        child.appendChild(doc.createTextNode(convertToString(m_igniterMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("sliverMass");
        child.appendChild(doc.createTextNode(convertToString(m_sliverMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("cavityVolume");
        child.appendChild(doc.createTextNode(convertToString(m_cavityVolume);
        e.appendChild(child);
    }
}




// ScenarioFeedSystem
ScenarioFeedSystem::ScenarioFeedSystem() :
    m_pressurizerTankPressure(0.0),
    m_pressurizerTankRadius(0.0),
    m_pressurizerTankMass(0.0),
    m_pressurizerGasMass(0.0),
    m_linesValvesMass(0.0),
    m_trappedPropMass(0.0),
    m_totalFeedSystemMass(0.0),
    m_totalFeedSystemLength(0.0),
    m_totalFeedSystemDiameter(0.0)
{
}

void ScenarioFeedSystem::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_pressurizerTankPressure = parseDouble(e.firstChildElement("pressurizerTankPressure").text());
    m_pressurizerTankRadius = parseDouble(e.firstChildElement("pressurizerTankRadius").text());
    m_pressurizerTankMass = parseDouble(e.firstChildElement("pressurizerTankMass").text());
    m_pressurizerGasMass = parseDouble(e.firstChildElement("pressurizerGasMass").text());
    m_linesValvesMass = parseDouble(e.firstChildElement("linesValvesMass").text());
    m_trappedPropMass = parseDouble(e.firstChildElement("trappedPropMass").text());
    m_totalFeedSystemMass = parseDouble(e.firstChildElement("totalFeedSystemMass").text());
    m_totalFeedSystemLength = parseDouble(e.firstChildElement("totalFeedSystemLength").text());
    m_totalFeedSystemDiameter = parseDouble(e.firstChildElement("totalFeedSystemDiameter").text());
}

QDomElement ScenarioFeedSystem::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("pressurizerTankPressure");
        child.appendChild(doc.createTextNode(convertToString(m_pressurizerTankPressure);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("pressurizerTankRadius");
        child.appendChild(doc.createTextNode(convertToString(m_pressurizerTankRadius);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("pressurizerTankMass");
        child.appendChild(doc.createTextNode(convertToString(m_pressurizerTankMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("pressurizerGasMass");
        child.appendChild(doc.createTextNode(convertToString(m_pressurizerGasMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("linesValvesMass");
        child.appendChild(doc.createTextNode(convertToString(m_linesValvesMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("trappedPropMass");
        child.appendChild(doc.createTextNode(convertToString(m_trappedPropMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalFeedSystemMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalFeedSystemMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalFeedSystemLength");
        child.appendChild(doc.createTextNode(convertToString(m_totalFeedSystemLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalFeedSystemDiameter");
        child.appendChild(doc.createTextNode(convertToString(m_totalFeedSystemDiameter);
        e.appendChild(child);
    }
}




// ScenarioCombustionChamber
ScenarioCombustionChamber::ScenarioCombustionChamber() :
    m_ab(0.0),
    m_acc(0.0),
    m_chamberPressure(0.0),
    m_chamberCharactLength(0.0),
    m_chamberLength(0.0),
    m_chamberDiameter(0.0),
    m_chamberMass(0.0)
{
}

void ScenarioCombustionChamber::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_abOverAt = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_abOverAt->load(e.firstChildElement("abOverAt"));
    m_accOverAt = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_accOverAt->load(e.firstChildElement("accOverAt"));
    m_ab = parseDouble(e.firstChildElement("ab").text());
    m_acc = parseDouble(e.firstChildElement("acc").text());
    m_chamberPressureNorm = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_chamberPressureNorm->load(e.firstChildElement("chamberPressureNorm"));
    m_chamberPressure = parseDouble(e.firstChildElement("chamberPressure").text());
    m_chamberCharactLength = parseDouble(e.firstChildElement("chamberCharactLength").text());
    m_chamberLength = parseDouble(e.firstChildElement("chamberLength").text());
    m_chamberDiameter = parseDouble(e.firstChildElement("chamberDiameter").text());
    m_chamberMass = parseDouble(e.firstChildElement("chamberMass").text());
}

QDomElement ScenarioCombustionChamber::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_abOverAt.isNull())
    {
        QDomElement child = m_abOverAt->toDomElement(doc);
        child.setTagName("abOverAt");
        e.appendChild(child);
    }
    if (!m_accOverAt.isNull())
    {
        QDomElement child = m_accOverAt->toDomElement(doc);
        child.setTagName("accOverAt");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("ab");
        child.appendChild(doc.createTextNode(convertToString(m_ab);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("acc");
        child.appendChild(doc.createTextNode(convertToString(m_acc);
        e.appendChild(child);
    }
    if (!m_chamberPressureNorm.isNull())
    {
        QDomElement child = m_chamberPressureNorm->toDomElement(doc);
        child.setTagName("chamberPressureNorm");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("chamberPressure");
        child.appendChild(doc.createTextNode(convertToString(m_chamberPressure);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("chamberCharactLength");
        child.appendChild(doc.createTextNode(convertToString(m_chamberCharactLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("chamberLength");
        child.appendChild(doc.createTextNode(convertToString(m_chamberLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("chamberDiameter");
        child.appendChild(doc.createTextNode(convertToString(m_chamberDiameter);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("chamberMass");
        child.appendChild(doc.createTextNode(convertToString(m_chamberMass);
        e.appendChild(child);
    }
}




// ScenarioNozzle
ScenarioNozzle::ScenarioNozzle() :
    m_at(0.0),
    m_ae(0.0),
    m_aeOverat(0.0),
    m_nozzleLength(0.0),
    m_nozzleDiameter(0.0),
    m_nozzleMass(0.0),
    m_tvcMass(0.0),
    m_nozzleCantAngle(0.0)
{
}

void ScenarioNozzle::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_at = parseDouble(e.firstChildElement("at").text());
    m_ae = parseDouble(e.firstChildElement("ae").text());
    m_aeOverat = parseDouble(e.firstChildElement("aeOverat").text());
    m_nozzleLength = parseDouble(e.firstChildElement("nozzleLength").text());
    m_nozzleDiameter = parseDouble(e.firstChildElement("nozzleDiameter").text());
    m_nozzleMass = parseDouble(e.firstChildElement("nozzleMass").text());
    m_nozzleType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_nozzleType->load(e.firstChildElement("nozzleType"));
    m_divAngle = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_divAngle->load(e.firstChildElement("divAngle"));
    m_tvcType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_tvcType->load(e.firstChildElement("tvcType"));
    m_tvcAngle = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_tvcAngle->load(e.firstChildElement("tvcAngle"));
    m_tvcMass = parseDouble(e.firstChildElement("tvcMass").text());
    m_nozzleCantAngle = parseDouble(e.firstChildElement("nozzleCantAngle").text());
    m_nozzleShape = (e.firstChildElement("nozzleShape").text());
}

QDomElement ScenarioNozzle::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("at");
        child.appendChild(doc.createTextNode(convertToString(m_at);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("ae");
        child.appendChild(doc.createTextNode(convertToString(m_ae);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("aeOverat");
        child.appendChild(doc.createTextNode(convertToString(m_aeOverat);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nozzleLength");
        child.appendChild(doc.createTextNode(convertToString(m_nozzleLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nozzleDiameter");
        child.appendChild(doc.createTextNode(convertToString(m_nozzleDiameter);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nozzleMass");
        child.appendChild(doc.createTextNode(convertToString(m_nozzleMass);
        e.appendChild(child);
    }
    if (!m_nozzleType.isNull())
    {
        QDomElement child = m_nozzleType->toDomElement(doc);
        child.setTagName("nozzleType");
        e.appendChild(child);
    }
    if (!m_divAngle.isNull())
    {
        QDomElement child = m_divAngle->toDomElement(doc);
        child.setTagName("divAngle");
        e.appendChild(child);
    }
    if (!m_tvcType.isNull())
    {
        QDomElement child = m_tvcType->toDomElement(doc);
        child.setTagName("tvcType");
        e.appendChild(child);
    }
    if (!m_tvcAngle.isNull())
    {
        QDomElement child = m_tvcAngle->toDomElement(doc);
        child.setTagName("tvcAngle");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("tvcMass");
        child.appendChild(doc.createTextNode(convertToString(m_tvcMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nozzleCantAngle");
        child.appendChild(doc.createTextNode(convertToString(m_nozzleCantAngle);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nozzleShape");
        child.appendChild(doc.createTextNode(convertToString(m_nozzleShape);
        e.appendChild(child);
    }
}




// ScenarioPerformance
ScenarioPerformance::ScenarioPerformance() :
    m_theoOptIsp(0.0),
    m_realOptIsp(0.0),
    m_realSeeIsp(0.0),
    m_realVacuumIsp(0.0),
    m_thrustCoeff(0.0),
    m_charactVelocity(0.0),
    m_thrustCoeffEff(0.0),
    m_charactVelocityEff(0.0),
    m_nominalMassFlow(0.0),
    m_minOperativeAlt(0.0)
{
}

void ScenarioPerformance::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_theoOptIsp = parseDouble(e.firstChildElement("theoOptIsp").text());
    m_realOptIsp = parseDouble(e.firstChildElement("realOptIsp").text());
    m_realSeeIsp = parseDouble(e.firstChildElement("realSeeIsp").text());
    m_realVacuumIsp = parseDouble(e.firstChildElement("realVacuumIsp").text());
    m_thrustCoeff = parseDouble(e.firstChildElement("thrustCoeff").text());
    m_charactVelocity = parseDouble(e.firstChildElement("charactVelocity").text());
    m_thrustCoeffEff = parseDouble(e.firstChildElement("thrustCoeffEff").text());
    m_charactVelocityEff = parseDouble(e.firstChildElement("charactVelocityEff").text());
    m_nominalMassFlow = parseDouble(e.firstChildElement("nominalMassFlow").text());
    m_minOperativeAlt = parseDouble(e.firstChildElement("minOperativeAlt").text());
    m_throttle = QSharedPointer<ScenarioOptVarBool>(new ScenarioOptVarBool());
    m_throttle->load(e.firstChildElement("throttle"));
    m_restart = QSharedPointer<ScenarioOptVarBool>(new ScenarioOptVarBool());
    m_restart->load(e.firstChildElement("restart"));
}

QDomElement ScenarioPerformance::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("theoOptIsp");
        child.appendChild(doc.createTextNode(convertToString(m_theoOptIsp);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("realOptIsp");
        child.appendChild(doc.createTextNode(convertToString(m_realOptIsp);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("realSeeIsp");
        child.appendChild(doc.createTextNode(convertToString(m_realSeeIsp);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("realVacuumIsp");
        child.appendChild(doc.createTextNode(convertToString(m_realVacuumIsp);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("thrustCoeff");
        child.appendChild(doc.createTextNode(convertToString(m_thrustCoeff);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("charactVelocity");
        child.appendChild(doc.createTextNode(convertToString(m_charactVelocity);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("thrustCoeffEff");
        child.appendChild(doc.createTextNode(convertToString(m_thrustCoeffEff);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("charactVelocityEff");
        child.appendChild(doc.createTextNode(convertToString(m_charactVelocityEff);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nominalMassFlow");
        child.appendChild(doc.createTextNode(convertToString(m_nominalMassFlow);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("minOperativeAlt");
        child.appendChild(doc.createTextNode(convertToString(m_minOperativeAlt);
        e.appendChild(child);
    }
    if (!m_throttle.isNull())
    {
        QDomElement child = m_throttle->toDomElement(doc);
        child.setTagName("throttle");
        e.appendChild(child);
    }
    if (!m_restart.isNull())
    {
        QDomElement child = m_restart->toDomElement(doc);
        child.setTagName("restart");
        e.appendChild(child);
    }
}




// ScenarioLVProgram
ScenarioLVProgram::ScenarioLVProgram() :
    m_nLaunches(0),
    m_nYearsOps(0.0),
    m_nLaunchesPerYear(0)
{
}

void ScenarioLVProgram::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_nLaunches = parseInt(e.firstChildElement("nLaunches").text());
    m_nYearsOps = parseDouble(e.firstChildElement("nYearsOps").text());
    m_nLaunchesPerYear = parseInt(e.firstChildElement("nLaunchesPerYear").text());
    m_programCostFactors = QSharedPointer<ScenarioprogramCostFactors>(new ScenarioprogramCostFactors());
    m_programCostFactors->load(e.firstChildElement("programCostFactors"));
}

QDomElement ScenarioLVProgram::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("nLaunches");
        child.appendChild(doc.createTextNode(convertToString(m_nLaunches);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nYearsOps");
        child.appendChild(doc.createTextNode(convertToString(m_nYearsOps);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nLaunchesPerYear");
        child.appendChild(doc.createTextNode(convertToString(m_nLaunchesPerYear);
        e.appendChild(child);
    }
    if (!m_programCostFactors.isNull())
    {
        QDomElement child = m_programCostFactors->toDomElement(doc);
        child.setTagName("programCostFactors");
        e.appendChild(child);
    }
}




// ScenarioprogramCostFactors
ScenarioprogramCostFactors::ScenarioprogramCostFactors() :
    m_f0(0.0),
    m_f1(0.0),
    m_f3(0.0),
    m_f7(0.0)
{
}

void ScenarioprogramCostFactors::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_f0 = parseDouble(e.firstChildElement("f0").text());
    m_launcherDesignComplexity = (e.firstChildElement("launcherDesignComplexity").text());
    m_f1 = parseDouble(e.firstChildElement("f1").text());
    m_teamExpertise = (e.firstChildElement("teamExpertise").text());
    m_f3 = parseDouble(e.firstChildElement("f3").text());
    m_contractorsStructure = (e.firstChildElement("contractorsStructure").text());
    m_f7 = parseDouble(e.firstChildElement("f7").text());
}

QDomElement ScenarioprogramCostFactors::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("f0");
        child.appendChild(doc.createTextNode(convertToString(m_f0);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("launcherDesignComplexity");
        child.appendChild(doc.createTextNode(convertToString(m_launcherDesignComplexity);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("f1");
        child.appendChild(doc.createTextNode(convertToString(m_f1);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("teamExpertise");
        child.appendChild(doc.createTextNode(convertToString(m_teamExpertise);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("f3");
        child.appendChild(doc.createTextNode(convertToString(m_f3);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("contractorsStructure");
        child.appendChild(doc.createTextNode(convertToString(m_contractorsStructure);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("f7");
        child.appendChild(doc.createTextNode(convertToString(m_f7);
        e.appendChild(child);
    }
}




// ScenarioLVMission
ScenarioLVMission::ScenarioLVMission()
{
}

void ScenarioLVMission::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_LVPayload = QSharedPointer<ScenarioLVPayload>(new ScenarioLVPayload());
    m_LVPayload->load(e.firstChildElement("LVPayload"));
    m_Environment = QSharedPointer<ScenarioEnvironmentType>(new ScenarioEnvironmentType());
    m_Environment->load(e.firstChildElement("Environment"));
    m_LaunchSite = QSharedPointer<ScenarioLaunchSite>(new ScenarioLaunchSite());
    m_LaunchSite->load(e.firstChildElement("LaunchSite"));
    m_TargetOrbit = QSharedPointer<ScenarioTargetOrbit>(new ScenarioTargetOrbit());
    m_TargetOrbit->load(e.firstChildElement("TargetOrbit"));
    m_Trajectory = QSharedPointer<ScenarioTrajectory>(new ScenarioTrajectory());
    m_Trajectory->load(e.firstChildElement("Trajectory"));
}

QDomElement ScenarioLVMission::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_LVPayload.isNull())
    {
        QDomElement child = m_LVPayload->toDomElement(doc);
        child.setTagName("LVPayload");
        e.appendChild(child);
    }
    if (!m_Environment.isNull())
    {
        QDomElement child = m_Environment->toDomElement(doc);
        child.setTagName("Environment");
        e.appendChild(child);
    }
    if (!m_LaunchSite.isNull())
    {
        QDomElement child = m_LaunchSite->toDomElement(doc);
        child.setTagName("LaunchSite");
        e.appendChild(child);
    }
    if (!m_TargetOrbit.isNull())
    {
        QDomElement child = m_TargetOrbit->toDomElement(doc);
        child.setTagName("TargetOrbit");
        e.appendChild(child);
    }
    if (!m_Trajectory.isNull())
    {
        QDomElement child = m_Trajectory->toDomElement(doc);
        child.setTagName("Trajectory");
        e.appendChild(child);
    }
}




// ScenarioLVPayload
ScenarioLVPayload::ScenarioLVPayload() :
    m_mass(0.0),
    m_length(0.0),
    m_diameter(0.0),
    m_CoGLongPosition(0.0),
    m_maxAxialAcc(0.0),
    m_maxHeatFlux(0.0)
{
}

void ScenarioLVPayload::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_mass = parseDouble(e.firstChildElement("mass").text());
    m_length = parseDouble(e.firstChildElement("length").text());
    m_diameter = parseDouble(e.firstChildElement("diameter").text());
    m_CoGLongPosition = parseDouble(e.firstChildElement("CoGLongPosition").text());
    m_maxAxialAcc = parseDouble(e.firstChildElement("maxAxialAcc").text());
    m_maxHeatFlux = parseDouble(e.firstChildElement("maxHeatFlux").text());
}

QDomElement ScenarioLVPayload::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("mass");
        child.appendChild(doc.createTextNode(convertToString(m_mass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("length");
        child.appendChild(doc.createTextNode(convertToString(m_length);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("diameter");
        child.appendChild(doc.createTextNode(convertToString(m_diameter);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("CoGLongPosition");
        child.appendChild(doc.createTextNode(convertToString(m_CoGLongPosition);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("maxAxialAcc");
        child.appendChild(doc.createTextNode(convertToString(m_maxAxialAcc);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("maxHeatFlux");
        child.appendChild(doc.createTextNode(convertToString(m_maxHeatFlux);
        e.appendChild(child);
    }
}




// ScenarioLaunchSite
ScenarioLaunchSite::ScenarioLaunchSite()
{
}

void ScenarioLaunchSite::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Abstract3DOFPosition = QSharedPointer<ScenarioAbstract3DOFPositionType>(new ScenarioAbstract3DOFPositionType());
    m_Abstract3DOFPosition->load(e.firstChildElement("Abstract3DOFPosition"));
    m_allowedAzimuths = parseDoubleList(e.firstChildElement("allowedAzimuths").text());
}

QDomElement ScenarioLaunchSite::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Abstract3DOFPosition.isNull())
    {
        QDomElement child = m_Abstract3DOFPosition->toDomElement(doc);
        child.setTagName("Abstract3DOFPosition");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("allowedAzimuths");
        child.appendChild(doc.createTextNode(convertToString(m_allowedAzimuths);
        e.appendChild(child);
    }
}




// ScenarioTargetOrbit
ScenarioTargetOrbit::ScenarioTargetOrbit() :
    m_semiaxis(0.0),
    m_eccentricity(0.0),
    m_inclination(0.0),
    m_semiaxisTol(0.0),
    m_eccentricityTol(0.0),
    m_inclinationTol(0.0)
{
}

void ScenarioTargetOrbit::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_semiaxis = parseDouble(e.firstChildElement("semiaxis").text());
    m_eccentricity = parseDouble(e.firstChildElement("eccentricity").text());
    m_inclination = parseDouble(e.firstChildElement("inclination").text());
    m_semiaxisTol = parseDouble(e.firstChildElement("semiaxisTol").text());
    m_eccentricityTol = parseDouble(e.firstChildElement("eccentricityTol").text());
    m_inclinationTol = parseDouble(e.firstChildElement("inclinationTol").text());
}

QDomElement ScenarioTargetOrbit::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("semiaxis");
        child.appendChild(doc.createTextNode(convertToString(m_semiaxis);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("eccentricity");
        child.appendChild(doc.createTextNode(convertToString(m_eccentricity);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("inclination");
        child.appendChild(doc.createTextNode(convertToString(m_inclination);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("semiaxisTol");
        child.appendChild(doc.createTextNode(convertToString(m_semiaxisTol);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("eccentricityTol");
        child.appendChild(doc.createTextNode(convertToString(m_eccentricityTol);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("inclinationTol");
        child.appendChild(doc.createTextNode(convertToString(m_inclinationTol);
        e.appendChild(child);
    }
}




// ScenarioLVSystemType
ScenarioLVSystemType::ScenarioLVSystemType()
{
}

void ScenarioLVSystemType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Architecture = QSharedPointer<ScenarioArchitecture>(new ScenarioArchitecture());
    m_Architecture->load(e.firstChildElement("Architecture"));
    {
        QDomElement f = e.firstChildElement("LowerStage");        while (!f.isNull())
        {
            QSharedPointer<ScenarioLowerStage> v = QSharedPointer<ScenarioLowerStage>(new ScenarioLowerStage());
            v->load(e.firstChildElement("LowerStage"));
            m_LowerStage << v;
        }
    m_UpperStage = QSharedPointer<ScenarioUpperStage>(new ScenarioUpperStage());
    m_UpperStage->load(e.firstChildElement("UpperStage"));
    {
        QDomElement f = e.firstChildElement("Boosters");        while (!f.isNull())
        {
            QSharedPointer<ScenarioBoosters> v = QSharedPointer<ScenarioBoosters>(new ScenarioBoosters());
            v->load(e.firstChildElement("Boosters"));
            m_Boosters << v;
        }
    m_SystemWeights = QSharedPointer<ScenarioSystemWeights>(new ScenarioSystemWeights());
    m_SystemWeights->load(e.firstChildElement("SystemWeights"));
    m_SystemCosts = QSharedPointer<ScenarioSystemCosts>(new ScenarioSystemCosts());
    m_SystemCosts->load(e.firstChildElement("SystemCosts"));
    m_SystemReliability = QSharedPointer<ScenarioSystemReliability>(new ScenarioSystemReliability());
    m_SystemReliability->load(e.firstChildElement("SystemReliability"));
    m_LVAerodynamics = QSharedPointer<ScenarioLVAerodynamics>(new ScenarioLVAerodynamics());
    m_LVAerodynamics->load(e.firstChildElement("LVAerodynamics"));
    m_DiscretizationSettings = QSharedPointer<ScenarioDiscretizationSettings>(new ScenarioDiscretizationSettings());
    m_DiscretizationSettings->load(e.firstChildElement("DiscretizationSettings"));
}

QDomElement ScenarioLVSystemType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Architecture.isNull())
    {
        QDomElement child = m_Architecture->toDomElement(doc);
        child.setTagName("Architecture");
        e.appendChild(child);
    }
    if (!m_UpperStage.isNull())
    {
        QDomElement child = m_UpperStage->toDomElement(doc);
        child.setTagName("UpperStage");
        e.appendChild(child);
    }
    if (!m_SystemWeights.isNull())
    {
        QDomElement child = m_SystemWeights->toDomElement(doc);
        child.setTagName("SystemWeights");
        e.appendChild(child);
    }
    if (!m_SystemCosts.isNull())
    {
        QDomElement child = m_SystemCosts->toDomElement(doc);
        child.setTagName("SystemCosts");
        e.appendChild(child);
    }
    if (!m_SystemReliability.isNull())
    {
        QDomElement child = m_SystemReliability->toDomElement(doc);
        child.setTagName("SystemReliability");
        e.appendChild(child);
    }
    if (!m_LVAerodynamics.isNull())
    {
        QDomElement child = m_LVAerodynamics->toDomElement(doc);
        child.setTagName("LVAerodynamics");
        e.appendChild(child);
    }
    if (!m_DiscretizationSettings.isNull())
    {
        QDomElement child = m_DiscretizationSettings->toDomElement(doc);
        child.setTagName("DiscretizationSettings");
        e.appendChild(child);
    }
}




// ScenarioArchitecture
ScenarioArchitecture::ScenarioArchitecture()
{
}

void ScenarioArchitecture::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_boosterConf = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_boosterConf->load(e.firstChildElement("boosterConf"));
    m_nStages = QSharedPointer<ScenarioOptVarInt>(new ScenarioOptVarInt());
    m_nStages->load(e.firstChildElement("nStages"));
    m_nBoosters = QSharedPointer<ScenarioOptVarInt>(new ScenarioOptVarInt());
    m_nBoosters->load(e.firstChildElement("nBoosters"));
    m_singleEngineType = QSharedPointer<ScenarioOptVarBool>(new ScenarioOptVarBool());
    m_singleEngineType->load(e.firstChildElement("singleEngineType"));
}

QDomElement ScenarioArchitecture::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_boosterConf.isNull())
    {
        QDomElement child = m_boosterConf->toDomElement(doc);
        child.setTagName("boosterConf");
        e.appendChild(child);
    }
    if (!m_nStages.isNull())
    {
        QDomElement child = m_nStages->toDomElement(doc);
        child.setTagName("nStages");
        e.appendChild(child);
    }
    if (!m_nBoosters.isNull())
    {
        QDomElement child = m_nBoosters->toDomElement(doc);
        child.setTagName("nBoosters");
        e.appendChild(child);
    }
    if (!m_singleEngineType.isNull())
    {
        QDomElement child = m_singleEngineType->toDomElement(doc);
        child.setTagName("singleEngineType");
        e.appendChild(child);
    }
}




// ScenarioLowerStage
ScenarioLowerStage::ScenarioLowerStage()
{
}

void ScenarioLowerStage::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Geometry = QSharedPointer<ScenarioGeometry>(new ScenarioGeometry());
    m_Geometry->load(e.firstChildElement("Geometry"));
    m_PropulsionSystem = QSharedPointer<ScenarioPropulsionSystem>(new ScenarioPropulsionSystem());
    m_PropulsionSystem->load(e.firstChildElement("PropulsionSystem"));
    m_ComponentWeights = QSharedPointer<ScenarioComponentWeights>(new ScenarioComponentWeights());
    m_ComponentWeights->load(e.firstChildElement("ComponentWeights"));
    m_ComponentCosts = QSharedPointer<ScenarioComponentCosts>(new ScenarioComponentCosts());
    m_ComponentCosts->load(e.firstChildElement("ComponentCosts"));
    m_ComponentReliability = QSharedPointer<ScenarioComponentReliability>(new ScenarioComponentReliability());
    m_ComponentReliability->load(e.firstChildElement("ComponentReliability"));
}

QDomElement ScenarioLowerStage::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Geometry.isNull())
    {
        QDomElement child = m_Geometry->toDomElement(doc);
        child.setTagName("Geometry");
        e.appendChild(child);
    }
    if (!m_PropulsionSystem.isNull())
    {
        QDomElement child = m_PropulsionSystem->toDomElement(doc);
        child.setTagName("PropulsionSystem");
        e.appendChild(child);
    }
    if (!m_ComponentWeights.isNull())
    {
        QDomElement child = m_ComponentWeights->toDomElement(doc);
        child.setTagName("ComponentWeights");
        e.appendChild(child);
    }
    if (!m_ComponentCosts.isNull())
    {
        QDomElement child = m_ComponentCosts->toDomElement(doc);
        child.setTagName("ComponentCosts");
        e.appendChild(child);
    }
    if (!m_ComponentReliability.isNull())
    {
        QDomElement child = m_ComponentReliability->toDomElement(doc);
        child.setTagName("ComponentReliability");
        e.appendChild(child);
    }
}




// ScenarioUpperStage
ScenarioUpperStage::ScenarioUpperStage()
{
}

void ScenarioUpperStage::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Geometry = QSharedPointer<ScenarioGeometry>(new ScenarioGeometry());
    m_Geometry->load(e.firstChildElement("Geometry"));
    m_PropulsionSystem = QSharedPointer<ScenarioPropulsionSystem>(new ScenarioPropulsionSystem());
    m_PropulsionSystem->load(e.firstChildElement("PropulsionSystem"));
    m_ComponentWeights = QSharedPointer<ScenarioComponentWeights>(new ScenarioComponentWeights());
    m_ComponentWeights->load(e.firstChildElement("ComponentWeights"));
    m_ComponentCosts = QSharedPointer<ScenarioComponentCosts>(new ScenarioComponentCosts());
    m_ComponentCosts->load(e.firstChildElement("ComponentCosts"));
    m_ComponentReliability = QSharedPointer<ScenarioComponentReliability>(new ScenarioComponentReliability());
    m_ComponentReliability->load(e.firstChildElement("ComponentReliability"));
}

QDomElement ScenarioUpperStage::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Geometry.isNull())
    {
        QDomElement child = m_Geometry->toDomElement(doc);
        child.setTagName("Geometry");
        e.appendChild(child);
    }
    if (!m_PropulsionSystem.isNull())
    {
        QDomElement child = m_PropulsionSystem->toDomElement(doc);
        child.setTagName("PropulsionSystem");
        e.appendChild(child);
    }
    if (!m_ComponentWeights.isNull())
    {
        QDomElement child = m_ComponentWeights->toDomElement(doc);
        child.setTagName("ComponentWeights");
        e.appendChild(child);
    }
    if (!m_ComponentCosts.isNull())
    {
        QDomElement child = m_ComponentCosts->toDomElement(doc);
        child.setTagName("ComponentCosts");
        e.appendChild(child);
    }
    if (!m_ComponentReliability.isNull())
    {
        QDomElement child = m_ComponentReliability->toDomElement(doc);
        child.setTagName("ComponentReliability");
        e.appendChild(child);
    }
}




// ScenarioBoosters
ScenarioBoosters::ScenarioBoosters()
{
}

void ScenarioBoosters::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Geometry = QSharedPointer<ScenarioGeometry>(new ScenarioGeometry());
    m_Geometry->load(e.firstChildElement("Geometry"));
    m_PropulsionSystem = QSharedPointer<ScenarioPropulsionSystem>(new ScenarioPropulsionSystem());
    m_PropulsionSystem->load(e.firstChildElement("PropulsionSystem"));
    m_ComponentWeights = QSharedPointer<ScenarioComponentWeights>(new ScenarioComponentWeights());
    m_ComponentWeights->load(e.firstChildElement("ComponentWeights"));
    m_ComponentCosts = QSharedPointer<ScenarioComponentCosts>(new ScenarioComponentCosts());
    m_ComponentCosts->load(e.firstChildElement("ComponentCosts"));
    m_ComponentReliability = QSharedPointer<ScenarioComponentReliability>(new ScenarioComponentReliability());
    m_ComponentReliability->load(e.firstChildElement("ComponentReliability"));
}

QDomElement ScenarioBoosters::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Geometry.isNull())
    {
        QDomElement child = m_Geometry->toDomElement(doc);
        child.setTagName("Geometry");
        e.appendChild(child);
    }
    if (!m_PropulsionSystem.isNull())
    {
        QDomElement child = m_PropulsionSystem->toDomElement(doc);
        child.setTagName("PropulsionSystem");
        e.appendChild(child);
    }
    if (!m_ComponentWeights.isNull())
    {
        QDomElement child = m_ComponentWeights->toDomElement(doc);
        child.setTagName("ComponentWeights");
        e.appendChild(child);
    }
    if (!m_ComponentCosts.isNull())
    {
        QDomElement child = m_ComponentCosts->toDomElement(doc);
        child.setTagName("ComponentCosts");
        e.appendChild(child);
    }
    if (!m_ComponentReliability.isNull())
    {
        QDomElement child = m_ComponentReliability->toDomElement(doc);
        child.setTagName("ComponentReliability");
        e.appendChild(child);
    }
}




// ScenarioDiscretizationSettings
ScenarioDiscretizationSettings::ScenarioDiscretizationSettings() :
    m_nMachPointsCl(0),
    m_nAlfaPointsCl(0),
    m_nMachPointsCd(0),
    m_nAlfaPointsCd(0),
    m_nMachPointsCm(0),
    m_nAlfaPointsCm(0),
    m_nPitchControlNodes(0),
    m_nYawControlNodes(0),
    m_nThrustControlNodes(0)
{
}

void ScenarioDiscretizationSettings::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_nMachPointsCl = parseInt(e.firstChildElement("nMachPointsCl").text());
    m_nAlfaPointsCl = parseInt(e.firstChildElement("nAlfaPointsCl").text());
    m_nMachPointsCd = parseInt(e.firstChildElement("nMachPointsCd").text());
    m_nAlfaPointsCd = parseInt(e.firstChildElement("nAlfaPointsCd").text());
    m_nMachPointsCm = parseInt(e.firstChildElement("nMachPointsCm").text());
    m_nAlfaPointsCm = parseInt(e.firstChildElement("nAlfaPointsCm").text());
    m_nPitchControlNodes = parseInt(e.firstChildElement("nPitchControlNodes").text());
    m_nYawControlNodes = parseInt(e.firstChildElement("nYawControlNodes").text());
    m_nThrustControlNodes = parseInt(e.firstChildElement("nThrustControlNodes").text());
}

QDomElement ScenarioDiscretizationSettings::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("nMachPointsCl");
        child.appendChild(doc.createTextNode(convertToString(m_nMachPointsCl);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nAlfaPointsCl");
        child.appendChild(doc.createTextNode(convertToString(m_nAlfaPointsCl);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nMachPointsCd");
        child.appendChild(doc.createTextNode(convertToString(m_nMachPointsCd);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nAlfaPointsCd");
        child.appendChild(doc.createTextNode(convertToString(m_nAlfaPointsCd);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nMachPointsCm");
        child.appendChild(doc.createTextNode(convertToString(m_nMachPointsCm);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nAlfaPointsCm");
        child.appendChild(doc.createTextNode(convertToString(m_nAlfaPointsCm);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nPitchControlNodes");
        child.appendChild(doc.createTextNode(convertToString(m_nPitchControlNodes);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nYawControlNodes");
        child.appendChild(doc.createTextNode(convertToString(m_nYawControlNodes);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nThrustControlNodes");
        child.appendChild(doc.createTextNode(convertToString(m_nThrustControlNodes);
        e.appendChild(child);
    }
}




// ScenarioGeometry
ScenarioGeometry::ScenarioGeometry() :
    m_length(0.0),
    m_diameter(0.0),
    m_baseLongPosition(0.0),
    m_constraintEngine(0.0),
    m_constraintInterference(0.0)
{
}

void ScenarioGeometry::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_length = parseDouble(e.firstChildElement("length").text());
    m_diameter = parseDouble(e.firstChildElement("diameter").text());
    m_diameterEqualToUpper = QSharedPointer<ScenarioOptVarBool>(new ScenarioOptVarBool());
    m_diameterEqualToUpper->load(e.firstChildElement("diameterEqualToUpper"));
    m_frontShape = (e.firstChildElement("frontShape").text());
    m_baseLongPosition = parseDouble(e.firstChildElement("baseLongPosition").text());
    m_constraintEngine = parseDouble(e.firstChildElement("constraintEngine").text());
    m_constraintInterference = parseDouble(e.firstChildElement("constraintInterference").text());
}

QDomElement ScenarioGeometry::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("length");
        child.appendChild(doc.createTextNode(convertToString(m_length);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("diameter");
        child.appendChild(doc.createTextNode(convertToString(m_diameter);
        e.appendChild(child);
    }
    if (!m_diameterEqualToUpper.isNull())
    {
        QDomElement child = m_diameterEqualToUpper->toDomElement(doc);
        child.setTagName("diameterEqualToUpper");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("frontShape");
        child.appendChild(doc.createTextNode(convertToString(m_frontShape);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("baseLongPosition");
        child.appendChild(doc.createTextNode(convertToString(m_baseLongPosition);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("constraintEngine");
        child.appendChild(doc.createTextNode(convertToString(m_constraintEngine);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("constraintInterference");
        child.appendChild(doc.createTextNode(convertToString(m_constraintInterference);
        e.appendChild(child);
    }
}




// ScenarioLVAerodynamics
ScenarioLVAerodynamics::ScenarioLVAerodynamics() :
    m_referenceArea(0.0),
    m_referenceLength(0.0)
{
}

void ScenarioLVAerodynamics::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_userDefinedAero = parseBoolean(e.firstChildElement("userDefinedAero").text());
    m_referenceArea = parseDouble(e.firstChildElement("referenceArea").text());
    m_referenceLength = parseDouble(e.firstChildElement("referenceLength").text());
    m_clFileName = (e.firstChildElement("clFileName").text());
    m_cdFileName = (e.firstChildElement("cdFileName").text());
    m_cmFileName = (e.firstChildElement("cmFileName").text());
}

QDomElement ScenarioLVAerodynamics::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("userDefinedAero");
        child.appendChild(doc.createTextNode(convertToString(m_userDefinedAero);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("referenceArea");
        child.appendChild(doc.createTextNode(convertToString(m_referenceArea);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("referenceLength");
        child.appendChild(doc.createTextNode(convertToString(m_referenceLength);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("clFileName");
        child.appendChild(doc.createTextNode(convertToString(m_clFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("cdFileName");
        child.appendChild(doc.createTextNode(convertToString(m_cdFileName);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("cmFileName");
        child.appendChild(doc.createTextNode(convertToString(m_cmFileName);
        e.appendChild(child);
    }
}




// ScenarioComponentWeights
ScenarioComponentWeights::ScenarioComponentWeights() :
    m_dryMass(0.0),
    m_dryCoGLongPosition(0.0),
    m_wetMass(0.0),
    m_mainStructMass(0.0),
    m_oxTankMass(0.0),
    m_fuelTankMass(0.0),
    m_tpsMass(0.0),
    m_avionicsMass(0.0),
    m_epsMass(0.0),
    m_plAdapterMass(0.0),
    m_padInterfaceMass(0.0),
    m_interstageMass(0.0)
{
}

void ScenarioComponentWeights::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_dryMass = parseDouble(e.firstChildElement("dryMass").text());
    m_dryCoGLongPosition = parseDouble(e.firstChildElement("dryCoGLongPosition").text());
    m_wetMass = parseDouble(e.firstChildElement("wetMass").text());
    m_mainStructMass = parseDouble(e.firstChildElement("mainStructMass").text());
    m_oxTankMass = parseDouble(e.firstChildElement("oxTankMass").text());
    m_fuelTankMass = parseDouble(e.firstChildElement("fuelTankMass").text());
    m_tpsMass = parseDouble(e.firstChildElement("tpsMass").text());
    m_avionicsMass = parseDouble(e.firstChildElement("avionicsMass").text());
    m_epsMass = parseDouble(e.firstChildElement("epsMass").text());
    m_plAdapterMass = parseDouble(e.firstChildElement("plAdapterMass").text());
    m_padInterfaceMass = parseDouble(e.firstChildElement("padInterfaceMass").text());
    m_interstageMass = parseDouble(e.firstChildElement("interstageMass").text());
}

QDomElement ScenarioComponentWeights::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("dryMass");
        child.appendChild(doc.createTextNode(convertToString(m_dryMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("dryCoGLongPosition");
        child.appendChild(doc.createTextNode(convertToString(m_dryCoGLongPosition);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("wetMass");
        child.appendChild(doc.createTextNode(convertToString(m_wetMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("mainStructMass");
        child.appendChild(doc.createTextNode(convertToString(m_mainStructMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("oxTankMass");
        child.appendChild(doc.createTextNode(convertToString(m_oxTankMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fuelTankMass");
        child.appendChild(doc.createTextNode(convertToString(m_fuelTankMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("tpsMass");
        child.appendChild(doc.createTextNode(convertToString(m_tpsMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("avionicsMass");
        child.appendChild(doc.createTextNode(convertToString(m_avionicsMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("epsMass");
        child.appendChild(doc.createTextNode(convertToString(m_epsMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("plAdapterMass");
        child.appendChild(doc.createTextNode(convertToString(m_plAdapterMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("padInterfaceMass");
        child.appendChild(doc.createTextNode(convertToString(m_padInterfaceMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("interstageMass");
        child.appendChild(doc.createTextNode(convertToString(m_interstageMass);
        e.appendChild(child);
    }
}




// ScenarioSystemWeights
ScenarioSystemWeights::ScenarioSystemWeights() :
    m_totalDryMass(0.0),
    m_totalWetMass(0.0),
    m_takeoffThrustOverWeight(0.0)
{
}

void ScenarioSystemWeights::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalDryMass = parseDouble(e.firstChildElement("totalDryMass").text());
    m_totalWetMass = parseDouble(e.firstChildElement("totalWetMass").text());
    m_takeoffThrustOverWeight = parseDouble(e.firstChildElement("takeoffThrustOverWeight").text());
    m_maxAxialAcc = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_maxAxialAcc->load(e.firstChildElement("maxAxialAcc"));
    m_maxHeatFlux = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_maxHeatFlux->load(e.firstChildElement("maxHeatFlux"));
    m_maxDynPressure = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_maxDynPressure->load(e.firstChildElement("maxDynPressure"));
    m_mainStructuralMaterial = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_mainStructuralMaterial->load(e.firstChildElement("mainStructuralMaterial"));
    m_tanksArrangement = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_tanksArrangement->load(e.firstChildElement("tanksArrangement"));
    m_tanksType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_tanksType->load(e.firstChildElement("tanksType"));
    m_redundancyLevel = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_redundancyLevel->load(e.firstChildElement("redundancyLevel"));
    m_structuralSafetyMargin = QSharedPointer<ScenarioOptVarDouble>(new ScenarioOptVarDouble());
    m_structuralSafetyMargin->load(e.firstChildElement("structuralSafetyMargin"));
}

QDomElement ScenarioSystemWeights::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalDryMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalDryMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalWetMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalWetMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("takeoffThrustOverWeight");
        child.appendChild(doc.createTextNode(convertToString(m_takeoffThrustOverWeight);
        e.appendChild(child);
    }
    if (!m_maxAxialAcc.isNull())
    {
        QDomElement child = m_maxAxialAcc->toDomElement(doc);
        child.setTagName("maxAxialAcc");
        e.appendChild(child);
    }
    if (!m_maxHeatFlux.isNull())
    {
        QDomElement child = m_maxHeatFlux->toDomElement(doc);
        child.setTagName("maxHeatFlux");
        e.appendChild(child);
    }
    if (!m_maxDynPressure.isNull())
    {
        QDomElement child = m_maxDynPressure->toDomElement(doc);
        child.setTagName("maxDynPressure");
        e.appendChild(child);
    }
    if (!m_mainStructuralMaterial.isNull())
    {
        QDomElement child = m_mainStructuralMaterial->toDomElement(doc);
        child.setTagName("mainStructuralMaterial");
        e.appendChild(child);
    }
    if (!m_tanksArrangement.isNull())
    {
        QDomElement child = m_tanksArrangement->toDomElement(doc);
        child.setTagName("tanksArrangement");
        e.appendChild(child);
    }
    if (!m_tanksType.isNull())
    {
        QDomElement child = m_tanksType->toDomElement(doc);
        child.setTagName("tanksType");
        e.appendChild(child);
    }
    if (!m_redundancyLevel.isNull())
    {
        QDomElement child = m_redundancyLevel->toDomElement(doc);
        child.setTagName("redundancyLevel");
        e.appendChild(child);
    }
    if (!m_structuralSafetyMargin.isNull())
    {
        QDomElement child = m_structuralSafetyMargin->toDomElement(doc);
        child.setTagName("structuralSafetyMargin");
        e.appendChild(child);
    }
}




// ScenarioComponentCosts
ScenarioComponentCosts::ScenarioComponentCosts()
{
}

void ScenarioComponentCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_nEngineTests = QSharedPointer<ScenarioOptVarInt>(new ScenarioOptVarInt());
    m_nEngineTests->load(e.firstChildElement("nEngineTests"));
    m_DevelopmentCosts = QSharedPointer<ScenarioDevelopmentCosts>(new ScenarioDevelopmentCosts());
    m_DevelopmentCosts->load(e.firstChildElement("DevelopmentCosts"));
    m_ProductionCosts = QSharedPointer<ScenarioProductionCosts>(new ScenarioProductionCosts());
    m_ProductionCosts->load(e.firstChildElement("ProductionCosts"));
}

QDomElement ScenarioComponentCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_nEngineTests.isNull())
    {
        QDomElement child = m_nEngineTests->toDomElement(doc);
        child.setTagName("nEngineTests");
        e.appendChild(child);
    }
    if (!m_DevelopmentCosts.isNull())
    {
        QDomElement child = m_DevelopmentCosts->toDomElement(doc);
        child.setTagName("DevelopmentCosts");
        e.appendChild(child);
    }
    if (!m_ProductionCosts.isNull())
    {
        QDomElement child = m_ProductionCosts->toDomElement(doc);
        child.setTagName("ProductionCosts");
        e.appendChild(child);
    }
}




// ScenarioDevelopmentCosts
ScenarioDevelopmentCosts::ScenarioDevelopmentCosts() :
    m_enginef2(0.0),
    m_enginef5(0.0),
    m_engineDevelopmentCost(0.0),
    m_systemf2(0.0),
    m_systemDevelopmentCost(0.0)
{
}

void ScenarioDevelopmentCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_enginef2 = parseDouble(e.firstChildElement("enginef2").text());
    m_enginef5 = parseDouble(e.firstChildElement("enginef5").text());
    m_engineDevelopmentCost = parseDouble(e.firstChildElement("engineDevelopmentCost").text());
    m_systemf2 = parseDouble(e.firstChildElement("systemf2").text());
    m_systemDevelopmentCost = parseDouble(e.firstChildElement("systemDevelopmentCost").text());
}

QDomElement ScenarioDevelopmentCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("enginef2");
        child.appendChild(doc.createTextNode(convertToString(m_enginef2);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("enginef5");
        child.appendChild(doc.createTextNode(convertToString(m_enginef5);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("engineDevelopmentCost");
        child.appendChild(doc.createTextNode(convertToString(m_engineDevelopmentCost);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("systemf2");
        child.appendChild(doc.createTextNode(convertToString(m_systemf2);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("systemDevelopmentCost");
        child.appendChild(doc.createTextNode(convertToString(m_systemDevelopmentCost);
        e.appendChild(child);
    }
}




// ScenarioProductionCosts
ScenarioProductionCosts::ScenarioProductionCosts() :
    m_totalNumberOfEngines(0),
    m_enginesLearningFactor(0.0),
    m_totalNumberOfSystems(0),
    m_systemsLearningFactor(0.0),
    m_totalEngineProductionCost(0.0),
    m_totalSystemProductionCost(0.0)
{
}

void ScenarioProductionCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalNumberOfEngines = parseInt(e.firstChildElement("totalNumberOfEngines").text());
    m_enginesLearningFactor = parseDouble(e.firstChildElement("enginesLearningFactor").text());
    m_totalNumberOfSystems = parseInt(e.firstChildElement("totalNumberOfSystems").text());
    m_systemsLearningFactor = parseDouble(e.firstChildElement("systemsLearningFactor").text());
    m_totalEngineProductionCost = parseDouble(e.firstChildElement("totalEngineProductionCost").text());
    m_totalSystemProductionCost = parseDouble(e.firstChildElement("totalSystemProductionCost").text());
}

QDomElement ScenarioProductionCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalNumberOfEngines");
        child.appendChild(doc.createTextNode(convertToString(m_totalNumberOfEngines);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("enginesLearningFactor");
        child.appendChild(doc.createTextNode(convertToString(m_enginesLearningFactor);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalNumberOfSystems");
        child.appendChild(doc.createTextNode(convertToString(m_totalNumberOfSystems);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("systemsLearningFactor");
        child.appendChild(doc.createTextNode(convertToString(m_systemsLearningFactor);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalEngineProductionCost");
        child.appendChild(doc.createTextNode(convertToString(m_totalEngineProductionCost);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalSystemProductionCost");
        child.appendChild(doc.createTextNode(convertToString(m_totalSystemProductionCost);
        e.appendChild(child);
    }
}




// ScenarioSystemCosts
ScenarioSystemCosts::ScenarioSystemCosts() :
    m_developmentTime(0.0)
{
}

void ScenarioSystemCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_TotalProgramCosts = QSharedPointer<ScenarioTotalProgramCosts>(new ScenarioTotalProgramCosts());
    m_TotalProgramCosts->load(e.firstChildElement("TotalProgramCosts"));
    m_developmentTime = parseDouble(e.firstChildElement("developmentTime").text());
    m_DirectOperationsCosts = QSharedPointer<ScenarioDirectOperationsCosts>(new ScenarioDirectOperationsCosts());
    m_DirectOperationsCosts->load(e.firstChildElement("DirectOperationsCosts"));
    m_IndirectOperationsCosts = QSharedPointer<ScenarioIndirectOperationsCosts>(new ScenarioIndirectOperationsCosts());
    m_IndirectOperationsCosts->load(e.firstChildElement("IndirectOperationsCosts"));
}

QDomElement ScenarioSystemCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_TotalProgramCosts.isNull())
    {
        QDomElement child = m_TotalProgramCosts->toDomElement(doc);
        child.setTagName("TotalProgramCosts");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("developmentTime");
        child.appendChild(doc.createTextNode(convertToString(m_developmentTime);
        e.appendChild(child);
    }
    if (!m_DirectOperationsCosts.isNull())
    {
        QDomElement child = m_DirectOperationsCosts->toDomElement(doc);
        child.setTagName("DirectOperationsCosts");
        e.appendChild(child);
    }
    if (!m_IndirectOperationsCosts.isNull())
    {
        QDomElement child = m_IndirectOperationsCosts->toDomElement(doc);
        child.setTagName("IndirectOperationsCosts");
        e.appendChild(child);
    }
}




// ScenarioTotalProgramCosts
ScenarioTotalProgramCosts::ScenarioTotalProgramCosts() :
    m_lifeCycleCostMY(0.0),
    m_costPerLaunchMY(0.0),
    m_costPerKiloMY(0.0),
    m_FYref(0),
    m_MYtoFYEuros(0.0),
    m_lifeCycleCostFYEuros(0.0),
    m_costPerLaunchFYEuros(0.0),
    m_costPerKiloFYEuros(0.0)
{
}

void ScenarioTotalProgramCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_lifeCycleCostMY = parseDouble(e.firstChildElement("lifeCycleCostMY").text());
    m_costPerLaunchMY = parseDouble(e.firstChildElement("costPerLaunchMY").text());
    m_costPerKiloMY = parseDouble(e.firstChildElement("costPerKiloMY").text());
    m_FYref = parseInt(e.firstChildElement("FYref").text());
    m_MYtoFYEuros = parseDouble(e.firstChildElement("MYtoFYEuros").text());
    m_lifeCycleCostFYEuros = parseDouble(e.firstChildElement("lifeCycleCostFYEuros").text());
    m_costPerLaunchFYEuros = parseDouble(e.firstChildElement("costPerLaunchFYEuros").text());
    m_costPerKiloFYEuros = parseDouble(e.firstChildElement("costPerKiloFYEuros").text());
}

QDomElement ScenarioTotalProgramCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("lifeCycleCostMY");
        child.appendChild(doc.createTextNode(convertToString(m_lifeCycleCostMY);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("costPerLaunchMY");
        child.appendChild(doc.createTextNode(convertToString(m_costPerLaunchMY);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("costPerKiloMY");
        child.appendChild(doc.createTextNode(convertToString(m_costPerKiloMY);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("FYref");
        child.appendChild(doc.createTextNode(convertToString(m_FYref);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("MYtoFYEuros");
        child.appendChild(doc.createTextNode(convertToString(m_MYtoFYEuros);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("lifeCycleCostFYEuros");
        child.appendChild(doc.createTextNode(convertToString(m_lifeCycleCostFYEuros);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("costPerLaunchFYEuros");
        child.appendChild(doc.createTextNode(convertToString(m_costPerLaunchFYEuros);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("costPerKiloFYEuros");
        child.appendChild(doc.createTextNode(convertToString(m_costPerKiloFYEuros);
        e.appendChild(child);
    }
}




// ScenarioDirectOperationsCosts
ScenarioDirectOperationsCosts::ScenarioDirectOperationsCosts() :
    m_transportCost(0.0),
    m_fv(0.0),
    m_fc(0.0),
    m_groundOperationsCost(0.0),
    m_Qn(0.0),
    m_flightOperationsCost(0.0),
    m_propellantsCost(0.0),
    m_insuranceFee(0.0)
{
}

void ScenarioDirectOperationsCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_transportCost = parseDouble(e.firstChildElement("transportCost").text());
    m_processingType = QSharedPointer<ScenarioOptVarString>(new ScenarioOptVarString());
    m_processingType->load(e.firstChildElement("processingType"));
    m_fv = parseDouble(e.firstChildElement("fv").text());
    m_fc = parseDouble(e.firstChildElement("fc").text());
    m_groundOperationsCost = parseDouble(e.firstChildElement("groundOperationsCost").text());
    m_Qn = parseDouble(e.firstChildElement("Qn").text());
    m_flightOperationsCost = parseDouble(e.firstChildElement("flightOperationsCost").text());
    m_propellantsCost = parseDouble(e.firstChildElement("propellantsCost").text());
    m_insuranceFee = parseDouble(e.firstChildElement("insuranceFee").text());
}

QDomElement ScenarioDirectOperationsCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("transportCost");
        child.appendChild(doc.createTextNode(convertToString(m_transportCost);
        e.appendChild(child);
    }
    if (!m_processingType.isNull())
    {
        QDomElement child = m_processingType->toDomElement(doc);
        child.setTagName("processingType");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fv");
        child.appendChild(doc.createTextNode(convertToString(m_fv);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fc");
        child.appendChild(doc.createTextNode(convertToString(m_fc);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("groundOperationsCost");
        child.appendChild(doc.createTextNode(convertToString(m_groundOperationsCost);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("Qn");
        child.appendChild(doc.createTextNode(convertToString(m_Qn);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("flightOperationsCost");
        child.appendChild(doc.createTextNode(convertToString(m_flightOperationsCost);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("propellantsCost");
        child.appendChild(doc.createTextNode(convertToString(m_propellantsCost);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("insuranceFee");
        child.appendChild(doc.createTextNode(convertToString(m_insuranceFee);
        e.appendChild(child);
    }
}




// ScenarioIndirectOperationsCosts
ScenarioIndirectOperationsCosts::ScenarioIndirectOperationsCosts() :
    m_groundFacilitiesBuildingCost(0.0),
    m_launchSiteCostPerYear(0.0),
    m_personnelCostPerYear(0.0)
{
}

void ScenarioIndirectOperationsCosts::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_groundFacilitiesBuildingCost = parseDouble(e.firstChildElement("groundFacilitiesBuildingCost").text());
    m_launchSiteCostPerYear = parseDouble(e.firstChildElement("launchSiteCostPerYear").text());
    m_personnelCostPerYear = parseDouble(e.firstChildElement("personnelCostPerYear").text());
}

QDomElement ScenarioIndirectOperationsCosts::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("groundFacilitiesBuildingCost");
        child.appendChild(doc.createTextNode(convertToString(m_groundFacilitiesBuildingCost);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("launchSiteCostPerYear");
        child.appendChild(doc.createTextNode(convertToString(m_launchSiteCostPerYear);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("personnelCostPerYear");
        child.appendChild(doc.createTextNode(convertToString(m_personnelCostPerYear);
        e.appendChild(child);
    }
}




// ScenarioComponentReliability
ScenarioComponentReliability::ScenarioComponentReliability() :
    m_avionicsFailureRate(0.0),
    m_structureFailureRate(0.0),
    m_tpsFailureRate(0.0),
    m_overallComponentFailureRate(0.0)
{
}

void ScenarioComponentReliability::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Separation = QSharedPointer<ScenarioSeparation>(new ScenarioSeparation());
    m_Separation->load(e.firstChildElement("Separation"));
    m_avionicsFailureRate = parseDouble(e.firstChildElement("avionicsFailureRate").text());
    m_structureFailureRate = parseDouble(e.firstChildElement("structureFailureRate").text());
    m_tpsFailureRate = parseDouble(e.firstChildElement("tpsFailureRate").text());
    m_PropulsionReliability = QSharedPointer<ScenarioPropulsionReliability>(new ScenarioPropulsionReliability());
    m_PropulsionReliability->load(e.firstChildElement("PropulsionReliability"));
    m_overallComponentFailureRate = parseDouble(e.firstChildElement("overallComponentFailureRate").text());
}

QDomElement ScenarioComponentReliability::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Separation.isNull())
    {
        QDomElement child = m_Separation->toDomElement(doc);
        child.setTagName("Separation");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("avionicsFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_avionicsFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("structureFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_structureFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("tpsFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_tpsFailureRate);
        e.appendChild(child);
    }
    if (!m_PropulsionReliability.isNull())
    {
        QDomElement child = m_PropulsionReliability->toDomElement(doc);
        child.setTagName("PropulsionReliability");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("overallComponentFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_overallComponentFailureRate);
        e.appendChild(child);
    }
}




// ScenarioSeparation
ScenarioSeparation::ScenarioSeparation() :
    m_stageSeparationReliability(0.0),
    m_boostersSetSeparationReliability(0.0),
    m_fairingSeparationReliability(0.0),
    m_payloadSeparationReliability(0.0)
{
}

void ScenarioSeparation::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_stageSeparationReliability = parseDouble(e.firstChildElement("stageSeparationReliability").text());
    m_boostersSetSeparationReliability = parseDouble(e.firstChildElement("boostersSetSeparationReliability").text());
    m_fairingSeparationReliability = parseDouble(e.firstChildElement("fairingSeparationReliability").text());
    m_payloadSeparationReliability = parseDouble(e.firstChildElement("payloadSeparationReliability").text());
}

QDomElement ScenarioSeparation::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("stageSeparationReliability");
        child.appendChild(doc.createTextNode(convertToString(m_stageSeparationReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("boostersSetSeparationReliability");
        child.appendChild(doc.createTextNode(convertToString(m_boostersSetSeparationReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("fairingSeparationReliability");
        child.appendChild(doc.createTextNode(convertToString(m_fairingSeparationReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("payloadSeparationReliability");
        child.appendChild(doc.createTextNode(convertToString(m_payloadSeparationReliability);
        e.appendChild(child);
    }
}




// ScenarioPropulsionReliability
ScenarioPropulsionReliability::ScenarioPropulsionReliability() :
    m_igniterReliability(0.0),
    m_feedFailureRate(0.0),
    m_grainFailureRate(0.0),
    m_chamberFailureRate(0.0),
    m_nozzleFailureRate(0.0),
    m_tvcFailureRate(0.0),
    m_singleEngineOverallFailureRate(0.0),
    m_propulsionOverallFailureRate(0.0)
{
}

void ScenarioPropulsionReliability::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_igniterReliability = parseDouble(e.firstChildElement("igniterReliability").text());
    m_feedFailureRate = parseDouble(e.firstChildElement("feedFailureRate").text());
    m_grainFailureRate = parseDouble(e.firstChildElement("grainFailureRate").text());
    m_chamberFailureRate = parseDouble(e.firstChildElement("chamberFailureRate").text());
    m_nozzleFailureRate = parseDouble(e.firstChildElement("nozzleFailureRate").text());
    m_tvcFailureRate = parseDouble(e.firstChildElement("tvcFailureRate").text());
    m_singleEngineOverallFailureRate = parseDouble(e.firstChildElement("singleEngineOverallFailureRate").text());
    m_propulsionOverallFailureRate = parseDouble(e.firstChildElement("propulsionOverallFailureRate").text());
}

QDomElement ScenarioPropulsionReliability::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("igniterReliability");
        child.appendChild(doc.createTextNode(convertToString(m_igniterReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("feedFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_feedFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("grainFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_grainFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("chamberFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_chamberFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("nozzleFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_nozzleFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("tvcFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_tvcFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("singleEngineOverallFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_singleEngineOverallFailureRate);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("propulsionOverallFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_propulsionOverallFailureRate);
        e.appendChild(child);
    }
}




// ScenarioSystemReliability
ScenarioSystemReliability::ScenarioSystemReliability() :
    m_globalVehicleReliability(0.0),
    m_selfDestructReliability(0.0),
    m_missionSafety(0.0),
    m_preLaunchReliability(0.0),
    m_numberOfAscentPhases(0)
{
}

void ScenarioSystemReliability::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_globalVehicleReliability = parseDouble(e.firstChildElement("globalVehicleReliability").text());
    m_selfDestructReliability = parseDouble(e.firstChildElement("selfDestructReliability").text());
    m_missionSafety = parseDouble(e.firstChildElement("missionSafety").text());
    m_preLaunchReliability = parseDouble(e.firstChildElement("preLaunchReliability").text());
    m_Launch = QSharedPointer<ScenarioLaunch>(new ScenarioLaunch());
    m_Launch->load(e.firstChildElement("Launch"));
    m_numberOfAscentPhases = parseInt(e.firstChildElement("numberOfAscentPhases").text());
    m_ascentPhasesDurations = parseDoubleList(e.firstChildElement("ascentPhasesDurations").text());
    m_ascentPhasesFailureRate = parseDoubleList(e.firstChildElement("ascentPhasesFailureRate").text());
}

QDomElement ScenarioSystemReliability::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("globalVehicleReliability");
        child.appendChild(doc.createTextNode(convertToString(m_globalVehicleReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("selfDestructReliability");
        child.appendChild(doc.createTextNode(convertToString(m_selfDestructReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("missionSafety");
        child.appendChild(doc.createTextNode(convertToString(m_missionSafety);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("preLaunchReliability");
        child.appendChild(doc.createTextNode(convertToString(m_preLaunchReliability);
        e.appendChild(child);
    }
    if (!m_Launch.isNull())
    {
        QDomElement child = m_Launch->toDomElement(doc);
        child.setTagName("Launch");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("numberOfAscentPhases");
        child.appendChild(doc.createTextNode(convertToString(m_numberOfAscentPhases);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("ascentPhasesDurations");
        child.appendChild(doc.createTextNode(convertToString(m_ascentPhasesDurations);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("ascentPhasesFailureRate");
        child.appendChild(doc.createTextNode(convertToString(m_ascentPhasesFailureRate);
        e.appendChild(child);
    }
}




// ScenarioLaunch
ScenarioLaunch::ScenarioLaunch() :
    m_launchSiteReliability(0.0),
    m_ignitionReliability(0.0),
    m_controlMarginReliability(0.0),
    m_launchOverallReliability(0.0)
{
}

void ScenarioLaunch::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_launchSiteReliability = parseDouble(e.firstChildElement("launchSiteReliability").text());
    m_ignitionReliability = parseDouble(e.firstChildElement("ignitionReliability").text());
    m_controlMarginReliability = parseDouble(e.firstChildElement("controlMarginReliability").text());
    m_launchOverallReliability = parseDouble(e.firstChildElement("launchOverallReliability").text());
}

QDomElement ScenarioLaunch::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("launchSiteReliability");
        child.appendChild(doc.createTextNode(convertToString(m_launchSiteReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("ignitionReliability");
        child.appendChild(doc.createTextNode(convertToString(m_ignitionReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("controlMarginReliability");
        child.appendChild(doc.createTextNode(convertToString(m_controlMarginReliability);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("launchOverallReliability");
        child.appendChild(doc.createTextNode(convertToString(m_launchOverallReliability);
        e.appendChild(child);
    }
}




// ScenarioTrajectory
ScenarioTrajectory::ScenarioTrajectory()
{
}

void ScenarioTrajectory::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_TakeOff = QSharedPointer<ScenarioTakeOff>(new ScenarioTakeOff());
    m_TakeOff->load(e.firstChildElement("TakeOff"));
    m_Ignitions = QSharedPointer<ScenarioIgnitions>(new ScenarioIgnitions());
    m_Ignitions->load(e.firstChildElement("Ignitions"));
    m_AtmosphericFlight = QSharedPointer<ScenarioAtmosphericFlight>(new ScenarioAtmosphericFlight());
    m_AtmosphericFlight->load(e.firstChildElement("AtmosphericFlight"));
    m_ExoatmosphericFlight = QSharedPointer<ScenarioExoatmosphericFlight>(new ScenarioExoatmosphericFlight());
    m_ExoatmosphericFlight->load(e.firstChildElement("ExoatmosphericFlight"));
    m_Constraints = QSharedPointer<ScenarioConstraints>(new ScenarioConstraints());
    m_Constraints->load(e.firstChildElement("Constraints"));
}

QDomElement ScenarioTrajectory::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_TakeOff.isNull())
    {
        QDomElement child = m_TakeOff->toDomElement(doc);
        child.setTagName("TakeOff");
        e.appendChild(child);
    }
    if (!m_Ignitions.isNull())
    {
        QDomElement child = m_Ignitions->toDomElement(doc);
        child.setTagName("Ignitions");
        e.appendChild(child);
    }
    if (!m_AtmosphericFlight.isNull())
    {
        QDomElement child = m_AtmosphericFlight->toDomElement(doc);
        child.setTagName("AtmosphericFlight");
        e.appendChild(child);
    }
    if (!m_ExoatmosphericFlight.isNull())
    {
        QDomElement child = m_ExoatmosphericFlight->toDomElement(doc);
        child.setTagName("ExoatmosphericFlight");
        e.appendChild(child);
    }
    if (!m_Constraints.isNull())
    {
        QDomElement child = m_Constraints->toDomElement(doc);
        child.setTagName("Constraints");
        e.appendChild(child);
    }
}




// ScenarioTakeOff
ScenarioTakeOff::ScenarioTakeOff() :
    m_padClearingAlt(0.0),
    m_maxPitchOverAngle(0.0),
    m_pitchOverDuration(0.0),
    m_pitchOverDecayTime(0.0),
    m_pitchOverHeadingAngle(0.0)
{
}

void ScenarioTakeOff::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_padClearingAlt = parseDouble(e.firstChildElement("padClearingAlt").text());
    m_maxPitchOverAngle = parseDouble(e.firstChildElement("maxPitchOverAngle").text());
    m_pitchOverDuration = parseDouble(e.firstChildElement("pitchOverDuration").text());
    m_pitchOverDecayTime = parseDouble(e.firstChildElement("pitchOverDecayTime").text());
    m_pitchOverHeadingAngle = parseDouble(e.firstChildElement("pitchOverHeadingAngle").text());
}

QDomElement ScenarioTakeOff::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("padClearingAlt");
        child.appendChild(doc.createTextNode(convertToString(m_padClearingAlt);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("maxPitchOverAngle");
        child.appendChild(doc.createTextNode(convertToString(m_maxPitchOverAngle);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("pitchOverDuration");
        child.appendChild(doc.createTextNode(convertToString(m_pitchOverDuration);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("pitchOverDecayTime");
        child.appendChild(doc.createTextNode(convertToString(m_pitchOverDecayTime);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("pitchOverHeadingAngle");
        child.appendChild(doc.createTextNode(convertToString(m_pitchOverHeadingAngle);
        e.appendChild(child);
    }
}




// ScenarioIgnitions
ScenarioIgnitions::ScenarioIgnitions() :
    m_coreIgnitionDelayTime(0.0),
    m_secondBoostersSetIgnitionDelayTime(0.0)
{
}

void ScenarioIgnitions::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_coreIgnitionDelayOption = parseBoolean(e.firstChildElement("coreIgnitionDelayOption").text());
    m_coreIgnitionDelayTime = parseDouble(e.firstChildElement("coreIgnitionDelayTime").text());
    m_secondBoostersSetIgnitionDelayOption = parseBoolean(e.firstChildElement("secondBoostersSetIgnitionDelayOption").text());
    m_secondBoostersSetIgnitionDelayTime = parseDouble(e.firstChildElement("secondBoostersSetIgnitionDelayTime").text());
}

QDomElement ScenarioIgnitions::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("coreIgnitionDelayOption");
        child.appendChild(doc.createTextNode(convertToString(m_coreIgnitionDelayOption);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("coreIgnitionDelayTime");
        child.appendChild(doc.createTextNode(convertToString(m_coreIgnitionDelayTime);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("secondBoostersSetIgnitionDelayOption");
        child.appendChild(doc.createTextNode(convertToString(m_secondBoostersSetIgnitionDelayOption);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("secondBoostersSetIgnitionDelayTime");
        child.appendChild(doc.createTextNode(convertToString(m_secondBoostersSetIgnitionDelayTime);
        e.appendChild(child);
    }
}




// ScenarioAtmosphericFlight
ScenarioAtmosphericFlight::ScenarioAtmosphericFlight()
{
}

void ScenarioAtmosphericFlight::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_optimizedPitchValues = parseDoubleList(e.firstChildElement("optimizedPitchValues").text());
    m_optimizedYawValues = parseDoubleList(e.firstChildElement("optimizedYawValues").text());
    m_optimizedThrustValues = parseDoubleList(e.firstChildElement("optimizedThrustValues").text());
}

QDomElement ScenarioAtmosphericFlight::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("optimizedPitchValues");
        child.appendChild(doc.createTextNode(convertToString(m_optimizedPitchValues);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("optimizedYawValues");
        child.appendChild(doc.createTextNode(convertToString(m_optimizedYawValues);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("optimizedThrustValues");
        child.appendChild(doc.createTextNode(convertToString(m_optimizedThrustValues);
        e.appendChild(child);
    }
}




// ScenarioExoatmosphericFlight
ScenarioExoatmosphericFlight::ScenarioExoatmosphericFlight() :
    m_bilinearLawInitPitch(0.0),
    m_bilinearLawFinalPitch(0.0),
    m_bilinearLawParam(0.0),
    m_circBurnTime(0.0)
{
}

void ScenarioExoatmosphericFlight::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_bilinearLawInitPitch = parseDouble(e.firstChildElement("bilinearLawInitPitch").text());
    m_bilinearLawFinalPitch = parseDouble(e.firstChildElement("bilinearLawFinalPitch").text());
    m_bilinearLawParam = parseDouble(e.firstChildElement("bilinearLawParam").text());
    m_circBurnOption = parseBoolean(e.firstChildElement("circBurnOption").text());
    m_circBurnTime = parseDouble(e.firstChildElement("circBurnTime").text());
}

QDomElement ScenarioExoatmosphericFlight::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("bilinearLawInitPitch");
        child.appendChild(doc.createTextNode(convertToString(m_bilinearLawInitPitch);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("bilinearLawFinalPitch");
        child.appendChild(doc.createTextNode(convertToString(m_bilinearLawFinalPitch);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("bilinearLawParam");
        child.appendChild(doc.createTextNode(convertToString(m_bilinearLawParam);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("circBurnOption");
        child.appendChild(doc.createTextNode(convertToString(m_circBurnOption);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("circBurnTime");
        child.appendChild(doc.createTextNode(convertToString(m_circBurnTime);
        e.appendChild(child);
    }
}




// ScenarioConstraints
ScenarioConstraints::ScenarioConstraints() :
    m_finalSemiaxisError(0.0),
    m_finalEccError(0.0),
    m_finalInclError(0.0),
    m_axialAccCstrViolation(0.0),
    m_heatFluxCstrViolation(0.0),
    m_dynPressCstrViolation(0.0),
    m_controllabilityCstrViolation(0.0)
{
}

void ScenarioConstraints::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_finalSemiaxisError = parseDouble(e.firstChildElement("finalSemiaxisError").text());
    m_finalEccError = parseDouble(e.firstChildElement("finalEccError").text());
    m_finalInclError = parseDouble(e.firstChildElement("finalInclError").text());
    m_axialAccCstrViolation = parseDouble(e.firstChildElement("axialAccCstrViolation").text());
    m_heatFluxCstrViolation = parseDouble(e.firstChildElement("heatFluxCstrViolation").text());
    m_dynPressCstrViolation = parseDouble(e.firstChildElement("dynPressCstrViolation").text());
    m_controllabilityCstrViolation = parseDouble(e.firstChildElement("controllabilityCstrViolation").text());
}

QDomElement ScenarioConstraints::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("finalSemiaxisError");
        child.appendChild(doc.createTextNode(convertToString(m_finalSemiaxisError);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("finalEccError");
        child.appendChild(doc.createTextNode(convertToString(m_finalEccError);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("finalInclError");
        child.appendChild(doc.createTextNode(convertToString(m_finalInclError);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("axialAccCstrViolation");
        child.appendChild(doc.createTextNode(convertToString(m_axialAccCstrViolation);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("heatFluxCstrViolation");
        child.appendChild(doc.createTextNode(convertToString(m_heatFluxCstrViolation);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("dynPressCstrViolation");
        child.appendChild(doc.createTextNode(convertToString(m_dynPressCstrViolation);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("controllabilityCstrViolation");
        child.appendChild(doc.createTextNode(convertToString(m_controllabilityCstrViolation);
        e.appendChild(child);
    }
}




// ScenarioInitialPositionType
ScenarioInitialPositionType::ScenarioInitialPositionType()
{
}

void ScenarioInitialPositionType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_CoordinateSystem = (e.firstChildElement("CoordinateSystem").text());
    m_Abstract6DOFPosition = QSharedPointer<ScenarioAbstract6DOFPositionType>(new ScenarioAbstract6DOFPositionType());
    m_Abstract6DOFPosition->load(e.firstChildElement("Abstract6DOFPosition"));
}

QDomElement ScenarioInitialPositionType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("CoordinateSystem");
        child.appendChild(doc.createTextNode(convertToString(m_CoordinateSystem);
        e.appendChild(child);
    }
    if (!m_Abstract6DOFPosition.isNull())
    {
        QDomElement child = m_Abstract6DOFPosition->toDomElement(doc);
        child.setTagName("Abstract6DOFPosition");
        e.appendChild(child);
    }
}




// ScenarioSC
ScenarioSC::ScenarioSC()
{
}

void ScenarioSC::load(const QDomElement& e)
{
    ScenarioParticipantsType::load(e);
    m_SCProgram = QSharedPointer<ScenarioSCProgram>(new ScenarioSCProgram());
    m_SCProgram->load(e.firstChildElement("SCProgram"));
    m_SCMission = QSharedPointer<ScenarioSCMission>(new ScenarioSCMission());
    m_SCMission->load(e.firstChildElement("SCMission"));
    m_System = QSharedPointer<ScenarioSCSystemType>(new ScenarioSCSystemType());
    m_System->load(e.firstChildElement("System"));
    m_Optimization = QSharedPointer<ScenarioOptimization>(new ScenarioOptimization());
    m_Optimization->load(e.firstChildElement("Optimization"));
    m_OutputFiles = QSharedPointer<ScenarioOutputFiles>(new ScenarioOutputFiles());
    m_OutputFiles->load(e.firstChildElement("OutputFiles"));
}

QDomElement ScenarioSC::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioParticipantsType::toDomElement(doc);
    if (!m_SCProgram.isNull())
    {
        QDomElement child = m_SCProgram->toDomElement(doc);
        child.setTagName("SCProgram");
        e.appendChild(child);
    }
    if (!m_SCMission.isNull())
    {
        QDomElement child = m_SCMission->toDomElement(doc);
        child.setTagName("SCMission");
        e.appendChild(child);
    }
    if (!m_System.isNull())
    {
        QDomElement child = m_System->toDomElement(doc);
        child.setTagName("System");
        e.appendChild(child);
    }
    if (!m_Optimization.isNull())
    {
        QDomElement child = m_Optimization->toDomElement(doc);
        child.setTagName("Optimization");
        e.appendChild(child);
    }
    if (!m_OutputFiles.isNull())
    {
        QDomElement child = m_OutputFiles->toDomElement(doc);
        child.setTagName("OutputFiles");
        e.appendChild(child);
    }
}




// ScenarioSCProgram
ScenarioSCProgram::ScenarioSCProgram()
{
}

void ScenarioSCProgram::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioSCProgram::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioSCMission
ScenarioSCMission::ScenarioSCMission()
{
}

void ScenarioSCMission::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Payload = QSharedPointer<ScenarioPayload>(new ScenarioPayload());
    m_Payload->load(e.firstChildElement("Payload"));
    m_TrajectoryPlan = QSharedPointer<ScenarioTrajectoryPlan>(new ScenarioTrajectoryPlan());
    m_TrajectoryPlan->load(e.firstChildElement("TrajectoryPlan"));
}

QDomElement ScenarioSCMission::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Payload.isNull())
    {
        QDomElement child = m_Payload->toDomElement(doc);
        child.setTagName("Payload");
        e.appendChild(child);
    }
    if (!m_TrajectoryPlan.isNull())
    {
        QDomElement child = m_TrajectoryPlan->toDomElement(doc);
        child.setTagName("TrajectoryPlan");
        e.appendChild(child);
    }
}




// ScenarioPayload
ScenarioPayload::ScenarioPayload() :
    m_mass(0.0),
    m_power(0.0)
{
}

void ScenarioPayload::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_mass = parseDouble(e.firstChildElement("mass").text());
    m_power = parseDouble(e.firstChildElement("power").text());
    m_ObservationAntenna = QSharedPointer<ScenarioObservationAntenna>(new ScenarioObservationAntenna());
    m_ObservationAntenna->load(e.firstChildElement("ObservationAntenna"));
}

QDomElement ScenarioPayload::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("mass");
        child.appendChild(doc.createTextNode(convertToString(m_mass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("power");
        child.appendChild(doc.createTextNode(convertToString(m_power);
        e.appendChild(child);
    }
    if (!m_ObservationAntenna.isNull())
    {
        QDomElement child = m_ObservationAntenna->toDomElement(doc);
        child.setTagName("ObservationAntenna");
        e.appendChild(child);
    }
}




// ScenarioTrajectoryPlan
ScenarioTrajectoryPlan::ScenarioTrajectoryPlan()
{
}

void ScenarioTrajectoryPlan::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    {
        QDomElement f = e.firstChildElement("AbstractTrajectory");        while (!f.isNull())
        {
            QSharedPointer<ScenarioAbstractTrajectoryType> v = QSharedPointer<ScenarioAbstractTrajectoryType>(new ScenarioAbstractTrajectoryType());
            v->load(e.firstChildElement("AbstractTrajectory"));
            m_AbstractTrajectory << v;
        }
}

QDomElement ScenarioTrajectoryPlan::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioAbstractTrajectoryType
ScenarioAbstractTrajectoryType::ScenarioAbstractTrajectoryType()
{
}

void ScenarioAbstractTrajectoryType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioAbstractTrajectoryType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioSCEnvironmentType
ScenarioSCEnvironmentType::ScenarioSCEnvironmentType()
{
}

void ScenarioSCEnvironmentType::load(const QDomElement& e)
{
    ScenarioEnvironmentType::load(e);
    m_perturbingBody = parseStringList(e.firstChildElement("perturbingBody").text());
    m_atmosphericDrag = parseBoolean(e.firstChildElement("atmosphericDrag").text());
    m_solarPressure = parseBoolean(e.firstChildElement("solarPressure").text());
}

QDomElement ScenarioSCEnvironmentType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioEnvironmentType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("perturbingBody");
        child.appendChild(doc.createTextNode(convertToString(m_perturbingBody);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("atmosphericDrag");
        child.appendChild(doc.createTextNode(convertToString(m_atmosphericDrag);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("solarPressure");
        child.appendChild(doc.createTextNode(convertToString(m_solarPressure);
        e.appendChild(child);
    }
}




// ScenarioLoiteringType
ScenarioLoiteringType::ScenarioLoiteringType()
{
}

void ScenarioLoiteringType::load(const QDomElement& e)
{
    ScenarioAbstractTrajectoryType::load(e);
    m_Environment = QSharedPointer<ScenarioSCEnvironmentType>(new ScenarioSCEnvironmentType());
    m_Environment->load(e.firstChildElement("Environment"));
    m_TimeLine = QSharedPointer<ScenarioTimeLine>(new ScenarioTimeLine());
    m_TimeLine->load(e.firstChildElement("TimeLine"));
    m_InitialPosition = QSharedPointer<ScenarioInitialPositionType>(new ScenarioInitialPositionType());
    m_InitialPosition->load(e.firstChildElement("InitialPosition"));
    m_InitialAttitude = QSharedPointer<ScenarioInitialAttitude>(new ScenarioInitialAttitude());
    m_InitialAttitude->load(e.firstChildElement("InitialAttitude"));
    m_PropagationPosition = QSharedPointer<ScenarioPropagationPosition>(new ScenarioPropagationPosition());
    m_PropagationPosition->load(e.firstChildElement("PropagationPosition"));
    m_PropagationAttitude = QSharedPointer<ScenarioPropagationAttitude>(new ScenarioPropagationAttitude());
    m_PropagationAttitude->load(e.firstChildElement("PropagationAttitude"));
}

QDomElement ScenarioLoiteringType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractTrajectoryType::toDomElement(doc);
    if (!m_Environment.isNull())
    {
        QDomElement child = m_Environment->toDomElement(doc);
        child.setTagName("Environment");
        e.appendChild(child);
    }
    if (!m_TimeLine.isNull())
    {
        QDomElement child = m_TimeLine->toDomElement(doc);
        child.setTagName("TimeLine");
        e.appendChild(child);
    }
    if (!m_InitialPosition.isNull())
    {
        QDomElement child = m_InitialPosition->toDomElement(doc);
        child.setTagName("InitialPosition");
        e.appendChild(child);
    }
    if (!m_InitialAttitude.isNull())
    {
        QDomElement child = m_InitialAttitude->toDomElement(doc);
        child.setTagName("InitialAttitude");
        e.appendChild(child);
    }
    if (!m_PropagationPosition.isNull())
    {
        QDomElement child = m_PropagationPosition->toDomElement(doc);
        child.setTagName("PropagationPosition");
        e.appendChild(child);
    }
    if (!m_PropagationAttitude.isNull())
    {
        QDomElement child = m_PropagationAttitude->toDomElement(doc);
        child.setTagName("PropagationAttitude");
        e.appendChild(child);
    }
}




// ScenarioInitialAttitude
ScenarioInitialAttitude::ScenarioInitialAttitude()
{
}

void ScenarioInitialAttitude::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_CoordinateSystem = (e.firstChildElement("CoordinateSystem").text());
    m_Abstract6DOFAttitude = QSharedPointer<ScenarioAbstract6DOFAttitudeType>(new ScenarioAbstract6DOFAttitudeType());
    m_Abstract6DOFAttitude->load(e.firstChildElement("Abstract6DOFAttitude"));
}

QDomElement ScenarioInitialAttitude::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("CoordinateSystem");
        child.appendChild(doc.createTextNode(convertToString(m_CoordinateSystem);
        e.appendChild(child);
    }
    if (!m_Abstract6DOFAttitude.isNull())
    {
        QDomElement child = m_Abstract6DOFAttitude->toDomElement(doc);
        child.setTagName("Abstract6DOFAttitude");
        e.appendChild(child);
    }
}




// ScenarioPropagationPosition
ScenarioPropagationPosition::ScenarioPropagationPosition() :
    m_timeStep(0.0)
{
}

void ScenarioPropagationPosition::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_propagator = (e.firstChildElement("propagator").text());
    m_integrator = (e.firstChildElement("integrator").text());
    m_timeStep = parseDouble(e.firstChildElement("timeStep").text());
}

QDomElement ScenarioPropagationPosition::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("propagator");
        child.appendChild(doc.createTextNode(convertToString(m_propagator);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("integrator");
        child.appendChild(doc.createTextNode(convertToString(m_integrator);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("timeStep");
        child.appendChild(doc.createTextNode(convertToString(m_timeStep);
        e.appendChild(child);
    }
}




// ScenarioPropagationAttitude
ScenarioPropagationAttitude::ScenarioPropagationAttitude() :
    m_timeStep(0.0)
{
}

void ScenarioPropagationAttitude::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_integrator = (e.firstChildElement("integrator").text());
    m_timeStep = parseDouble(e.firstChildElement("timeStep").text());
}

QDomElement ScenarioPropagationAttitude::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("integrator");
        child.appendChild(doc.createTextNode(convertToString(m_integrator);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("timeStep");
        child.appendChild(doc.createTextNode(convertToString(m_timeStep);
        e.appendChild(child);
    }
}




// ScenarioRendezvousType
ScenarioRendezvousType::ScenarioRendezvousType()
{
}

void ScenarioRendezvousType::load(const QDomElement& e)
{
    ScenarioAbstractTrajectoryType::load(e);
    m_Environment = QSharedPointer<ScenarioSCEnvironmentType>(new ScenarioSCEnvironmentType());
    m_Environment->load(e.firstChildElement("Environment"));
    m_Parameters = QSharedPointer<ScenarioParameters>(new ScenarioParameters());
    m_Parameters->load(e.firstChildElement("Parameters"));
    m_Target = (e.firstChildElement("Target").text());
    m_ManoeuvrePlan = QSharedPointer<ScenarioManoeuvrePlan>(new ScenarioManoeuvrePlan());
    m_ManoeuvrePlan->load(e.firstChildElement("ManoeuvrePlan"));
}

QDomElement ScenarioRendezvousType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractTrajectoryType::toDomElement(doc);
    if (!m_Environment.isNull())
    {
        QDomElement child = m_Environment->toDomElement(doc);
        child.setTagName("Environment");
        e.appendChild(child);
    }
    if (!m_Parameters.isNull())
    {
        QDomElement child = m_Parameters->toDomElement(doc);
        child.setTagName("Parameters");
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("Target");
        child.appendChild(doc.createTextNode(convertToString(m_Target);
        e.appendChild(child);
    }
    if (!m_ManoeuvrePlan.isNull())
    {
        QDomElement child = m_ManoeuvrePlan->toDomElement(doc);
        child.setTagName("ManoeuvrePlan");
        e.appendChild(child);
    }
}




// ScenarioParameters
ScenarioParameters::ScenarioParameters()
{
}

void ScenarioParameters::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_TimeLine = QSharedPointer<ScenarioTimeLine>(new ScenarioTimeLine());
    m_TimeLine->load(e.firstChildElement("TimeLine"));
    m_InitialPosition = QSharedPointer<ScenarioInitialPositionType>(new ScenarioInitialPositionType());
    m_InitialPosition->load(e.firstChildElement("InitialPosition"));
}

QDomElement ScenarioParameters::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_TimeLine.isNull())
    {
        QDomElement child = m_TimeLine->toDomElement(doc);
        child.setTagName("TimeLine");
        e.appendChild(child);
    }
    if (!m_InitialPosition.isNull())
    {
        QDomElement child = m_InitialPosition->toDomElement(doc);
        child.setTagName("InitialPosition");
        e.appendChild(child);
    }
}




// ScenarioManoeuvrePlan
ScenarioManoeuvrePlan::ScenarioManoeuvrePlan()
{
}

void ScenarioManoeuvrePlan::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    {
        QDomElement f = e.firstChildElement("AbstractManoeuvre");        while (!f.isNull())
        {
            QSharedPointer<ScenarioManoeuvreType> v = QSharedPointer<ScenarioManoeuvreType>(new ScenarioManoeuvreType());
            v->load(e.firstChildElement("AbstractManoeuvre"));
            m_AbstractManoeuvre << v;
        }
}

QDomElement ScenarioManoeuvrePlan::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioManoeuvreType
ScenarioManoeuvreType::ScenarioManoeuvreType()
{
}

void ScenarioManoeuvreType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}

QDomElement ScenarioManoeuvreType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




// ScenarioSTA_MANOEUVRE_DURATION
ScenarioSTA_MANOEUVRE_DURATION::ScenarioSTA_MANOEUVRE_DURATION() :
    m_Duration(0.0)
{
}

void ScenarioSTA_MANOEUVRE_DURATION::load(const QDomElement& e)
{
    ScenarioManoeuvreType::load(e);
    m_Duration = parseDouble(e.firstChildElement("Duration").text());
}

QDomElement ScenarioSTA_MANOEUVRE_DURATION::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioManoeuvreType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("Duration");
        child.appendChild(doc.createTextNode(convertToString(m_Duration);
        e.appendChild(child);
    }
}




// ScenarioSTA_MANOEUVRE_DELTAV
ScenarioSTA_MANOEUVRE_DELTAV::ScenarioSTA_MANOEUVRE_DELTAV() :
    m_DeltaVx(0.0),
    m_DeltaVy(0.0),
    m_DeltaVz(0.0),
    m_Duration(0.0)
{
}

void ScenarioSTA_MANOEUVRE_DELTAV::load(const QDomElement& e)
{
    ScenarioManoeuvreType::load(e);
    m_DeltaVx = parseDouble(e.firstChildElement("DeltaVx").text());
    m_DeltaVy = parseDouble(e.firstChildElement("DeltaVy").text());
    m_DeltaVz = parseDouble(e.firstChildElement("DeltaVz").text());
    m_Duration = parseDouble(e.firstChildElement("Duration").text());
}

QDomElement ScenarioSTA_MANOEUVRE_DELTAV::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioManoeuvreType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("DeltaVx");
        child.appendChild(doc.createTextNode(convertToString(m_DeltaVx);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("DeltaVy");
        child.appendChild(doc.createTextNode(convertToString(m_DeltaVy);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("DeltaVz");
        child.appendChild(doc.createTextNode(convertToString(m_DeltaVz);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("Duration");
        child.appendChild(doc.createTextNode(convertToString(m_Duration);
        e.appendChild(child);
    }
}




// ScenarioSTA_MANOEUVRE_V_POSITION
ScenarioSTA_MANOEUVRE_V_POSITION::ScenarioSTA_MANOEUVRE_V_POSITION() :
    m_DisplacementVbar(0.0)
{
}

void ScenarioSTA_MANOEUVRE_V_POSITION::load(const QDomElement& e)
{
    ScenarioManoeuvreType::load(e);
    m_DisplacementVbar = parseDouble(e.firstChildElement("DisplacementVbar").text());
}

QDomElement ScenarioSTA_MANOEUVRE_V_POSITION::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioManoeuvreType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("DisplacementVbar");
        child.appendChild(doc.createTextNode(convertToString(m_DisplacementVbar);
        e.appendChild(child);
    }
}




// ScenarioSTA_MANOEUVRE_R_POSITION
ScenarioSTA_MANOEUVRE_R_POSITION::ScenarioSTA_MANOEUVRE_R_POSITION() :
    m_DisplacementRbar(0.0)
{
}

void ScenarioSTA_MANOEUVRE_R_POSITION::load(const QDomElement& e)
{
    ScenarioManoeuvreType::load(e);
    m_DisplacementRbar = parseDouble(e.firstChildElement("DisplacementRbar").text());
}

QDomElement ScenarioSTA_MANOEUVRE_R_POSITION::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioManoeuvreType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("DisplacementRbar");
        child.appendChild(doc.createTextNode(convertToString(m_DisplacementRbar);
        e.appendChild(child);
    }
}




// ScenarioSTA_MANOEUVRE_V_R_POSITION
ScenarioSTA_MANOEUVRE_V_R_POSITION::ScenarioSTA_MANOEUVRE_V_R_POSITION() :
    m_DisplacementVbar(0.0),
    m_DisplacementRbar(0.0)
{
}

void ScenarioSTA_MANOEUVRE_V_R_POSITION::load(const QDomElement& e)
{
    ScenarioManoeuvreType::load(e);
    m_DisplacementVbar = parseDouble(e.firstChildElement("DisplacementVbar").text());
    m_DisplacementRbar = parseDouble(e.firstChildElement("DisplacementRbar").text());
}

QDomElement ScenarioSTA_MANOEUVRE_V_R_POSITION::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioManoeuvreType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("DisplacementVbar");
        child.appendChild(doc.createTextNode(convertToString(m_DisplacementVbar);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("DisplacementRbar");
        child.appendChild(doc.createTextNode(convertToString(m_DisplacementRbar);
        e.appendChild(child);
    }
}




// ScenarioSTA_MANOEUVRE_V_R_DURATION
ScenarioSTA_MANOEUVRE_V_R_DURATION::ScenarioSTA_MANOEUVRE_V_R_DURATION() :
    m_DisplacementVbar(0.0),
    m_DisplacementRbar(0.0),
    m_Duration(0.0)
{
}

void ScenarioSTA_MANOEUVRE_V_R_DURATION::load(const QDomElement& e)
{
    ScenarioManoeuvreType::load(e);
    m_DisplacementVbar = parseDouble(e.firstChildElement("DisplacementVbar").text());
    m_DisplacementRbar = parseDouble(e.firstChildElement("DisplacementRbar").text());
    m_Duration = parseDouble(e.firstChildElement("Duration").text());
}

QDomElement ScenarioSTA_MANOEUVRE_V_R_DURATION::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioManoeuvreType::toDomElement(doc);
    {
        QDomElement child = doc.createElement("DisplacementVbar");
        child.appendChild(doc.createTextNode(convertToString(m_DisplacementVbar);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("DisplacementRbar");
        child.appendChild(doc.createTextNode(convertToString(m_DisplacementRbar);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("Duration");
        child.appendChild(doc.createTextNode(convertToString(m_Duration);
        e.appendChild(child);
    }
}




// ScenarioLagrangianType
ScenarioLagrangianType::ScenarioLagrangianType()
{
}

void ScenarioLagrangianType::load(const QDomElement& e)
{
    ScenarioAbstractTrajectoryType::load(e);
}

QDomElement ScenarioLagrangianType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractTrajectoryType::toDomElement(doc);
}




// ScenarioFlyByType
ScenarioFlyByType::ScenarioFlyByType()
{
}

void ScenarioFlyByType::load(const QDomElement& e)
{
    ScenarioAbstractTrajectoryType::load(e);
}

QDomElement ScenarioFlyByType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioAbstractTrajectoryType::toDomElement(doc);
}




// ScenarioSCSystemType
ScenarioSCSystemType::ScenarioSCSystemType()
{
}

void ScenarioSCSystemType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_SystemBudgets = QSharedPointer<ScenarioSystemBudgets>(new ScenarioSystemBudgets());
    m_SystemBudgets->load(e.firstChildElement("SystemBudgets"));
    m_SCAerodynamics = QSharedPointer<ScenarioSCAerodynamics>(new ScenarioSCAerodynamics());
    m_SCAerodynamics->load(e.firstChildElement("SCAerodynamics"));
    m_Propulsion = QSharedPointer<ScenarioPropulsion>(new ScenarioPropulsion());
    m_Propulsion->load(e.firstChildElement("Propulsion"));
    m_Structure = QSharedPointer<ScenarioStructure>(new ScenarioStructure());
    m_Structure->load(e.firstChildElement("Structure"));
    m_TCS = QSharedPointer<ScenarioTCS>(new ScenarioTCS());
    m_TCS->load(e.firstChildElement("TCS"));
    m_EPS = QSharedPointer<ScenarioEPS>(new ScenarioEPS());
    m_EPS->load(e.firstChildElement("EPS"));
    m_TTC = QSharedPointer<ScenarioTTC>(new ScenarioTTC());
    m_TTC->load(e.firstChildElement("TTC"));
    m_AOCS = QSharedPointer<ScenarioAOCS>(new ScenarioAOCS());
    m_AOCS->load(e.firstChildElement("AOCS"));
    m_OBDH = QSharedPointer<ScenarioOBDH>(new ScenarioOBDH());
    m_OBDH->load(e.firstChildElement("OBDH"));
}

QDomElement ScenarioSCSystemType::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_SystemBudgets.isNull())
    {
        QDomElement child = m_SystemBudgets->toDomElement(doc);
        child.setTagName("SystemBudgets");
        e.appendChild(child);
    }
    if (!m_SCAerodynamics.isNull())
    {
        QDomElement child = m_SCAerodynamics->toDomElement(doc);
        child.setTagName("SCAerodynamics");
        e.appendChild(child);
    }
    if (!m_Propulsion.isNull())
    {
        QDomElement child = m_Propulsion->toDomElement(doc);
        child.setTagName("Propulsion");
        e.appendChild(child);
    }
    if (!m_Structure.isNull())
    {
        QDomElement child = m_Structure->toDomElement(doc);
        child.setTagName("Structure");
        e.appendChild(child);
    }
    if (!m_TCS.isNull())
    {
        QDomElement child = m_TCS->toDomElement(doc);
        child.setTagName("TCS");
        e.appendChild(child);
    }
    if (!m_EPS.isNull())
    {
        QDomElement child = m_EPS->toDomElement(doc);
        child.setTagName("EPS");
        e.appendChild(child);
    }
    if (!m_TTC.isNull())
    {
        QDomElement child = m_TTC->toDomElement(doc);
        child.setTagName("TTC");
        e.appendChild(child);
    }
    if (!m_AOCS.isNull())
    {
        QDomElement child = m_AOCS->toDomElement(doc);
        child.setTagName("AOCS");
        e.appendChild(child);
    }
    if (!m_OBDH.isNull())
    {
        QDomElement child = m_OBDH->toDomElement(doc);
        child.setTagName("OBDH");
        e.appendChild(child);
    }
}




// ScenarioSystemBudgets
ScenarioSystemBudgets::ScenarioSystemBudgets()
{
}

void ScenarioSystemBudgets::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_Mass = QSharedPointer<ScenarioMass>(new ScenarioMass());
    m_Mass->load(e.firstChildElement("Mass"));
    m_Power = QSharedPointer<ScenarioPower>(new ScenarioPower());
    m_Power->load(e.firstChildElement("Power"));
    m_Link = QSharedPointer<ScenarioLink>(new ScenarioLink());
    m_Link->load(e.firstChildElement("Link"));
}

QDomElement ScenarioSystemBudgets::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    if (!m_Mass.isNull())
    {
        QDomElement child = m_Mass->toDomElement(doc);
        child.setTagName("Mass");
        e.appendChild(child);
    }
    if (!m_Power.isNull())
    {
        QDomElement child = m_Power->toDomElement(doc);
        child.setTagName("Power");
        e.appendChild(child);
    }
    if (!m_Link.isNull())
    {
        QDomElement child = m_Link->toDomElement(doc);
        child.setTagName("Link");
        e.appendChild(child);
    }
}




// ScenarioMass
ScenarioMass::ScenarioMass() :
    m_dryMass(0.0),
    m_wetMass(0.0)
{
}

void ScenarioMass::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_dryMass = parseDouble(e.firstChildElement("dryMass").text());
    m_wetMass = parseDouble(e.firstChildElement("wetMass").text());
}

QDomElement ScenarioMass::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("dryMass");
        child.appendChild(doc.createTextNode(convertToString(m_dryMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("wetMass");
        child.appendChild(doc.createTextNode(convertToString(m_wetMass);
        e.appendChild(child);
    }
}




// ScenarioPower
ScenarioPower::ScenarioPower() :
    m_totalPowerBoL(0.0),
    m_totalPowerEoL(0.0)
{
}

void ScenarioPower::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalPowerBoL = parseDouble(e.firstChildElement("totalPowerBoL").text());
    m_totalPowerEoL = parseDouble(e.firstChildElement("totalPowerEoL").text());
}

QDomElement ScenarioPower::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalPowerBoL");
        child.appendChild(doc.createTextNode(convertToString(m_totalPowerBoL);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalPowerEoL");
        child.appendChild(doc.createTextNode(convertToString(m_totalPowerEoL);
        e.appendChild(child);
    }
}




// ScenarioLink
ScenarioLink::ScenarioLink() :
    m_uplinkMargin(0.0),
    m_downlinkMargin(0.0)
{
}

void ScenarioLink::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_uplinkMargin = parseDouble(e.firstChildElement("uplinkMargin").text());
    m_downlinkMargin = parseDouble(e.firstChildElement("downlinkMargin").text());
}

QDomElement ScenarioLink::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("uplinkMargin");
        child.appendChild(doc.createTextNode(convertToString(m_uplinkMargin);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("downlinkMargin");
        child.appendChild(doc.createTextNode(convertToString(m_downlinkMargin);
        e.appendChild(child);
    }
}




// ScenarioSCAerodynamics
ScenarioSCAerodynamics::ScenarioSCAerodynamics() :
    m_surfaceArea(0.0),
    m_Cd(0.0)
{
}

void ScenarioSCAerodynamics::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_surfaceArea = parseDouble(e.firstChildElement("surfaceArea").text());
    m_Cd = parseDouble(e.firstChildElement("Cd").text());
}

QDomElement ScenarioSCAerodynamics::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("surfaceArea");
        child.appendChild(doc.createTextNode(convertToString(m_surfaceArea);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("Cd");
        child.appendChild(doc.createTextNode(convertToString(m_Cd);
        e.appendChild(child);
    }
}




// ScenarioPropulsion
ScenarioPropulsion::ScenarioPropulsion() :
    m_numberOfEngines(0),
    m_thrustPerEngine(0.0),
    m_specificImpulse(0.0),
    m_propellantMass(0.0),
    m_totalPropulsionDryMass(0.0)
{
}

void ScenarioPropulsion::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_numberOfEngines = parseInt(e.firstChildElement("numberOfEngines").text());
    m_thrustPerEngine = parseDouble(e.firstChildElement("thrustPerEngine").text());
    m_specificImpulse = parseDouble(e.firstChildElement("specificImpulse").text());
    m_propellantMass = parseDouble(e.firstChildElement("propellantMass").text());
    m_totalPropulsionDryMass = parseDouble(e.firstChildElement("totalPropulsionDryMass").text());
}

QDomElement ScenarioPropulsion::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("numberOfEngines");
        child.appendChild(doc.createTextNode(convertToString(m_numberOfEngines);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("thrustPerEngine");
        child.appendChild(doc.createTextNode(convertToString(m_thrustPerEngine);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("specificImpulse");
        child.appendChild(doc.createTextNode(convertToString(m_specificImpulse);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("propellantMass");
        child.appendChild(doc.createTextNode(convertToString(m_propellantMass);
        e.appendChild(child);
    }
    {
        QDomElement child = doc.createElement("totalPropulsionDryMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalPropulsionDryMass);
        e.appendChild(child);
    }
}




// ScenarioStructure
ScenarioStructure::ScenarioStructure() :
    m_totalStructureMass(0.0)
{
}

void ScenarioStructure::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalStructureMass = parseDouble(e.firstChildElement("totalStructureMass").text());
}

QDomElement ScenarioStructure::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalStructureMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalStructureMass);
        e.appendChild(child);
    }
}




// ScenarioTCS
ScenarioTCS::ScenarioTCS() :
    m_totalTCSMass(0.0)
{
}

void ScenarioTCS::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalTCSMass = parseDouble(e.firstChildElement("totalTCSMass").text());
}

QDomElement ScenarioTCS::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalTCSMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalTCSMass);
        e.appendChild(child);
    }
}




// ScenarioEPS
ScenarioEPS::ScenarioEPS() :
    m_totalEPSMass(0.0)
{
}

void ScenarioEPS::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalEPSMass = parseDouble(e.firstChildElement("totalEPSMass").text());
}

QDomElement ScenarioEPS::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalEPSMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalEPSMass);
        e.appendChild(child);
    }
}




// ScenarioTTC
ScenarioTTC::ScenarioTTC() :
    m_totalTTCMass(0.0)
{
}

void ScenarioTTC::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalTTCMass = parseDouble(e.firstChildElement("totalTTCMass").text());
    {
        QDomElement f = e.firstChildElement("CommunicationAntenna");        while (!f.isNull())
        {
            QSharedPointer<ScenarioCommunicationAntenna> v = QSharedPointer<ScenarioCommunicationAntenna>(new ScenarioCommunicationAntenna());
            v->load(e.firstChildElement("CommunicationAntenna"));
            m_CommunicationAntenna << v;
        }
}

QDomElement ScenarioTTC::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalTTCMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalTTCMass);
        e.appendChild(child);
    }
}




// ScenarioAOCS
ScenarioAOCS::ScenarioAOCS() :
    m_totalAOCSMass(0.0)
{
}

void ScenarioAOCS::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalAOCSMass = parseDouble(e.firstChildElement("totalAOCSMass").text());
}

QDomElement ScenarioAOCS::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalAOCSMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalAOCSMass);
        e.appendChild(child);
    }
}




// ScenarioOBDH
ScenarioOBDH::ScenarioOBDH() :
    m_totalOBDHMass(0.0)
{
}

void ScenarioOBDH::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_totalOBDHMass = parseDouble(e.firstChildElement("totalOBDHMass").text());
}

QDomElement ScenarioOBDH::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
    {
        QDomElement child = doc.createElement("totalOBDHMass");
        child.appendChild(doc.createTextNode(convertToString(m_totalOBDHMass);
        e.appendChild(child);
    }
}




// ScenarioSpaceScenario
ScenarioSpaceScenario::ScenarioSpaceScenario()
{
}

void ScenarioSpaceScenario::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    {
        QDomElement f = e.firstChildElement("AbstractParticipants");        while (!f.isNull())
        {
            QSharedPointer<ScenarioParticipantsType> v = QSharedPointer<ScenarioParticipantsType>(new ScenarioParticipantsType());
            v->load(e.firstChildElement("AbstractParticipants"));
            m_AbstractParticipants << v;
        }
}

QDomElement ScenarioSpaceScenario::toDomElement(QDomDocument& doc) const
{
    QDomElement e = ScenarioObject::toDomElement(doc);
}




