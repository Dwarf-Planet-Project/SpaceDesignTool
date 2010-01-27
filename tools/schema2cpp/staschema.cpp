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




// ScenarioOptVarBool
ScenarioOptVarBool::ScenarioOptVarBool()
{
}

void ScenarioOptVarBool::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_value = parseBoolean(e.firstChildElement("value").text());
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




// ScenarioPropagation
ScenarioPropagation::ScenarioPropagation()
{
}

void ScenarioPropagation::load(const QDomElement& e)
{
    ScenarioObject::load(e);
    m_propagator = (e.firstChildElement("propagator").text());
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




// ScenarioAbstract3DOFPositionType
ScenarioAbstract3DOFPositionType::ScenarioAbstract3DOFPositionType()
{
}

void ScenarioAbstract3DOFPositionType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
}




// ScenarioAbstract6DOFPositionType
ScenarioAbstract6DOFPositionType::ScenarioAbstract6DOFPositionType()
{
}

void ScenarioAbstract6DOFPositionType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioAbstract6DOFAttitudeType
ScenarioAbstract6DOFAttitudeType::ScenarioAbstract6DOFAttitudeType()
{
}

void ScenarioAbstract6DOFAttitudeType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioAbstract12DOFStateType
ScenarioAbstract12DOFStateType::ScenarioAbstract12DOFStateType()
{
}

void ScenarioAbstract12DOFStateType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioAbstractConeType
ScenarioAbstractConeType::ScenarioAbstractConeType()
{
}

void ScenarioAbstractConeType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioOvalConeType
ScenarioOvalConeType::ScenarioOvalConeType()
{
}

void ScenarioOvalConeType::load(const QDomElement& e)
{
    ScenarioAbstractConeType::load(e);
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




// ScenarioCommunicationAntenna
ScenarioCommunicationAntenna::ScenarioCommunicationAntenna()
{
}

void ScenarioCommunicationAntenna::load(const QDomElement& e)
{
    ScenarioAntenna::load(e);
}




// ScenarioObservationAntenna
ScenarioObservationAntenna::ScenarioObservationAntenna()
{
}

void ScenarioObservationAntenna::load(const QDomElement& e)
{
    ScenarioAntenna::load(e);
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




// ScenarioLocation
ScenarioLocation::ScenarioLocation()
{
}

void ScenarioLocation::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioSCProgram
ScenarioSCProgram::ScenarioSCProgram()
{
}

void ScenarioSCProgram::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioAbstractTrajectoryType
ScenarioAbstractTrajectoryType::ScenarioAbstractTrajectoryType()
{
}

void ScenarioAbstractTrajectoryType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioManoeuvreType
ScenarioManoeuvreType::ScenarioManoeuvreType()
{
}

void ScenarioManoeuvreType::load(const QDomElement& e)
{
    ScenarioObject::load(e);
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




// ScenarioLagrangianType
ScenarioLagrangianType::ScenarioLagrangianType()
{
}

void ScenarioLagrangianType::load(const QDomElement& e)
{
    ScenarioAbstractTrajectoryType::load(e);
}




// ScenarioFlyByType
ScenarioFlyByType::ScenarioFlyByType()
{
}

void ScenarioFlyByType::load(const QDomElement& e)
{
    ScenarioAbstractTrajectoryType::load(e);
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




