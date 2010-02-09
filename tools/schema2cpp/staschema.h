class QDomElement;
#include "ScenarioParse.h"

class ScenarioObject
{
    public:
    void load(const QDomElement& /* e */) {}
    QDomElement toDomElement(QDomDocument& doc)
    {
        return doc.createElement("Object");
    }
};

// Forward declarations

class ScenarioParticipantsType;
class ScenarioOptVarDouble;
class ScenarioOptVarInt;
class ScenarioOptVarBool;
class ScenarioOptVarString;
class ScenarioTimeLine;
class ScenarioPropagation;
class ScenarioEnvironmentType;
class ScenarioCentralBodyType;
class ScenarioGravityModel;
class ScenarioAbstract3DOFPositionType;
class ScenarioAbstract6DOFPositionType;
class ScenarioGroundPositionType;
class ScenarioStateVectorType;
class ScenarioKeplerianElementsType;
class ScenarioSphericalCoordinatesType;
class ScenarioAbstract6DOFAttitudeType;
class ScenarioEulerBLVLHType;
class ScenarioEulerBIType;
class ScenarioqBLVLHType;
class ScenarioqBIType;
class ScenarioAbstract12DOFStateType;
class ScenarioState12DOF;
class ScenarioOptimization;
class ScenarioOutputFiles;
class ScenarioAbstractConeType;
class ScenarioCircularCone;
class ScenarioRectangularConeType;
class ScenarioOvalConeType;
class ScenarioAntenna;
class ScenarioPointingDirection;
class ScenarioCommunicationAntenna;
class ScenarioObservationAntenna;
class ScenarioLocationType;
class ScenarioGroundStation;
class ScenarioLaunchPad;
class ScenarioPoint;
class ScenarioLocation;
class ScenarioLV;
class ScenarioPropulsionSystem;
class ScenarioSystem;
class ScenarioLiquidTanks;
class ScenarioSolidGrain;
class ScenarioFeedSystem;
class ScenarioCombustionChamber;
class ScenarioNozzle;
class ScenarioPerformance;
class ScenarioLVProgram;
class ScenarioprogramCostFactors;
class ScenarioLVMission;
class ScenarioLVPayload;
class ScenarioLaunchSite;
class ScenarioTargetOrbit;
class ScenarioLVSystemType;
class ScenarioArchitecture;
class ScenarioLowerStage;
class ScenarioUpperStage;
class ScenarioBoosters;
class ScenarioDiscretizationSettings;
class ScenarioGeometry;
class ScenarioLVAerodynamics;
class ScenarioComponentWeights;
class ScenarioSystemWeights;
class ScenarioComponentCosts;
class ScenarioDevelopmentCosts;
class ScenarioProductionCosts;
class ScenarioSystemCosts;
class ScenarioTotalProgramCosts;
class ScenarioDirectOperationsCosts;
class ScenarioIndirectOperationsCosts;
class ScenarioComponentReliability;
class ScenarioSeparation;
class ScenarioPropulsionReliability;
class ScenarioSystemReliability;
class ScenarioLaunch;
class ScenarioTrajectory;
class ScenarioTakeOff;
class ScenarioIgnitions;
class ScenarioAtmosphericFlight;
class ScenarioExoatmosphericFlight;
class ScenarioConstraints;
class ScenarioInitialPositionType;
class ScenarioSC;
class ScenarioSCProgram;
class ScenarioSCMission;
class ScenarioPayload;
class ScenarioTrajectoryPlan;
class ScenarioAbstractTrajectoryType;
class ScenarioSCEnvironmentType;
class ScenarioLoiteringType;
class ScenarioInitialAttitude;
class ScenarioPropagationPosition;
class ScenarioPropagationAttitude;
class ScenarioRendezvousType;
class ScenarioParameters;
class ScenarioManoeuvrePlan;
class ScenarioManoeuvreType;
class ScenarioSTA_MANOEUVRE_DURATION;
class ScenarioSTA_MANOEUVRE_DELTAV;
class ScenarioSTA_MANOEUVRE_V_POSITION;
class ScenarioSTA_MANOEUVRE_R_POSITION;
class ScenarioSTA_MANOEUVRE_V_R_POSITION;
class ScenarioSTA_MANOEUVRE_V_R_DURATION;
class ScenarioLagrangianType;
class ScenarioFlyByType;
class ScenarioSCSystemType;
class ScenarioSystemBudgets;
class ScenarioMass;
class ScenarioPower;
class ScenarioLink;
class ScenarioSCAerodynamics;
class ScenarioPropulsion;
class ScenarioStructure;
class ScenarioTCS;
class ScenarioEPS;
class ScenarioTTC;
class ScenarioAOCS;
class ScenarioOBDH;
class ScenarioSpaceScenario;


// Space scenario class definitions

// ScenarioParticipantsType
class ScenarioParticipantsType : public ScenarioObject
{
public:
    ScenarioParticipantsType();
    void load(const QDomElement& e);

    QString Name() const
    { return m_Name; }
    void setName(QString Name)
    { m_Name = Name; }

private:
    QString m_Name;
};


// ScenarioOptVarDouble
class ScenarioOptVarDouble : public ScenarioObject
{
public:
    ScenarioOptVarDouble();
    void load(const QDomElement& e);

    double min() const
    { return m_min; }
    void setMin(double min)
    { m_min = min; }
    double value() const
    { return m_value; }
    void setValue(double value)
    { m_value = value; }
    double max() const
    { return m_max; }
    void setMax(double max)
    { m_max = max; }

private:
    double m_min;
    double m_value;
    double m_max;
};


// ScenarioOptVarInt
class ScenarioOptVarInt : public ScenarioObject
{
public:
    ScenarioOptVarInt();
    void load(const QDomElement& e);

    const QList<int>& list() const
    { return m_list; }
    QList<int>& list()
    { return m_list; }
    void setList(QList<int> list)
    { m_list = list; }
    int value() const
    { return m_value; }
    void setValue(int value)
    { m_value = value; }

private:
    QList<int> m_list;
    int m_value;
};


// ScenarioOptVarBool
class ScenarioOptVarBool : public ScenarioObject
{
public:
    ScenarioOptVarBool();
    void load(const QDomElement& e);

    bool value() const
    { return m_value; }
    void setValue(bool value)
    { m_value = value; }

private:
    bool m_value;
};


// ScenarioOptVarString
class ScenarioOptVarString : public ScenarioObject
{
public:
    ScenarioOptVarString();
    void load(const QDomElement& e);

    const QList<QString>& list() const
    { return m_list; }
    QList<QString>& list()
    { return m_list; }
    void setList(QList<QString> list)
    { m_list = list; }
    QString value() const
    { return m_value; }
    void setValue(QString value)
    { m_value = value; }

private:
    QList<QString> m_list;
    QString m_value;
};


// ScenarioTimeLine
class ScenarioTimeLine : public ScenarioObject
{
public:
    ScenarioTimeLine();
    void load(const QDomElement& e);

    QDateTime StartTime() const
    { return m_StartTime; }
    void setStartTime(QDateTime StartTime)
    { m_StartTime = StartTime; }
    double StepTime() const
    { return m_StepTime; }
    void setStepTime(double StepTime)
    { m_StepTime = StepTime; }
    QDateTime EndTime() const
    { return m_EndTime; }
    void setEndTime(QDateTime EndTime)
    { m_EndTime = EndTime; }

private:
    QDateTime m_StartTime;
    double m_StepTime;
    QDateTime m_EndTime;
};


// ScenarioPropagation
class ScenarioPropagation : public ScenarioObject
{
public:
    ScenarioPropagation();
    void load(const QDomElement& e);

    QString propagator() const
    { return m_propagator; }
    void setPropagator(QString propagator)
    { m_propagator = propagator; }

private:
    QString m_propagator;
};


// ScenarioEnvironmentType
class ScenarioEnvironmentType : public ScenarioObject
{
public:
    ScenarioEnvironmentType();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioCentralBodyType> CentralBody() const
    { return m_CentralBody; }
    void setCentralBody(QSharedPointer<ScenarioCentralBodyType> CentralBody)
    { m_CentralBody = CentralBody; }

private:
    QSharedPointer<ScenarioCentralBodyType> m_CentralBody;
};


// ScenarioCentralBodyType
class ScenarioCentralBodyType : public ScenarioObject
{
public:
    ScenarioCentralBodyType();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioGravityModel> GravityModel() const
    { return m_GravityModel; }
    void setGravityModel(QSharedPointer<ScenarioGravityModel> GravityModel)
    { m_GravityModel = GravityModel; }
    QString atmosphere() const
    { return m_atmosphere; }
    void setAtmosphere(QString atmosphere)
    { m_atmosphere = atmosphere; }
    bool ellipticity() const
    { return m_ellipticity; }
    void setEllipticity(bool ellipticity)
    { m_ellipticity = ellipticity; }

private:
    QSharedPointer<ScenarioGravityModel> m_GravityModel;
    QString m_atmosphere;
    bool m_ellipticity;
};


// ScenarioGravityModel
class ScenarioGravityModel : public ScenarioObject
{
public:
    ScenarioGravityModel();
    void load(const QDomElement& e);

    QString modelName() const
    { return m_modelName; }
    void setModelName(QString modelName)
    { m_modelName = modelName; }
    int numberOfZonals() const
    { return m_numberOfZonals; }
    void setNumberOfZonals(int numberOfZonals)
    { m_numberOfZonals = numberOfZonals; }
    int numberOfTesserals() const
    { return m_numberOfTesserals; }
    void setNumberOfTesserals(int numberOfTesserals)
    { m_numberOfTesserals = numberOfTesserals; }

private:
    QString m_modelName;
    int m_numberOfZonals;
    int m_numberOfTesserals;
};


// ScenarioAbstract3DOFPositionType
class ScenarioAbstract3DOFPositionType : public ScenarioObject
{
public:
    ScenarioAbstract3DOFPositionType();
    void load(const QDomElement& e);


private:
};


// ScenarioAbstract6DOFPositionType
class ScenarioAbstract6DOFPositionType : public ScenarioObject
{
public:
    ScenarioAbstract6DOFPositionType();
    void load(const QDomElement& e);


private:
};


// ScenarioGroundPositionType
class ScenarioGroundPositionType : public ScenarioAbstract3DOFPositionType
{
public:
    ScenarioGroundPositionType();
    void load(const QDomElement& e);

    double latitude() const
    { return m_latitude; }
    void setLatitude(double latitude)
    { m_latitude = latitude; }
    double longitude() const
    { return m_longitude; }
    void setLongitude(double longitude)
    { m_longitude = longitude; }
    double altitude() const
    { return m_altitude; }
    void setAltitude(double altitude)
    { m_altitude = altitude; }

private:
    double m_latitude;
    double m_longitude;
    double m_altitude;
};


// ScenarioStateVectorType
class ScenarioStateVectorType : public ScenarioAbstract6DOFPositionType
{
public:
    ScenarioStateVectorType();
    void load(const QDomElement& e);

    double x() const
    { return m_x; }
    void setX(double x)
    { m_x = x; }
    double y() const
    { return m_y; }
    void setY(double y)
    { m_y = y; }
    double z() const
    { return m_z; }
    void setZ(double z)
    { m_z = z; }
    double vx() const
    { return m_vx; }
    void setVx(double vx)
    { m_vx = vx; }
    double vy() const
    { return m_vy; }
    void setVy(double vy)
    { m_vy = vy; }
    double vz() const
    { return m_vz; }
    void setVz(double vz)
    { m_vz = vz; }

private:
    double m_x;
    double m_y;
    double m_z;
    double m_vx;
    double m_vy;
    double m_vz;
};


// ScenarioKeplerianElementsType
class ScenarioKeplerianElementsType : public ScenarioAbstract6DOFPositionType
{
public:
    ScenarioKeplerianElementsType();
    void load(const QDomElement& e);

    double semiMajorAxis() const
    { return m_semiMajorAxis; }
    void setSemiMajorAxis(double semiMajorAxis)
    { m_semiMajorAxis = semiMajorAxis; }
    double eccentricity() const
    { return m_eccentricity; }
    void setEccentricity(double eccentricity)
    { m_eccentricity = eccentricity; }
    double inclination() const
    { return m_inclination; }
    void setInclination(double inclination)
    { m_inclination = inclination; }
    double RAAN() const
    { return m_RAAN; }
    void setRAAN(double RAAN)
    { m_RAAN = RAAN; }
    double argumentOfPeriapsis() const
    { return m_argumentOfPeriapsis; }
    void setArgumentOfPeriapsis(double argumentOfPeriapsis)
    { m_argumentOfPeriapsis = argumentOfPeriapsis; }
    double trueAnomaly() const
    { return m_trueAnomaly; }
    void setTrueAnomaly(double trueAnomaly)
    { m_trueAnomaly = trueAnomaly; }

private:
    double m_semiMajorAxis;
    double m_eccentricity;
    double m_inclination;
    double m_RAAN;
    double m_argumentOfPeriapsis;
    double m_trueAnomaly;
};


// ScenarioSphericalCoordinatesType
class ScenarioSphericalCoordinatesType : public ScenarioAbstract6DOFPositionType
{
public:
    ScenarioSphericalCoordinatesType();
    void load(const QDomElement& e);

    double radialDistance() const
    { return m_radialDistance; }
    void setRadialDistance(double radialDistance)
    { m_radialDistance = radialDistance; }
    double longitude() const
    { return m_longitude; }
    void setLongitude(double longitude)
    { m_longitude = longitude; }
    double latitude() const
    { return m_latitude; }
    void setLatitude(double latitude)
    { m_latitude = latitude; }
    double flightPathVelocity() const
    { return m_flightPathVelocity; }
    void setFlightPathVelocity(double flightPathVelocity)
    { m_flightPathVelocity = flightPathVelocity; }
    double flightPathAngle() const
    { return m_flightPathAngle; }
    void setFlightPathAngle(double flightPathAngle)
    { m_flightPathAngle = flightPathAngle; }
    double headingAngle() const
    { return m_headingAngle; }
    void setHeadingAngle(double headingAngle)
    { m_headingAngle = headingAngle; }

private:
    double m_radialDistance;
    double m_longitude;
    double m_latitude;
    double m_flightPathVelocity;
    double m_flightPathAngle;
    double m_headingAngle;
};


// ScenarioAbstract6DOFAttitudeType
class ScenarioAbstract6DOFAttitudeType : public ScenarioObject
{
public:
    ScenarioAbstract6DOFAttitudeType();
    void load(const QDomElement& e);


private:
};


// ScenarioEulerBLVLHType
class ScenarioEulerBLVLHType : public ScenarioAbstract6DOFAttitudeType
{
public:
    ScenarioEulerBLVLHType();
    void load(const QDomElement& e);

    double phi() const
    { return m_phi; }
    void setPhi(double phi)
    { m_phi = phi; }
    double theta() const
    { return m_theta; }
    void setTheta(double theta)
    { m_theta = theta; }
    double psi() const
    { return m_psi; }
    void setPsi(double psi)
    { m_psi = psi; }
    double phiDot() const
    { return m_phiDot; }
    void setPhiDot(double phiDot)
    { m_phiDot = phiDot; }
    double thetaDot() const
    { return m_thetaDot; }
    void setThetaDot(double thetaDot)
    { m_thetaDot = thetaDot; }
    double psiDot() const
    { return m_psiDot; }
    void setPsiDot(double psiDot)
    { m_psiDot = psiDot; }

private:
    double m_phi;
    double m_theta;
    double m_psi;
    double m_phiDot;
    double m_thetaDot;
    double m_psiDot;
};


// ScenarioEulerBIType
class ScenarioEulerBIType : public ScenarioAbstract6DOFAttitudeType
{
public:
    ScenarioEulerBIType();
    void load(const QDomElement& e);

    double phi() const
    { return m_phi; }
    void setPhi(double phi)
    { m_phi = phi; }
    double theta() const
    { return m_theta; }
    void setTheta(double theta)
    { m_theta = theta; }
    double psi() const
    { return m_psi; }
    void setPsi(double psi)
    { m_psi = psi; }
    double phiDot() const
    { return m_phiDot; }
    void setPhiDot(double phiDot)
    { m_phiDot = phiDot; }
    double thetaDot() const
    { return m_thetaDot; }
    void setThetaDot(double thetaDot)
    { m_thetaDot = thetaDot; }
    double psiDot() const
    { return m_psiDot; }
    void setPsiDot(double psiDot)
    { m_psiDot = psiDot; }

private:
    double m_phi;
    double m_theta;
    double m_psi;
    double m_phiDot;
    double m_thetaDot;
    double m_psiDot;
};


// ScenarioqBLVLHType
class ScenarioqBLVLHType : public ScenarioAbstract6DOFAttitudeType
{
public:
    ScenarioqBLVLHType();
    void load(const QDomElement& e);

    double q1() const
    { return m_q1; }
    void setQ1(double q1)
    { m_q1 = q1; }
    double q2() const
    { return m_q2; }
    void setQ2(double q2)
    { m_q2 = q2; }
    double q3() const
    { return m_q3; }
    void setQ3(double q3)
    { m_q3 = q3; }
    double q4() const
    { return m_q4; }
    void setQ4(double q4)
    { m_q4 = q4; }
    double q1Dot() const
    { return m_q1Dot; }
    void setQ1Dot(double q1Dot)
    { m_q1Dot = q1Dot; }
    double q2Dot() const
    { return m_q2Dot; }
    void setQ2Dot(double q2Dot)
    { m_q2Dot = q2Dot; }
    double q3Dot() const
    { return m_q3Dot; }
    void setQ3Dot(double q3Dot)
    { m_q3Dot = q3Dot; }
    double q4Dot() const
    { return m_q4Dot; }
    void setQ4Dot(double q4Dot)
    { m_q4Dot = q4Dot; }

private:
    double m_q1;
    double m_q2;
    double m_q3;
    double m_q4;
    double m_q1Dot;
    double m_q2Dot;
    double m_q3Dot;
    double m_q4Dot;
};


// ScenarioqBIType
class ScenarioqBIType : public ScenarioAbstract6DOFAttitudeType
{
public:
    ScenarioqBIType();
    void load(const QDomElement& e);

    double q1() const
    { return m_q1; }
    void setQ1(double q1)
    { m_q1 = q1; }
    double q2() const
    { return m_q2; }
    void setQ2(double q2)
    { m_q2 = q2; }
    double q3() const
    { return m_q3; }
    void setQ3(double q3)
    { m_q3 = q3; }
    double q4() const
    { return m_q4; }
    void setQ4(double q4)
    { m_q4 = q4; }
    double q1Dot() const
    { return m_q1Dot; }
    void setQ1Dot(double q1Dot)
    { m_q1Dot = q1Dot; }
    double q2Dot() const
    { return m_q2Dot; }
    void setQ2Dot(double q2Dot)
    { m_q2Dot = q2Dot; }
    double q3Dot() const
    { return m_q3Dot; }
    void setQ3Dot(double q3Dot)
    { m_q3Dot = q3Dot; }
    double q4Dot() const
    { return m_q4Dot; }
    void setQ4Dot(double q4Dot)
    { m_q4Dot = q4Dot; }

private:
    double m_q1;
    double m_q2;
    double m_q3;
    double m_q4;
    double m_q1Dot;
    double m_q2Dot;
    double m_q3Dot;
    double m_q4Dot;
};


// ScenarioAbstract12DOFStateType
class ScenarioAbstract12DOFStateType : public ScenarioObject
{
public:
    ScenarioAbstract12DOFStateType();
    void load(const QDomElement& e);


private:
};


// ScenarioState12DOF
class ScenarioState12DOF : public ScenarioAbstract12DOFStateType
{
public:
    ScenarioState12DOF();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioAbstract6DOFPositionType> Abstract6DOFPosition() const
    { return m_Abstract6DOFPosition; }
    void setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType> Abstract6DOFPosition)
    { m_Abstract6DOFPosition = Abstract6DOFPosition; }
    QSharedPointer<ScenarioAbstract6DOFAttitudeType> Abstract6DOFAttitude() const
    { return m_Abstract6DOFAttitude; }
    void setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType> Abstract6DOFAttitude)
    { m_Abstract6DOFAttitude = Abstract6DOFAttitude; }

private:
    QSharedPointer<ScenarioAbstract6DOFPositionType> m_Abstract6DOFPosition;
    QSharedPointer<ScenarioAbstract6DOFAttitudeType> m_Abstract6DOFAttitude;
};


// ScenarioOptimization
class ScenarioOptimization : public ScenarioObject
{
public:
    ScenarioOptimization();
    void load(const QDomElement& e);

    int nVariables() const
    { return m_nVariables; }
    void setNVariables(int nVariables)
    { m_nVariables = nVariables; }
    const QList<QString>& variables() const
    { return m_variables; }
    QList<QString>& variables()
    { return m_variables; }
    void setVariables(QList<QString> variables)
    { m_variables = variables; }
    const QList<double>& variablesMatrix() const
    { return m_variablesMatrix; }
    QList<double>& variablesMatrix()
    { return m_variablesMatrix; }
    void setVariablesMatrix(QList<double> variablesMatrix)
    { m_variablesMatrix = variablesMatrix; }
    int nObjectives() const
    { return m_nObjectives; }
    void setNObjectives(int nObjectives)
    { m_nObjectives = nObjectives; }
    const QList<QString>& objectives() const
    { return m_objectives; }
    QList<QString>& objectives()
    { return m_objectives; }
    void setObjectives(QList<QString> objectives)
    { m_objectives = objectives; }
    const QList<double>& objectivesMatrix() const
    { return m_objectivesMatrix; }
    QList<double>& objectivesMatrix()
    { return m_objectivesMatrix; }
    void setObjectivesMatrix(QList<double> objectivesMatrix)
    { m_objectivesMatrix = objectivesMatrix; }
    int nConstraints() const
    { return m_nConstraints; }
    void setNConstraints(int nConstraints)
    { m_nConstraints = nConstraints; }
    const QList<QString>& constraints() const
    { return m_constraints; }
    QList<QString>& constraints()
    { return m_constraints; }
    void setConstraints(QList<QString> constraints)
    { m_constraints = constraints; }
    const QList<double>& constraintsMatrix() const
    { return m_constraintsMatrix; }
    QList<double>& constraintsMatrix()
    { return m_constraintsMatrix; }
    void setConstraintsMatrix(QList<double> constraintsMatrix)
    { m_constraintsMatrix = constraintsMatrix; }
    QString algorithm() const
    { return m_algorithm; }
    void setAlgorithm(QString algorithm)
    { m_algorithm = algorithm; }
    const QList<double>& algorithmParameters() const
    { return m_algorithmParameters; }
    QList<double>& algorithmParameters()
    { return m_algorithmParameters; }
    void setAlgorithmParameters(QList<double> algorithmParameters)
    { m_algorithmParameters = algorithmParameters; }

private:
    int m_nVariables;
    QList<QString> m_variables;
    QList<double> m_variablesMatrix;
    int m_nObjectives;
    QList<QString> m_objectives;
    QList<double> m_objectivesMatrix;
    int m_nConstraints;
    QList<QString> m_constraints;
    QList<double> m_constraintsMatrix;
    QString m_algorithm;
    QList<double> m_algorithmParameters;
};


// ScenarioOutputFiles
class ScenarioOutputFiles : public ScenarioObject
{
public:
    ScenarioOutputFiles();
    void load(const QDomElement& e);

    QString paretoFrontVarsFileName() const
    { return m_paretoFrontVarsFileName; }
    void setParetoFrontVarsFileName(QString paretoFrontVarsFileName)
    { m_paretoFrontVarsFileName = paretoFrontVarsFileName; }
    QString paretoFrontCstrsFileName() const
    { return m_paretoFrontCstrsFileName; }
    void setParetoFrontCstrsFileName(QString paretoFrontCstrsFileName)
    { m_paretoFrontCstrsFileName = paretoFrontCstrsFileName; }
    QString paretoFrontObjsFileName() const
    { return m_paretoFrontObjsFileName; }
    void setParetoFrontObjsFileName(QString paretoFrontObjsFileName)
    { m_paretoFrontObjsFileName = paretoFrontObjsFileName; }
    QString geometryFileName() const
    { return m_geometryFileName; }
    void setGeometryFileName(QString geometryFileName)
    { m_geometryFileName = geometryFileName; }
    QString trajPostionFileName() const
    { return m_trajPostionFileName; }
    void setTrajPostionFileName(QString trajPostionFileName)
    { m_trajPostionFileName = trajPostionFileName; }
    QString trajAttitudeFileName() const
    { return m_trajAttitudeFileName; }
    void setTrajAttitudeFileName(QString trajAttitudeFileName)
    { m_trajAttitudeFileName = trajAttitudeFileName; }
    QString trajMiscFileName() const
    { return m_trajMiscFileName; }
    void setTrajMiscFileName(QString trajMiscFileName)
    { m_trajMiscFileName = trajMiscFileName; }
    QString successProbabilityFileName() const
    { return m_successProbabilityFileName; }
    void setSuccessProbabilityFileName(QString successProbabilityFileName)
    { m_successProbabilityFileName = successProbabilityFileName; }

private:
    QString m_paretoFrontVarsFileName;
    QString m_paretoFrontCstrsFileName;
    QString m_paretoFrontObjsFileName;
    QString m_geometryFileName;
    QString m_trajPostionFileName;
    QString m_trajAttitudeFileName;
    QString m_trajMiscFileName;
    QString m_successProbabilityFileName;
};


// ScenarioAbstractConeType
class ScenarioAbstractConeType : public ScenarioObject
{
public:
    ScenarioAbstractConeType();
    void load(const QDomElement& e);


private:
};


// ScenarioCircularCone
class ScenarioCircularCone : public ScenarioAbstractConeType
{
public:
    ScenarioCircularCone();
    void load(const QDomElement& e);

    double coneAngle() const
    { return m_coneAngle; }
    void setConeAngle(double coneAngle)
    { m_coneAngle = coneAngle; }

private:
    double m_coneAngle;
};


// ScenarioRectangularConeType
class ScenarioRectangularConeType : public ScenarioAbstractConeType
{
public:
    ScenarioRectangularConeType();
    void load(const QDomElement& e);

    double coneAngle1() const
    { return m_coneAngle1; }
    void setConeAngle1(double coneAngle1)
    { m_coneAngle1 = coneAngle1; }
    double coneAngle2() const
    { return m_coneAngle2; }
    void setConeAngle2(double coneAngle2)
    { m_coneAngle2 = coneAngle2; }

private:
    double m_coneAngle1;
    double m_coneAngle2;
};


// ScenarioOvalConeType
class ScenarioOvalConeType : public ScenarioAbstractConeType
{
public:
    ScenarioOvalConeType();
    void load(const QDomElement& e);


private:
};


// ScenarioAntenna
class ScenarioAntenna : public ScenarioObject
{
public:
    ScenarioAntenna();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioAbstractConeType> AbstractCone() const
    { return m_AbstractCone; }
    void setAbstractCone(QSharedPointer<ScenarioAbstractConeType> AbstractCone)
    { m_AbstractCone = AbstractCone; }
    QSharedPointer<ScenarioPointingDirection> PointingDirection() const
    { return m_PointingDirection; }
    void setPointingDirection(QSharedPointer<ScenarioPointingDirection> PointingDirection)
    { m_PointingDirection = PointingDirection; }

private:
    QSharedPointer<ScenarioAbstractConeType> m_AbstractCone;
    QSharedPointer<ScenarioPointingDirection> m_PointingDirection;
};


// ScenarioPointingDirection
class ScenarioPointingDirection : public ScenarioObject
{
public:
    ScenarioPointingDirection();
    void load(const QDomElement& e);

    QString referenceSystem() const
    { return m_referenceSystem; }
    void setReferenceSystem(QString referenceSystem)
    { m_referenceSystem = referenceSystem; }
    double azimuth() const
    { return m_azimuth; }
    void setAzimuth(double azimuth)
    { m_azimuth = azimuth; }
    double elevation() const
    { return m_elevation; }
    void setElevation(double elevation)
    { m_elevation = elevation; }
    double azimuthDot() const
    { return m_azimuthDot; }
    void setAzimuthDot(double azimuthDot)
    { m_azimuthDot = azimuthDot; }
    double elevationDot() const
    { return m_elevationDot; }
    void setElevationDot(double elevationDot)
    { m_elevationDot = elevationDot; }

private:
    QString m_referenceSystem;
    double m_azimuth;
    double m_elevation;
    double m_azimuthDot;
    double m_elevationDot;
};


// ScenarioCommunicationAntenna
class ScenarioCommunicationAntenna : public ScenarioAntenna
{
public:
    ScenarioCommunicationAntenna();
    void load(const QDomElement& e);


private:
};


// ScenarioObservationAntenna
class ScenarioObservationAntenna : public ScenarioAntenna
{
public:
    ScenarioObservationAntenna();
    void load(const QDomElement& e);


private:
};


// ScenarioLocationType
class ScenarioLocationType : public ScenarioObject
{
public:
    ScenarioLocationType();
    void load(const QDomElement& e);

    QString CentralBody() const
    { return m_CentralBody; }
    void setCentralBody(QString CentralBody)
    { m_CentralBody = CentralBody; }
    QSharedPointer<ScenarioAbstract3DOFPositionType> Abstract3DOFPosition() const
    { return m_Abstract3DOFPosition; }
    void setAbstract3DOFPosition(QSharedPointer<ScenarioAbstract3DOFPositionType> Abstract3DOFPosition)
    { m_Abstract3DOFPosition = Abstract3DOFPosition; }

private:
    QString m_CentralBody;
    QSharedPointer<ScenarioAbstract3DOFPositionType> m_Abstract3DOFPosition;
};


// ScenarioGroundStation
class ScenarioGroundStation : public ScenarioParticipantsType
{
public:
    ScenarioGroundStation();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioLocationType> Location() const
    { return m_Location; }
    void setLocation(QSharedPointer<ScenarioLocationType> Location)
    { m_Location = Location; }
    QSharedPointer<ScenarioCommunicationAntenna> CommunicationAntenna() const
    { return m_CommunicationAntenna; }
    void setCommunicationAntenna(QSharedPointer<ScenarioCommunicationAntenna> CommunicationAntenna)
    { m_CommunicationAntenna = CommunicationAntenna; }

private:
    QSharedPointer<ScenarioLocationType> m_Location;
    QSharedPointer<ScenarioCommunicationAntenna> m_CommunicationAntenna;
};


// ScenarioLaunchPad
class ScenarioLaunchPad : public ScenarioParticipantsType
{
public:
    ScenarioLaunchPad();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioLocationType> Location() const
    { return m_Location; }
    void setLocation(QSharedPointer<ScenarioLocationType> Location)
    { m_Location = Location; }
    double clearingAltitude() const
    { return m_clearingAltitude; }
    void setClearingAltitude(double clearingAltitude)
    { m_clearingAltitude = clearingAltitude; }

private:
    QSharedPointer<ScenarioLocationType> m_Location;
    double m_clearingAltitude;
};


// ScenarioPoint
class ScenarioPoint : public ScenarioParticipantsType
{
public:
    ScenarioPoint();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioLocation> Location() const
    { return m_Location; }
    void setLocation(QSharedPointer<ScenarioLocation> Location)
    { m_Location = Location; }

private:
    QSharedPointer<ScenarioLocation> m_Location;
};


// ScenarioLocation
class ScenarioLocation : public ScenarioObject
{
public:
    ScenarioLocation();
    void load(const QDomElement& e);


private:
};


// ScenarioLV
class ScenarioLV : public ScenarioParticipantsType
{
public:
    ScenarioLV();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioLVProgram> LVProgram() const
    { return m_LVProgram; }
    void setLVProgram(QSharedPointer<ScenarioLVProgram> LVProgram)
    { m_LVProgram = LVProgram; }
    QSharedPointer<ScenarioLVMission> LVMission() const
    { return m_LVMission; }
    void setLVMission(QSharedPointer<ScenarioLVMission> LVMission)
    { m_LVMission = LVMission; }
    QSharedPointer<ScenarioLVSystemType> System() const
    { return m_System; }
    void setSystem(QSharedPointer<ScenarioLVSystemType> System)
    { m_System = System; }
    QSharedPointer<ScenarioOptimization> Optimization() const
    { return m_Optimization; }
    void setOptimization(QSharedPointer<ScenarioOptimization> Optimization)
    { m_Optimization = Optimization; }
    QSharedPointer<ScenarioOutputFiles> OutputFiles() const
    { return m_OutputFiles; }
    void setOutputFiles(QSharedPointer<ScenarioOutputFiles> OutputFiles)
    { m_OutputFiles = OutputFiles; }
    QString appearance() const
    { return m_appearance; }
    void setAppearance(QString appearance)
    { m_appearance = appearance; }

private:
    QSharedPointer<ScenarioLVProgram> m_LVProgram;
    QSharedPointer<ScenarioLVMission> m_LVMission;
    QSharedPointer<ScenarioLVSystemType> m_System;
    QSharedPointer<ScenarioOptimization> m_Optimization;
    QSharedPointer<ScenarioOutputFiles> m_OutputFiles;
    QString m_appearance;
};


// ScenarioPropulsionSystem
class ScenarioPropulsionSystem : public ScenarioObject
{
public:
    ScenarioPropulsionSystem();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioSystem> System() const
    { return m_System; }
    void setSystem(QSharedPointer<ScenarioSystem> System)
    { m_System = System; }
    QSharedPointer<ScenarioLiquidTanks> LiquidTanks() const
    { return m_LiquidTanks; }
    void setLiquidTanks(QSharedPointer<ScenarioLiquidTanks> LiquidTanks)
    { m_LiquidTanks = LiquidTanks; }
    QSharedPointer<ScenarioSolidGrain> SolidGrain() const
    { return m_SolidGrain; }
    void setSolidGrain(QSharedPointer<ScenarioSolidGrain> SolidGrain)
    { m_SolidGrain = SolidGrain; }
    QSharedPointer<ScenarioFeedSystem> FeedSystem() const
    { return m_FeedSystem; }
    void setFeedSystem(QSharedPointer<ScenarioFeedSystem> FeedSystem)
    { m_FeedSystem = FeedSystem; }
    QSharedPointer<ScenarioCombustionChamber> CombustionChamber() const
    { return m_CombustionChamber; }
    void setCombustionChamber(QSharedPointer<ScenarioCombustionChamber> CombustionChamber)
    { m_CombustionChamber = CombustionChamber; }
    QSharedPointer<ScenarioNozzle> Nozzle() const
    { return m_Nozzle; }
    void setNozzle(QSharedPointer<ScenarioNozzle> Nozzle)
    { m_Nozzle = Nozzle; }
    QSharedPointer<ScenarioPerformance> Performance() const
    { return m_Performance; }
    void setPerformance(QSharedPointer<ScenarioPerformance> Performance)
    { m_Performance = Performance; }

private:
    QSharedPointer<ScenarioSystem> m_System;
    QSharedPointer<ScenarioLiquidTanks> m_LiquidTanks;
    QSharedPointer<ScenarioSolidGrain> m_SolidGrain;
    QSharedPointer<ScenarioFeedSystem> m_FeedSystem;
    QSharedPointer<ScenarioCombustionChamber> m_CombustionChamber;
    QSharedPointer<ScenarioNozzle> m_Nozzle;
    QSharedPointer<ScenarioPerformance> m_Performance;
};


// ScenarioSystem
class ScenarioSystem : public ScenarioObject
{
public:
    ScenarioSystem();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioOptVarString> propType() const
    { return m_propType; }
    void setPropType(QSharedPointer<ScenarioOptVarString> propType)
    { m_propType = propType; }
    QString fuelType() const
    { return m_fuelType; }
    void setFuelType(QString fuelType)
    { m_fuelType = fuelType; }
    QString oxType() const
    { return m_oxType; }
    void setOxType(QString oxType)
    { m_oxType = oxType; }
    QSharedPointer<ScenarioOptVarInt> numberOfEngines() const
    { return m_numberOfEngines; }
    void setNumberOfEngines(QSharedPointer<ScenarioOptVarInt> numberOfEngines)
    { m_numberOfEngines = numberOfEngines; }
    QSharedPointer<ScenarioOptVarDouble> nominalThrust() const
    { return m_nominalThrust; }
    void setNominalThrust(QSharedPointer<ScenarioOptVarDouble> nominalThrust)
    { m_nominalThrust = nominalThrust; }
    double totalEngineDryMass() const
    { return m_totalEngineDryMass; }
    void setTotalEngineDryMass(double totalEngineDryMass)
    { m_totalEngineDryMass = totalEngineDryMass; }
    QSharedPointer<ScenarioOptVarDouble> propMass() const
    { return m_propMass; }
    void setPropMass(QSharedPointer<ScenarioOptVarDouble> propMass)
    { m_propMass = propMass; }
    double engineCoGLongPosition() const
    { return m_engineCoGLongPosition; }
    void setEngineCoGLongPosition(double engineCoGLongPosition)
    { m_engineCoGLongPosition = engineCoGLongPosition; }
    QSharedPointer<ScenarioOptVarDouble> lengthOverDiam() const
    { return m_lengthOverDiam; }
    void setLengthOverDiam(QSharedPointer<ScenarioOptVarDouble> lengthOverDiam)
    { m_lengthOverDiam = lengthOverDiam; }
    QSharedPointer<ScenarioOptVarBool> offTheShelf() const
    { return m_offTheShelf; }
    void setOffTheShelf(QSharedPointer<ScenarioOptVarBool> offTheShelf)
    { m_offTheShelf = offTheShelf; }
    QSharedPointer<ScenarioOptVarString> offTheShelfEngineType() const
    { return m_offTheShelfEngineType; }
    void setOffTheShelfEngineType(QSharedPointer<ScenarioOptVarString> offTheShelfEngineType)
    { m_offTheShelfEngineType = offTheShelfEngineType; }
    QSharedPointer<ScenarioOptVarString> feedType() const
    { return m_feedType; }
    void setFeedType(QSharedPointer<ScenarioOptVarString> feedType)
    { m_feedType = feedType; }
    QString coolingType() const
    { return m_coolingType; }
    void setCoolingType(QString coolingType)
    { m_coolingType = coolingType; }
    QSharedPointer<ScenarioOptVarDouble> mixtureRatio() const
    { return m_mixtureRatio; }
    void setMixtureRatio(QSharedPointer<ScenarioOptVarDouble> mixtureRatio)
    { m_mixtureRatio = mixtureRatio; }
    QSharedPointer<ScenarioOptVarDouble> nozzleOptAlt() const
    { return m_nozzleOptAlt; }
    void setNozzleOptAlt(QSharedPointer<ScenarioOptVarDouble> nozzleOptAlt)
    { m_nozzleOptAlt = nozzleOptAlt; }

private:
    QSharedPointer<ScenarioOptVarString> m_propType;
    QString m_fuelType;
    QString m_oxType;
    QSharedPointer<ScenarioOptVarInt> m_numberOfEngines;
    QSharedPointer<ScenarioOptVarDouble> m_nominalThrust;
    double m_totalEngineDryMass;
    QSharedPointer<ScenarioOptVarDouble> m_propMass;
    double m_engineCoGLongPosition;
    QSharedPointer<ScenarioOptVarDouble> m_lengthOverDiam;
    QSharedPointer<ScenarioOptVarBool> m_offTheShelf;
    QSharedPointer<ScenarioOptVarString> m_offTheShelfEngineType;
    QSharedPointer<ScenarioOptVarString> m_feedType;
    QString m_coolingType;
    QSharedPointer<ScenarioOptVarDouble> m_mixtureRatio;
    QSharedPointer<ScenarioOptVarDouble> m_nozzleOptAlt;
};


// ScenarioLiquidTanks
class ScenarioLiquidTanks : public ScenarioObject
{
public:
    ScenarioLiquidTanks();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioOptVarDouble> tanksPressureNorm() const
    { return m_tanksPressureNorm; }
    void setTanksPressureNorm(QSharedPointer<ScenarioOptVarDouble> tanksPressureNorm)
    { m_tanksPressureNorm = tanksPressureNorm; }
    double oxTankLength() const
    { return m_oxTankLength; }
    void setOxTankLength(double oxTankLength)
    { m_oxTankLength = oxTankLength; }
    double oxTankDiam() const
    { return m_oxTankDiam; }
    void setOxTankDiam(double oxTankDiam)
    { m_oxTankDiam = oxTankDiam; }
    double oxTankPressure() const
    { return m_oxTankPressure; }
    void setOxTankPressure(double oxTankPressure)
    { m_oxTankPressure = oxTankPressure; }
    double fuelTankLength() const
    { return m_fuelTankLength; }
    void setFuelTankLength(double fuelTankLength)
    { m_fuelTankLength = fuelTankLength; }
    double fuelTankDiam() const
    { return m_fuelTankDiam; }
    void setFuelTankDiam(double fuelTankDiam)
    { m_fuelTankDiam = fuelTankDiam; }
    double fuelTankPressure() const
    { return m_fuelTankPressure; }
    void setFuelTankPressure(double fuelTankPressure)
    { m_fuelTankPressure = fuelTankPressure; }

private:
    QSharedPointer<ScenarioOptVarDouble> m_tanksPressureNorm;
    double m_oxTankLength;
    double m_oxTankDiam;
    double m_oxTankPressure;
    double m_fuelTankLength;
    double m_fuelTankDiam;
    double m_fuelTankPressure;
};


// ScenarioSolidGrain
class ScenarioSolidGrain : public ScenarioObject
{
public:
    ScenarioSolidGrain();
    void load(const QDomElement& e);

    double grainLength() const
    { return m_grainLength; }
    void setGrainLength(double grainLength)
    { m_grainLength = grainLength; }
    double grainDiam() const
    { return m_grainDiam; }
    void setGrainDiam(double grainDiam)
    { m_grainDiam = grainDiam; }
    double igniterLength() const
    { return m_igniterLength; }
    void setIgniterLength(double igniterLength)
    { m_igniterLength = igniterLength; }
    double igniterDiam() const
    { return m_igniterDiam; }
    void setIgniterDiam(double igniterDiam)
    { m_igniterDiam = igniterDiam; }
    double igniterMass() const
    { return m_igniterMass; }
    void setIgniterMass(double igniterMass)
    { m_igniterMass = igniterMass; }
    double sliverMass() const
    { return m_sliverMass; }
    void setSliverMass(double sliverMass)
    { m_sliverMass = sliverMass; }
    double cavityVolume() const
    { return m_cavityVolume; }
    void setCavityVolume(double cavityVolume)
    { m_cavityVolume = cavityVolume; }

private:
    double m_grainLength;
    double m_grainDiam;
    double m_igniterLength;
    double m_igniterDiam;
    double m_igniterMass;
    double m_sliverMass;
    double m_cavityVolume;
};


// ScenarioFeedSystem
class ScenarioFeedSystem : public ScenarioObject
{
public:
    ScenarioFeedSystem();
    void load(const QDomElement& e);

    double pressurizerTankPressure() const
    { return m_pressurizerTankPressure; }
    void setPressurizerTankPressure(double pressurizerTankPressure)
    { m_pressurizerTankPressure = pressurizerTankPressure; }
    double pressurizerTankRadius() const
    { return m_pressurizerTankRadius; }
    void setPressurizerTankRadius(double pressurizerTankRadius)
    { m_pressurizerTankRadius = pressurizerTankRadius; }
    double pressurizerTankMass() const
    { return m_pressurizerTankMass; }
    void setPressurizerTankMass(double pressurizerTankMass)
    { m_pressurizerTankMass = pressurizerTankMass; }
    double pressurizerGasMass() const
    { return m_pressurizerGasMass; }
    void setPressurizerGasMass(double pressurizerGasMass)
    { m_pressurizerGasMass = pressurizerGasMass; }
    double linesValvesMass() const
    { return m_linesValvesMass; }
    void setLinesValvesMass(double linesValvesMass)
    { m_linesValvesMass = linesValvesMass; }
    double trappedPropMass() const
    { return m_trappedPropMass; }
    void setTrappedPropMass(double trappedPropMass)
    { m_trappedPropMass = trappedPropMass; }
    double totalFeedSystemMass() const
    { return m_totalFeedSystemMass; }
    void setTotalFeedSystemMass(double totalFeedSystemMass)
    { m_totalFeedSystemMass = totalFeedSystemMass; }
    double totalFeedSystemLength() const
    { return m_totalFeedSystemLength; }
    void setTotalFeedSystemLength(double totalFeedSystemLength)
    { m_totalFeedSystemLength = totalFeedSystemLength; }
    double totalFeedSystemDiameter() const
    { return m_totalFeedSystemDiameter; }
    void setTotalFeedSystemDiameter(double totalFeedSystemDiameter)
    { m_totalFeedSystemDiameter = totalFeedSystemDiameter; }

private:
    double m_pressurizerTankPressure;
    double m_pressurizerTankRadius;
    double m_pressurizerTankMass;
    double m_pressurizerGasMass;
    double m_linesValvesMass;
    double m_trappedPropMass;
    double m_totalFeedSystemMass;
    double m_totalFeedSystemLength;
    double m_totalFeedSystemDiameter;
};


// ScenarioCombustionChamber
class ScenarioCombustionChamber : public ScenarioObject
{
public:
    ScenarioCombustionChamber();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioOptVarDouble> abOverAt() const
    { return m_abOverAt; }
    void setAbOverAt(QSharedPointer<ScenarioOptVarDouble> abOverAt)
    { m_abOverAt = abOverAt; }
    QSharedPointer<ScenarioOptVarDouble> accOverAt() const
    { return m_accOverAt; }
    void setAccOverAt(QSharedPointer<ScenarioOptVarDouble> accOverAt)
    { m_accOverAt = accOverAt; }
    double ab() const
    { return m_ab; }
    void setAb(double ab)
    { m_ab = ab; }
    double acc() const
    { return m_acc; }
    void setAcc(double acc)
    { m_acc = acc; }
    QSharedPointer<ScenarioOptVarDouble> chamberPressureNorm() const
    { return m_chamberPressureNorm; }
    void setChamberPressureNorm(QSharedPointer<ScenarioOptVarDouble> chamberPressureNorm)
    { m_chamberPressureNorm = chamberPressureNorm; }
    double chamberPressure() const
    { return m_chamberPressure; }
    void setChamberPressure(double chamberPressure)
    { m_chamberPressure = chamberPressure; }
    double chamberCharactLength() const
    { return m_chamberCharactLength; }
    void setChamberCharactLength(double chamberCharactLength)
    { m_chamberCharactLength = chamberCharactLength; }
    double chamberLength() const
    { return m_chamberLength; }
    void setChamberLength(double chamberLength)
    { m_chamberLength = chamberLength; }
    double chamberDiameter() const
    { return m_chamberDiameter; }
    void setChamberDiameter(double chamberDiameter)
    { m_chamberDiameter = chamberDiameter; }
    double chamberMass() const
    { return m_chamberMass; }
    void setChamberMass(double chamberMass)
    { m_chamberMass = chamberMass; }

private:
    QSharedPointer<ScenarioOptVarDouble> m_abOverAt;
    QSharedPointer<ScenarioOptVarDouble> m_accOverAt;
    double m_ab;
    double m_acc;
    QSharedPointer<ScenarioOptVarDouble> m_chamberPressureNorm;
    double m_chamberPressure;
    double m_chamberCharactLength;
    double m_chamberLength;
    double m_chamberDiameter;
    double m_chamberMass;
};


// ScenarioNozzle
class ScenarioNozzle : public ScenarioObject
{
public:
    ScenarioNozzle();
    void load(const QDomElement& e);

    double at() const
    { return m_at; }
    void setAt(double at)
    { m_at = at; }
    double ae() const
    { return m_ae; }
    void setAe(double ae)
    { m_ae = ae; }
    double aeOverat() const
    { return m_aeOverat; }
    void setAeOverat(double aeOverat)
    { m_aeOverat = aeOverat; }
    double nozzleLength() const
    { return m_nozzleLength; }
    void setNozzleLength(double nozzleLength)
    { m_nozzleLength = nozzleLength; }
    double nozzleDiameter() const
    { return m_nozzleDiameter; }
    void setNozzleDiameter(double nozzleDiameter)
    { m_nozzleDiameter = nozzleDiameter; }
    double nozzleMass() const
    { return m_nozzleMass; }
    void setNozzleMass(double nozzleMass)
    { m_nozzleMass = nozzleMass; }
    QSharedPointer<ScenarioOptVarString> nozzleType() const
    { return m_nozzleType; }
    void setNozzleType(QSharedPointer<ScenarioOptVarString> nozzleType)
    { m_nozzleType = nozzleType; }
    QSharedPointer<ScenarioOptVarDouble> divAngle() const
    { return m_divAngle; }
    void setDivAngle(QSharedPointer<ScenarioOptVarDouble> divAngle)
    { m_divAngle = divAngle; }
    QSharedPointer<ScenarioOptVarString> tvcType() const
    { return m_tvcType; }
    void setTvcType(QSharedPointer<ScenarioOptVarString> tvcType)
    { m_tvcType = tvcType; }
    QSharedPointer<ScenarioOptVarDouble> tvcAngle() const
    { return m_tvcAngle; }
    void setTvcAngle(QSharedPointer<ScenarioOptVarDouble> tvcAngle)
    { m_tvcAngle = tvcAngle; }
    double tvcMass() const
    { return m_tvcMass; }
    void setTvcMass(double tvcMass)
    { m_tvcMass = tvcMass; }
    double nozzleCantAngle() const
    { return m_nozzleCantAngle; }
    void setNozzleCantAngle(double nozzleCantAngle)
    { m_nozzleCantAngle = nozzleCantAngle; }
    QString nozzleShape() const
    { return m_nozzleShape; }
    void setNozzleShape(QString nozzleShape)
    { m_nozzleShape = nozzleShape; }

private:
    double m_at;
    double m_ae;
    double m_aeOverat;
    double m_nozzleLength;
    double m_nozzleDiameter;
    double m_nozzleMass;
    QSharedPointer<ScenarioOptVarString> m_nozzleType;
    QSharedPointer<ScenarioOptVarDouble> m_divAngle;
    QSharedPointer<ScenarioOptVarString> m_tvcType;
    QSharedPointer<ScenarioOptVarDouble> m_tvcAngle;
    double m_tvcMass;
    double m_nozzleCantAngle;
    QString m_nozzleShape;
};


// ScenarioPerformance
class ScenarioPerformance : public ScenarioObject
{
public:
    ScenarioPerformance();
    void load(const QDomElement& e);

    double theoOptIsp() const
    { return m_theoOptIsp; }
    void setTheoOptIsp(double theoOptIsp)
    { m_theoOptIsp = theoOptIsp; }
    double realOptIsp() const
    { return m_realOptIsp; }
    void setRealOptIsp(double realOptIsp)
    { m_realOptIsp = realOptIsp; }
    double realSeeIsp() const
    { return m_realSeeIsp; }
    void setRealSeeIsp(double realSeeIsp)
    { m_realSeeIsp = realSeeIsp; }
    double realVacuumIsp() const
    { return m_realVacuumIsp; }
    void setRealVacuumIsp(double realVacuumIsp)
    { m_realVacuumIsp = realVacuumIsp; }
    double thrustCoeff() const
    { return m_thrustCoeff; }
    void setThrustCoeff(double thrustCoeff)
    { m_thrustCoeff = thrustCoeff; }
    double charactVelocity() const
    { return m_charactVelocity; }
    void setCharactVelocity(double charactVelocity)
    { m_charactVelocity = charactVelocity; }
    double thrustCoeffEff() const
    { return m_thrustCoeffEff; }
    void setThrustCoeffEff(double thrustCoeffEff)
    { m_thrustCoeffEff = thrustCoeffEff; }
    double charactVelocityEff() const
    { return m_charactVelocityEff; }
    void setCharactVelocityEff(double charactVelocityEff)
    { m_charactVelocityEff = charactVelocityEff; }
    double nominalMassFlow() const
    { return m_nominalMassFlow; }
    void setNominalMassFlow(double nominalMassFlow)
    { m_nominalMassFlow = nominalMassFlow; }
    double minOperativeAlt() const
    { return m_minOperativeAlt; }
    void setMinOperativeAlt(double minOperativeAlt)
    { m_minOperativeAlt = minOperativeAlt; }
    QSharedPointer<ScenarioOptVarBool> throttle() const
    { return m_throttle; }
    void setThrottle(QSharedPointer<ScenarioOptVarBool> throttle)
    { m_throttle = throttle; }
    QSharedPointer<ScenarioOptVarBool> restart() const
    { return m_restart; }
    void setRestart(QSharedPointer<ScenarioOptVarBool> restart)
    { m_restart = restart; }

private:
    double m_theoOptIsp;
    double m_realOptIsp;
    double m_realSeeIsp;
    double m_realVacuumIsp;
    double m_thrustCoeff;
    double m_charactVelocity;
    double m_thrustCoeffEff;
    double m_charactVelocityEff;
    double m_nominalMassFlow;
    double m_minOperativeAlt;
    QSharedPointer<ScenarioOptVarBool> m_throttle;
    QSharedPointer<ScenarioOptVarBool> m_restart;
};


// ScenarioLVProgram
class ScenarioLVProgram : public ScenarioObject
{
public:
    ScenarioLVProgram();
    void load(const QDomElement& e);

    int nLaunches() const
    { return m_nLaunches; }
    void setNLaunches(int nLaunches)
    { m_nLaunches = nLaunches; }
    double nYearsOps() const
    { return m_nYearsOps; }
    void setNYearsOps(double nYearsOps)
    { m_nYearsOps = nYearsOps; }
    int nLaunchesPerYear() const
    { return m_nLaunchesPerYear; }
    void setNLaunchesPerYear(int nLaunchesPerYear)
    { m_nLaunchesPerYear = nLaunchesPerYear; }
    QSharedPointer<ScenarioprogramCostFactors> programCostFactors() const
    { return m_programCostFactors; }
    void setProgramCostFactors(QSharedPointer<ScenarioprogramCostFactors> programCostFactors)
    { m_programCostFactors = programCostFactors; }

private:
    int m_nLaunches;
    double m_nYearsOps;
    int m_nLaunchesPerYear;
    QSharedPointer<ScenarioprogramCostFactors> m_programCostFactors;
};


// ScenarioprogramCostFactors
class ScenarioprogramCostFactors : public ScenarioObject
{
public:
    ScenarioprogramCostFactors();
    void load(const QDomElement& e);

    double f0() const
    { return m_f0; }
    void setF0(double f0)
    { m_f0 = f0; }
    QString launcherDesignComplexity() const
    { return m_launcherDesignComplexity; }
    void setLauncherDesignComplexity(QString launcherDesignComplexity)
    { m_launcherDesignComplexity = launcherDesignComplexity; }
    double f1() const
    { return m_f1; }
    void setF1(double f1)
    { m_f1 = f1; }
    QString teamExpertise() const
    { return m_teamExpertise; }
    void setTeamExpertise(QString teamExpertise)
    { m_teamExpertise = teamExpertise; }
    double f3() const
    { return m_f3; }
    void setF3(double f3)
    { m_f3 = f3; }
    QString contractorsStructure() const
    { return m_contractorsStructure; }
    void setContractorsStructure(QString contractorsStructure)
    { m_contractorsStructure = contractorsStructure; }
    double f7() const
    { return m_f7; }
    void setF7(double f7)
    { m_f7 = f7; }

private:
    double m_f0;
    QString m_launcherDesignComplexity;
    double m_f1;
    QString m_teamExpertise;
    double m_f3;
    QString m_contractorsStructure;
    double m_f7;
};


// ScenarioLVMission
class ScenarioLVMission : public ScenarioObject
{
public:
    ScenarioLVMission();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioLVPayload> LVPayload() const
    { return m_LVPayload; }
    void setLVPayload(QSharedPointer<ScenarioLVPayload> LVPayload)
    { m_LVPayload = LVPayload; }
    QSharedPointer<ScenarioEnvironmentType> Environment() const
    { return m_Environment; }
    void setEnvironment(QSharedPointer<ScenarioEnvironmentType> Environment)
    { m_Environment = Environment; }
    QSharedPointer<ScenarioLaunchSite> LaunchSite() const
    { return m_LaunchSite; }
    void setLaunchSite(QSharedPointer<ScenarioLaunchSite> LaunchSite)
    { m_LaunchSite = LaunchSite; }
    QSharedPointer<ScenarioTargetOrbit> TargetOrbit() const
    { return m_TargetOrbit; }
    void setTargetOrbit(QSharedPointer<ScenarioTargetOrbit> TargetOrbit)
    { m_TargetOrbit = TargetOrbit; }
    QSharedPointer<ScenarioTrajectory> Trajectory() const
    { return m_Trajectory; }
    void setTrajectory(QSharedPointer<ScenarioTrajectory> Trajectory)
    { m_Trajectory = Trajectory; }

private:
    QSharedPointer<ScenarioLVPayload> m_LVPayload;
    QSharedPointer<ScenarioEnvironmentType> m_Environment;
    QSharedPointer<ScenarioLaunchSite> m_LaunchSite;
    QSharedPointer<ScenarioTargetOrbit> m_TargetOrbit;
    QSharedPointer<ScenarioTrajectory> m_Trajectory;
};


// ScenarioLVPayload
class ScenarioLVPayload : public ScenarioObject
{
public:
    ScenarioLVPayload();
    void load(const QDomElement& e);

    double mass() const
    { return m_mass; }
    void setMass(double mass)
    { m_mass = mass; }
    double length() const
    { return m_length; }
    void setLength(double length)
    { m_length = length; }
    double diameter() const
    { return m_diameter; }
    void setDiameter(double diameter)
    { m_diameter = diameter; }
    double CoGLongPosition() const
    { return m_CoGLongPosition; }
    void setCoGLongPosition(double CoGLongPosition)
    { m_CoGLongPosition = CoGLongPosition; }
    double maxAxialAcc() const
    { return m_maxAxialAcc; }
    void setMaxAxialAcc(double maxAxialAcc)
    { m_maxAxialAcc = maxAxialAcc; }
    double maxHeatFlux() const
    { return m_maxHeatFlux; }
    void setMaxHeatFlux(double maxHeatFlux)
    { m_maxHeatFlux = maxHeatFlux; }

private:
    double m_mass;
    double m_length;
    double m_diameter;
    double m_CoGLongPosition;
    double m_maxAxialAcc;
    double m_maxHeatFlux;
};


// ScenarioLaunchSite
class ScenarioLaunchSite : public ScenarioObject
{
public:
    ScenarioLaunchSite();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioAbstract3DOFPositionType> Abstract3DOFPosition() const
    { return m_Abstract3DOFPosition; }
    void setAbstract3DOFPosition(QSharedPointer<ScenarioAbstract3DOFPositionType> Abstract3DOFPosition)
    { m_Abstract3DOFPosition = Abstract3DOFPosition; }
    const QList<double>& allowedAzimuths() const
    { return m_allowedAzimuths; }
    QList<double>& allowedAzimuths()
    { return m_allowedAzimuths; }
    void setAllowedAzimuths(QList<double> allowedAzimuths)
    { m_allowedAzimuths = allowedAzimuths; }

private:
    QSharedPointer<ScenarioAbstract3DOFPositionType> m_Abstract3DOFPosition;
    QList<double> m_allowedAzimuths;
};


// ScenarioTargetOrbit
class ScenarioTargetOrbit : public ScenarioObject
{
public:
    ScenarioTargetOrbit();
    void load(const QDomElement& e);

    double semiaxis() const
    { return m_semiaxis; }
    void setSemiaxis(double semiaxis)
    { m_semiaxis = semiaxis; }
    double eccentricity() const
    { return m_eccentricity; }
    void setEccentricity(double eccentricity)
    { m_eccentricity = eccentricity; }
    double inclination() const
    { return m_inclination; }
    void setInclination(double inclination)
    { m_inclination = inclination; }
    double semiaxisTol() const
    { return m_semiaxisTol; }
    void setSemiaxisTol(double semiaxisTol)
    { m_semiaxisTol = semiaxisTol; }
    double eccentricityTol() const
    { return m_eccentricityTol; }
    void setEccentricityTol(double eccentricityTol)
    { m_eccentricityTol = eccentricityTol; }
    double inclinationTol() const
    { return m_inclinationTol; }
    void setInclinationTol(double inclinationTol)
    { m_inclinationTol = inclinationTol; }

private:
    double m_semiaxis;
    double m_eccentricity;
    double m_inclination;
    double m_semiaxisTol;
    double m_eccentricityTol;
    double m_inclinationTol;
};


// ScenarioLVSystemType
class ScenarioLVSystemType : public ScenarioObject
{
public:
    ScenarioLVSystemType();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioArchitecture> Architecture() const
    { return m_Architecture; }
    void setArchitecture(QSharedPointer<ScenarioArchitecture> Architecture)
    { m_Architecture = Architecture; }
    const QList<QSharedPointer<ScenarioLowerStage> >& LowerStage() const
    { return m_LowerStage; }
    QList<QSharedPointer<ScenarioLowerStage> >& LowerStage()
    { return m_LowerStage; }
    void setLowerStage(QList<QSharedPointer<ScenarioLowerStage> > LowerStage)
    { m_LowerStage = LowerStage; }
    QSharedPointer<ScenarioUpperStage> UpperStage() const
    { return m_UpperStage; }
    void setUpperStage(QSharedPointer<ScenarioUpperStage> UpperStage)
    { m_UpperStage = UpperStage; }
    const QList<QSharedPointer<ScenarioBoosters> >& Boosters() const
    { return m_Boosters; }
    QList<QSharedPointer<ScenarioBoosters> >& Boosters()
    { return m_Boosters; }
    void setBoosters(QList<QSharedPointer<ScenarioBoosters> > Boosters)
    { m_Boosters = Boosters; }
    QSharedPointer<ScenarioSystemWeights> SystemWeights() const
    { return m_SystemWeights; }
    void setSystemWeights(QSharedPointer<ScenarioSystemWeights> SystemWeights)
    { m_SystemWeights = SystemWeights; }
    QSharedPointer<ScenarioSystemCosts> SystemCosts() const
    { return m_SystemCosts; }
    void setSystemCosts(QSharedPointer<ScenarioSystemCosts> SystemCosts)
    { m_SystemCosts = SystemCosts; }
    QSharedPointer<ScenarioSystemReliability> SystemReliability() const
    { return m_SystemReliability; }
    void setSystemReliability(QSharedPointer<ScenarioSystemReliability> SystemReliability)
    { m_SystemReliability = SystemReliability; }
    QSharedPointer<ScenarioLVAerodynamics> LVAerodynamics() const
    { return m_LVAerodynamics; }
    void setLVAerodynamics(QSharedPointer<ScenarioLVAerodynamics> LVAerodynamics)
    { m_LVAerodynamics = LVAerodynamics; }
    QSharedPointer<ScenarioDiscretizationSettings> DiscretizationSettings() const
    { return m_DiscretizationSettings; }
    void setDiscretizationSettings(QSharedPointer<ScenarioDiscretizationSettings> DiscretizationSettings)
    { m_DiscretizationSettings = DiscretizationSettings; }

private:
    QSharedPointer<ScenarioArchitecture> m_Architecture;
    QList<QSharedPointer<ScenarioLowerStage> > m_LowerStage;
    QSharedPointer<ScenarioUpperStage> m_UpperStage;
    QList<QSharedPointer<ScenarioBoosters> > m_Boosters;
    QSharedPointer<ScenarioSystemWeights> m_SystemWeights;
    QSharedPointer<ScenarioSystemCosts> m_SystemCosts;
    QSharedPointer<ScenarioSystemReliability> m_SystemReliability;
    QSharedPointer<ScenarioLVAerodynamics> m_LVAerodynamics;
    QSharedPointer<ScenarioDiscretizationSettings> m_DiscretizationSettings;
};


// ScenarioArchitecture
class ScenarioArchitecture : public ScenarioObject
{
public:
    ScenarioArchitecture();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioOptVarString> boosterConf() const
    { return m_boosterConf; }
    void setBoosterConf(QSharedPointer<ScenarioOptVarString> boosterConf)
    { m_boosterConf = boosterConf; }
    QSharedPointer<ScenarioOptVarInt> nStages() const
    { return m_nStages; }
    void setNStages(QSharedPointer<ScenarioOptVarInt> nStages)
    { m_nStages = nStages; }
    QSharedPointer<ScenarioOptVarInt> nBoosters() const
    { return m_nBoosters; }
    void setNBoosters(QSharedPointer<ScenarioOptVarInt> nBoosters)
    { m_nBoosters = nBoosters; }
    QSharedPointer<ScenarioOptVarBool> singleEngineType() const
    { return m_singleEngineType; }
    void setSingleEngineType(QSharedPointer<ScenarioOptVarBool> singleEngineType)
    { m_singleEngineType = singleEngineType; }

private:
    QSharedPointer<ScenarioOptVarString> m_boosterConf;
    QSharedPointer<ScenarioOptVarInt> m_nStages;
    QSharedPointer<ScenarioOptVarInt> m_nBoosters;
    QSharedPointer<ScenarioOptVarBool> m_singleEngineType;
};


// ScenarioLowerStage
class ScenarioLowerStage : public ScenarioObject
{
public:
    ScenarioLowerStage();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioGeometry> Geometry() const
    { return m_Geometry; }
    void setGeometry(QSharedPointer<ScenarioGeometry> Geometry)
    { m_Geometry = Geometry; }
    QSharedPointer<ScenarioPropulsionSystem> PropulsionSystem() const
    { return m_PropulsionSystem; }
    void setPropulsionSystem(QSharedPointer<ScenarioPropulsionSystem> PropulsionSystem)
    { m_PropulsionSystem = PropulsionSystem; }
    QSharedPointer<ScenarioComponentWeights> ComponentWeights() const
    { return m_ComponentWeights; }
    void setComponentWeights(QSharedPointer<ScenarioComponentWeights> ComponentWeights)
    { m_ComponentWeights = ComponentWeights; }
    QSharedPointer<ScenarioComponentCosts> ComponentCosts() const
    { return m_ComponentCosts; }
    void setComponentCosts(QSharedPointer<ScenarioComponentCosts> ComponentCosts)
    { m_ComponentCosts = ComponentCosts; }
    QSharedPointer<ScenarioComponentReliability> ComponentReliability() const
    { return m_ComponentReliability; }
    void setComponentReliability(QSharedPointer<ScenarioComponentReliability> ComponentReliability)
    { m_ComponentReliability = ComponentReliability; }

private:
    QSharedPointer<ScenarioGeometry> m_Geometry;
    QSharedPointer<ScenarioPropulsionSystem> m_PropulsionSystem;
    QSharedPointer<ScenarioComponentWeights> m_ComponentWeights;
    QSharedPointer<ScenarioComponentCosts> m_ComponentCosts;
    QSharedPointer<ScenarioComponentReliability> m_ComponentReliability;
};


// ScenarioUpperStage
class ScenarioUpperStage : public ScenarioObject
{
public:
    ScenarioUpperStage();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioGeometry> Geometry() const
    { return m_Geometry; }
    void setGeometry(QSharedPointer<ScenarioGeometry> Geometry)
    { m_Geometry = Geometry; }
    QSharedPointer<ScenarioPropulsionSystem> PropulsionSystem() const
    { return m_PropulsionSystem; }
    void setPropulsionSystem(QSharedPointer<ScenarioPropulsionSystem> PropulsionSystem)
    { m_PropulsionSystem = PropulsionSystem; }
    QSharedPointer<ScenarioComponentWeights> ComponentWeights() const
    { return m_ComponentWeights; }
    void setComponentWeights(QSharedPointer<ScenarioComponentWeights> ComponentWeights)
    { m_ComponentWeights = ComponentWeights; }
    QSharedPointer<ScenarioComponentCosts> ComponentCosts() const
    { return m_ComponentCosts; }
    void setComponentCosts(QSharedPointer<ScenarioComponentCosts> ComponentCosts)
    { m_ComponentCosts = ComponentCosts; }
    QSharedPointer<ScenarioComponentReliability> ComponentReliability() const
    { return m_ComponentReliability; }
    void setComponentReliability(QSharedPointer<ScenarioComponentReliability> ComponentReliability)
    { m_ComponentReliability = ComponentReliability; }

private:
    QSharedPointer<ScenarioGeometry> m_Geometry;
    QSharedPointer<ScenarioPropulsionSystem> m_PropulsionSystem;
    QSharedPointer<ScenarioComponentWeights> m_ComponentWeights;
    QSharedPointer<ScenarioComponentCosts> m_ComponentCosts;
    QSharedPointer<ScenarioComponentReliability> m_ComponentReliability;
};


// ScenarioBoosters
class ScenarioBoosters : public ScenarioObject
{
public:
    ScenarioBoosters();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioGeometry> Geometry() const
    { return m_Geometry; }
    void setGeometry(QSharedPointer<ScenarioGeometry> Geometry)
    { m_Geometry = Geometry; }
    QSharedPointer<ScenarioPropulsionSystem> PropulsionSystem() const
    { return m_PropulsionSystem; }
    void setPropulsionSystem(QSharedPointer<ScenarioPropulsionSystem> PropulsionSystem)
    { m_PropulsionSystem = PropulsionSystem; }
    QSharedPointer<ScenarioComponentWeights> ComponentWeights() const
    { return m_ComponentWeights; }
    void setComponentWeights(QSharedPointer<ScenarioComponentWeights> ComponentWeights)
    { m_ComponentWeights = ComponentWeights; }
    QSharedPointer<ScenarioComponentCosts> ComponentCosts() const
    { return m_ComponentCosts; }
    void setComponentCosts(QSharedPointer<ScenarioComponentCosts> ComponentCosts)
    { m_ComponentCosts = ComponentCosts; }
    QSharedPointer<ScenarioComponentReliability> ComponentReliability() const
    { return m_ComponentReliability; }
    void setComponentReliability(QSharedPointer<ScenarioComponentReliability> ComponentReliability)
    { m_ComponentReliability = ComponentReliability; }

private:
    QSharedPointer<ScenarioGeometry> m_Geometry;
    QSharedPointer<ScenarioPropulsionSystem> m_PropulsionSystem;
    QSharedPointer<ScenarioComponentWeights> m_ComponentWeights;
    QSharedPointer<ScenarioComponentCosts> m_ComponentCosts;
    QSharedPointer<ScenarioComponentReliability> m_ComponentReliability;
};


// ScenarioDiscretizationSettings
class ScenarioDiscretizationSettings : public ScenarioObject
{
public:
    ScenarioDiscretizationSettings();
    void load(const QDomElement& e);

    int nMachPointsCl() const
    { return m_nMachPointsCl; }
    void setNMachPointsCl(int nMachPointsCl)
    { m_nMachPointsCl = nMachPointsCl; }
    int nAlfaPointsCl() const
    { return m_nAlfaPointsCl; }
    void setNAlfaPointsCl(int nAlfaPointsCl)
    { m_nAlfaPointsCl = nAlfaPointsCl; }
    int nMachPointsCd() const
    { return m_nMachPointsCd; }
    void setNMachPointsCd(int nMachPointsCd)
    { m_nMachPointsCd = nMachPointsCd; }
    int nAlfaPointsCd() const
    { return m_nAlfaPointsCd; }
    void setNAlfaPointsCd(int nAlfaPointsCd)
    { m_nAlfaPointsCd = nAlfaPointsCd; }
    int nMachPointsCm() const
    { return m_nMachPointsCm; }
    void setNMachPointsCm(int nMachPointsCm)
    { m_nMachPointsCm = nMachPointsCm; }
    int nAlfaPointsCm() const
    { return m_nAlfaPointsCm; }
    void setNAlfaPointsCm(int nAlfaPointsCm)
    { m_nAlfaPointsCm = nAlfaPointsCm; }
    int nPitchControlNodes() const
    { return m_nPitchControlNodes; }
    void setNPitchControlNodes(int nPitchControlNodes)
    { m_nPitchControlNodes = nPitchControlNodes; }
    int nYawControlNodes() const
    { return m_nYawControlNodes; }
    void setNYawControlNodes(int nYawControlNodes)
    { m_nYawControlNodes = nYawControlNodes; }
    int nThrustControlNodes() const
    { return m_nThrustControlNodes; }
    void setNThrustControlNodes(int nThrustControlNodes)
    { m_nThrustControlNodes = nThrustControlNodes; }

private:
    int m_nMachPointsCl;
    int m_nAlfaPointsCl;
    int m_nMachPointsCd;
    int m_nAlfaPointsCd;
    int m_nMachPointsCm;
    int m_nAlfaPointsCm;
    int m_nPitchControlNodes;
    int m_nYawControlNodes;
    int m_nThrustControlNodes;
};


// ScenarioGeometry
class ScenarioGeometry : public ScenarioObject
{
public:
    ScenarioGeometry();
    void load(const QDomElement& e);

    double length() const
    { return m_length; }
    void setLength(double length)
    { m_length = length; }
    double diameter() const
    { return m_diameter; }
    void setDiameter(double diameter)
    { m_diameter = diameter; }
    QSharedPointer<ScenarioOptVarBool> diameterEqualToUpper() const
    { return m_diameterEqualToUpper; }
    void setDiameterEqualToUpper(QSharedPointer<ScenarioOptVarBool> diameterEqualToUpper)
    { m_diameterEqualToUpper = diameterEqualToUpper; }
    QString frontShape() const
    { return m_frontShape; }
    void setFrontShape(QString frontShape)
    { m_frontShape = frontShape; }
    double baseLongPosition() const
    { return m_baseLongPosition; }
    void setBaseLongPosition(double baseLongPosition)
    { m_baseLongPosition = baseLongPosition; }
    double constraintEngine() const
    { return m_constraintEngine; }
    void setConstraintEngine(double constraintEngine)
    { m_constraintEngine = constraintEngine; }
    double constraintInterference() const
    { return m_constraintInterference; }
    void setConstraintInterference(double constraintInterference)
    { m_constraintInterference = constraintInterference; }

private:
    double m_length;
    double m_diameter;
    QSharedPointer<ScenarioOptVarBool> m_diameterEqualToUpper;
    QString m_frontShape;
    double m_baseLongPosition;
    double m_constraintEngine;
    double m_constraintInterference;
};


// ScenarioLVAerodynamics
class ScenarioLVAerodynamics : public ScenarioObject
{
public:
    ScenarioLVAerodynamics();
    void load(const QDomElement& e);

    bool userDefinedAero() const
    { return m_userDefinedAero; }
    void setUserDefinedAero(bool userDefinedAero)
    { m_userDefinedAero = userDefinedAero; }
    double referenceArea() const
    { return m_referenceArea; }
    void setReferenceArea(double referenceArea)
    { m_referenceArea = referenceArea; }
    double referenceLength() const
    { return m_referenceLength; }
    void setReferenceLength(double referenceLength)
    { m_referenceLength = referenceLength; }
    QString clFileName() const
    { return m_clFileName; }
    void setClFileName(QString clFileName)
    { m_clFileName = clFileName; }
    QString cdFileName() const
    { return m_cdFileName; }
    void setCdFileName(QString cdFileName)
    { m_cdFileName = cdFileName; }
    QString cmFileName() const
    { return m_cmFileName; }
    void setCmFileName(QString cmFileName)
    { m_cmFileName = cmFileName; }

private:
    bool m_userDefinedAero;
    double m_referenceArea;
    double m_referenceLength;
    QString m_clFileName;
    QString m_cdFileName;
    QString m_cmFileName;
};


// ScenarioComponentWeights
class ScenarioComponentWeights : public ScenarioObject
{
public:
    ScenarioComponentWeights();
    void load(const QDomElement& e);

    double dryMass() const
    { return m_dryMass; }
    void setDryMass(double dryMass)
    { m_dryMass = dryMass; }
    double dryCoGLongPosition() const
    { return m_dryCoGLongPosition; }
    void setDryCoGLongPosition(double dryCoGLongPosition)
    { m_dryCoGLongPosition = dryCoGLongPosition; }
    double wetMass() const
    { return m_wetMass; }
    void setWetMass(double wetMass)
    { m_wetMass = wetMass; }
    double mainStructMass() const
    { return m_mainStructMass; }
    void setMainStructMass(double mainStructMass)
    { m_mainStructMass = mainStructMass; }
    double oxTankMass() const
    { return m_oxTankMass; }
    void setOxTankMass(double oxTankMass)
    { m_oxTankMass = oxTankMass; }
    double fuelTankMass() const
    { return m_fuelTankMass; }
    void setFuelTankMass(double fuelTankMass)
    { m_fuelTankMass = fuelTankMass; }
    double tpsMass() const
    { return m_tpsMass; }
    void setTpsMass(double tpsMass)
    { m_tpsMass = tpsMass; }
    double avionicsMass() const
    { return m_avionicsMass; }
    void setAvionicsMass(double avionicsMass)
    { m_avionicsMass = avionicsMass; }
    double epsMass() const
    { return m_epsMass; }
    void setEpsMass(double epsMass)
    { m_epsMass = epsMass; }
    double plAdapterMass() const
    { return m_plAdapterMass; }
    void setPlAdapterMass(double plAdapterMass)
    { m_plAdapterMass = plAdapterMass; }
    double padInterfaceMass() const
    { return m_padInterfaceMass; }
    void setPadInterfaceMass(double padInterfaceMass)
    { m_padInterfaceMass = padInterfaceMass; }
    double interstageMass() const
    { return m_interstageMass; }
    void setInterstageMass(double interstageMass)
    { m_interstageMass = interstageMass; }

private:
    double m_dryMass;
    double m_dryCoGLongPosition;
    double m_wetMass;
    double m_mainStructMass;
    double m_oxTankMass;
    double m_fuelTankMass;
    double m_tpsMass;
    double m_avionicsMass;
    double m_epsMass;
    double m_plAdapterMass;
    double m_padInterfaceMass;
    double m_interstageMass;
};


// ScenarioSystemWeights
class ScenarioSystemWeights : public ScenarioObject
{
public:
    ScenarioSystemWeights();
    void load(const QDomElement& e);

    double totalDryMass() const
    { return m_totalDryMass; }
    void setTotalDryMass(double totalDryMass)
    { m_totalDryMass = totalDryMass; }
    double totalWetMass() const
    { return m_totalWetMass; }
    void setTotalWetMass(double totalWetMass)
    { m_totalWetMass = totalWetMass; }
    double takeoffThrustOverWeight() const
    { return m_takeoffThrustOverWeight; }
    void setTakeoffThrustOverWeight(double takeoffThrustOverWeight)
    { m_takeoffThrustOverWeight = takeoffThrustOverWeight; }
    QSharedPointer<ScenarioOptVarDouble> maxAxialAcc() const
    { return m_maxAxialAcc; }
    void setMaxAxialAcc(QSharedPointer<ScenarioOptVarDouble> maxAxialAcc)
    { m_maxAxialAcc = maxAxialAcc; }
    QSharedPointer<ScenarioOptVarDouble> maxHeatFlux() const
    { return m_maxHeatFlux; }
    void setMaxHeatFlux(QSharedPointer<ScenarioOptVarDouble> maxHeatFlux)
    { m_maxHeatFlux = maxHeatFlux; }
    QSharedPointer<ScenarioOptVarDouble> maxDynPressure() const
    { return m_maxDynPressure; }
    void setMaxDynPressure(QSharedPointer<ScenarioOptVarDouble> maxDynPressure)
    { m_maxDynPressure = maxDynPressure; }
    QSharedPointer<ScenarioOptVarString> mainStructuralMaterial() const
    { return m_mainStructuralMaterial; }
    void setMainStructuralMaterial(QSharedPointer<ScenarioOptVarString> mainStructuralMaterial)
    { m_mainStructuralMaterial = mainStructuralMaterial; }
    QSharedPointer<ScenarioOptVarString> tanksArrangement() const
    { return m_tanksArrangement; }
    void setTanksArrangement(QSharedPointer<ScenarioOptVarString> tanksArrangement)
    { m_tanksArrangement = tanksArrangement; }
    QSharedPointer<ScenarioOptVarString> tanksType() const
    { return m_tanksType; }
    void setTanksType(QSharedPointer<ScenarioOptVarString> tanksType)
    { m_tanksType = tanksType; }
    QSharedPointer<ScenarioOptVarString> redundancyLevel() const
    { return m_redundancyLevel; }
    void setRedundancyLevel(QSharedPointer<ScenarioOptVarString> redundancyLevel)
    { m_redundancyLevel = redundancyLevel; }
    QSharedPointer<ScenarioOptVarDouble> structuralSafetyMargin() const
    { return m_structuralSafetyMargin; }
    void setStructuralSafetyMargin(QSharedPointer<ScenarioOptVarDouble> structuralSafetyMargin)
    { m_structuralSafetyMargin = structuralSafetyMargin; }

private:
    double m_totalDryMass;
    double m_totalWetMass;
    double m_takeoffThrustOverWeight;
    QSharedPointer<ScenarioOptVarDouble> m_maxAxialAcc;
    QSharedPointer<ScenarioOptVarDouble> m_maxHeatFlux;
    QSharedPointer<ScenarioOptVarDouble> m_maxDynPressure;
    QSharedPointer<ScenarioOptVarString> m_mainStructuralMaterial;
    QSharedPointer<ScenarioOptVarString> m_tanksArrangement;
    QSharedPointer<ScenarioOptVarString> m_tanksType;
    QSharedPointer<ScenarioOptVarString> m_redundancyLevel;
    QSharedPointer<ScenarioOptVarDouble> m_structuralSafetyMargin;
};


// ScenarioComponentCosts
class ScenarioComponentCosts : public ScenarioObject
{
public:
    ScenarioComponentCosts();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioOptVarInt> nEngineTests() const
    { return m_nEngineTests; }
    void setNEngineTests(QSharedPointer<ScenarioOptVarInt> nEngineTests)
    { m_nEngineTests = nEngineTests; }
    QSharedPointer<ScenarioDevelopmentCosts> DevelopmentCosts() const
    { return m_DevelopmentCosts; }
    void setDevelopmentCosts(QSharedPointer<ScenarioDevelopmentCosts> DevelopmentCosts)
    { m_DevelopmentCosts = DevelopmentCosts; }
    QSharedPointer<ScenarioProductionCosts> ProductionCosts() const
    { return m_ProductionCosts; }
    void setProductionCosts(QSharedPointer<ScenarioProductionCosts> ProductionCosts)
    { m_ProductionCosts = ProductionCosts; }

private:
    QSharedPointer<ScenarioOptVarInt> m_nEngineTests;
    QSharedPointer<ScenarioDevelopmentCosts> m_DevelopmentCosts;
    QSharedPointer<ScenarioProductionCosts> m_ProductionCosts;
};


// ScenarioDevelopmentCosts
class ScenarioDevelopmentCosts : public ScenarioObject
{
public:
    ScenarioDevelopmentCosts();
    void load(const QDomElement& e);

    double enginef2() const
    { return m_enginef2; }
    void setEnginef2(double enginef2)
    { m_enginef2 = enginef2; }
    double enginef5() const
    { return m_enginef5; }
    void setEnginef5(double enginef5)
    { m_enginef5 = enginef5; }
    double engineDevelopmentCost() const
    { return m_engineDevelopmentCost; }
    void setEngineDevelopmentCost(double engineDevelopmentCost)
    { m_engineDevelopmentCost = engineDevelopmentCost; }
    double systemf2() const
    { return m_systemf2; }
    void setSystemf2(double systemf2)
    { m_systemf2 = systemf2; }
    double systemDevelopmentCost() const
    { return m_systemDevelopmentCost; }
    void setSystemDevelopmentCost(double systemDevelopmentCost)
    { m_systemDevelopmentCost = systemDevelopmentCost; }

private:
    double m_enginef2;
    double m_enginef5;
    double m_engineDevelopmentCost;
    double m_systemf2;
    double m_systemDevelopmentCost;
};


// ScenarioProductionCosts
class ScenarioProductionCosts : public ScenarioObject
{
public:
    ScenarioProductionCosts();
    void load(const QDomElement& e);

    int totalNumberOfEngines() const
    { return m_totalNumberOfEngines; }
    void setTotalNumberOfEngines(int totalNumberOfEngines)
    { m_totalNumberOfEngines = totalNumberOfEngines; }
    double enginesLearningFactor() const
    { return m_enginesLearningFactor; }
    void setEnginesLearningFactor(double enginesLearningFactor)
    { m_enginesLearningFactor = enginesLearningFactor; }
    int totalNumberOfSystems() const
    { return m_totalNumberOfSystems; }
    void setTotalNumberOfSystems(int totalNumberOfSystems)
    { m_totalNumberOfSystems = totalNumberOfSystems; }
    double systemsLearningFactor() const
    { return m_systemsLearningFactor; }
    void setSystemsLearningFactor(double systemsLearningFactor)
    { m_systemsLearningFactor = systemsLearningFactor; }
    double totalEngineProductionCost() const
    { return m_totalEngineProductionCost; }
    void setTotalEngineProductionCost(double totalEngineProductionCost)
    { m_totalEngineProductionCost = totalEngineProductionCost; }
    double totalSystemProductionCost() const
    { return m_totalSystemProductionCost; }
    void setTotalSystemProductionCost(double totalSystemProductionCost)
    { m_totalSystemProductionCost = totalSystemProductionCost; }

private:
    int m_totalNumberOfEngines;
    double m_enginesLearningFactor;
    int m_totalNumberOfSystems;
    double m_systemsLearningFactor;
    double m_totalEngineProductionCost;
    double m_totalSystemProductionCost;
};


// ScenarioSystemCosts
class ScenarioSystemCosts : public ScenarioObject
{
public:
    ScenarioSystemCosts();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioTotalProgramCosts> TotalProgramCosts() const
    { return m_TotalProgramCosts; }
    void setTotalProgramCosts(QSharedPointer<ScenarioTotalProgramCosts> TotalProgramCosts)
    { m_TotalProgramCosts = TotalProgramCosts; }
    double developmentTime() const
    { return m_developmentTime; }
    void setDevelopmentTime(double developmentTime)
    { m_developmentTime = developmentTime; }
    QSharedPointer<ScenarioDirectOperationsCosts> DirectOperationsCosts() const
    { return m_DirectOperationsCosts; }
    void setDirectOperationsCosts(QSharedPointer<ScenarioDirectOperationsCosts> DirectOperationsCosts)
    { m_DirectOperationsCosts = DirectOperationsCosts; }
    QSharedPointer<ScenarioIndirectOperationsCosts> IndirectOperationsCosts() const
    { return m_IndirectOperationsCosts; }
    void setIndirectOperationsCosts(QSharedPointer<ScenarioIndirectOperationsCosts> IndirectOperationsCosts)
    { m_IndirectOperationsCosts = IndirectOperationsCosts; }

private:
    QSharedPointer<ScenarioTotalProgramCosts> m_TotalProgramCosts;
    double m_developmentTime;
    QSharedPointer<ScenarioDirectOperationsCosts> m_DirectOperationsCosts;
    QSharedPointer<ScenarioIndirectOperationsCosts> m_IndirectOperationsCosts;
};


// ScenarioTotalProgramCosts
class ScenarioTotalProgramCosts : public ScenarioObject
{
public:
    ScenarioTotalProgramCosts();
    void load(const QDomElement& e);

    double lifeCycleCostMY() const
    { return m_lifeCycleCostMY; }
    void setLifeCycleCostMY(double lifeCycleCostMY)
    { m_lifeCycleCostMY = lifeCycleCostMY; }
    double costPerLaunchMY() const
    { return m_costPerLaunchMY; }
    void setCostPerLaunchMY(double costPerLaunchMY)
    { m_costPerLaunchMY = costPerLaunchMY; }
    double costPerKiloMY() const
    { return m_costPerKiloMY; }
    void setCostPerKiloMY(double costPerKiloMY)
    { m_costPerKiloMY = costPerKiloMY; }
    int FYref() const
    { return m_FYref; }
    void setFYref(int FYref)
    { m_FYref = FYref; }
    double MYtoFYEuros() const
    { return m_MYtoFYEuros; }
    void setMYtoFYEuros(double MYtoFYEuros)
    { m_MYtoFYEuros = MYtoFYEuros; }
    double lifeCycleCostFYEuros() const
    { return m_lifeCycleCostFYEuros; }
    void setLifeCycleCostFYEuros(double lifeCycleCostFYEuros)
    { m_lifeCycleCostFYEuros = lifeCycleCostFYEuros; }
    double costPerLaunchFYEuros() const
    { return m_costPerLaunchFYEuros; }
    void setCostPerLaunchFYEuros(double costPerLaunchFYEuros)
    { m_costPerLaunchFYEuros = costPerLaunchFYEuros; }
    double costPerKiloFYEuros() const
    { return m_costPerKiloFYEuros; }
    void setCostPerKiloFYEuros(double costPerKiloFYEuros)
    { m_costPerKiloFYEuros = costPerKiloFYEuros; }

private:
    double m_lifeCycleCostMY;
    double m_costPerLaunchMY;
    double m_costPerKiloMY;
    int m_FYref;
    double m_MYtoFYEuros;
    double m_lifeCycleCostFYEuros;
    double m_costPerLaunchFYEuros;
    double m_costPerKiloFYEuros;
};


// ScenarioDirectOperationsCosts
class ScenarioDirectOperationsCosts : public ScenarioObject
{
public:
    ScenarioDirectOperationsCosts();
    void load(const QDomElement& e);

    double transportCost() const
    { return m_transportCost; }
    void setTransportCost(double transportCost)
    { m_transportCost = transportCost; }
    QSharedPointer<ScenarioOptVarString> processingType() const
    { return m_processingType; }
    void setProcessingType(QSharedPointer<ScenarioOptVarString> processingType)
    { m_processingType = processingType; }
    double fv() const
    { return m_fv; }
    void setFv(double fv)
    { m_fv = fv; }
    double fc() const
    { return m_fc; }
    void setFc(double fc)
    { m_fc = fc; }
    double groundOperationsCost() const
    { return m_groundOperationsCost; }
    void setGroundOperationsCost(double groundOperationsCost)
    { m_groundOperationsCost = groundOperationsCost; }
    double Qn() const
    { return m_Qn; }
    void setQn(double Qn)
    { m_Qn = Qn; }
    double flightOperationsCost() const
    { return m_flightOperationsCost; }
    void setFlightOperationsCost(double flightOperationsCost)
    { m_flightOperationsCost = flightOperationsCost; }
    double propellantsCost() const
    { return m_propellantsCost; }
    void setPropellantsCost(double propellantsCost)
    { m_propellantsCost = propellantsCost; }
    double insuranceFee() const
    { return m_insuranceFee; }
    void setInsuranceFee(double insuranceFee)
    { m_insuranceFee = insuranceFee; }

private:
    double m_transportCost;
    QSharedPointer<ScenarioOptVarString> m_processingType;
    double m_fv;
    double m_fc;
    double m_groundOperationsCost;
    double m_Qn;
    double m_flightOperationsCost;
    double m_propellantsCost;
    double m_insuranceFee;
};


// ScenarioIndirectOperationsCosts
class ScenarioIndirectOperationsCosts : public ScenarioObject
{
public:
    ScenarioIndirectOperationsCosts();
    void load(const QDomElement& e);

    double groundFacilitiesBuildingCost() const
    { return m_groundFacilitiesBuildingCost; }
    void setGroundFacilitiesBuildingCost(double groundFacilitiesBuildingCost)
    { m_groundFacilitiesBuildingCost = groundFacilitiesBuildingCost; }
    double launchSiteCostPerYear() const
    { return m_launchSiteCostPerYear; }
    void setLaunchSiteCostPerYear(double launchSiteCostPerYear)
    { m_launchSiteCostPerYear = launchSiteCostPerYear; }
    double personnelCostPerYear() const
    { return m_personnelCostPerYear; }
    void setPersonnelCostPerYear(double personnelCostPerYear)
    { m_personnelCostPerYear = personnelCostPerYear; }

private:
    double m_groundFacilitiesBuildingCost;
    double m_launchSiteCostPerYear;
    double m_personnelCostPerYear;
};


// ScenarioComponentReliability
class ScenarioComponentReliability : public ScenarioObject
{
public:
    ScenarioComponentReliability();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioSeparation> Separation() const
    { return m_Separation; }
    void setSeparation(QSharedPointer<ScenarioSeparation> Separation)
    { m_Separation = Separation; }
    double avionicsFailureRate() const
    { return m_avionicsFailureRate; }
    void setAvionicsFailureRate(double avionicsFailureRate)
    { m_avionicsFailureRate = avionicsFailureRate; }
    double structureFailureRate() const
    { return m_structureFailureRate; }
    void setStructureFailureRate(double structureFailureRate)
    { m_structureFailureRate = structureFailureRate; }
    double tpsFailureRate() const
    { return m_tpsFailureRate; }
    void setTpsFailureRate(double tpsFailureRate)
    { m_tpsFailureRate = tpsFailureRate; }
    QSharedPointer<ScenarioPropulsionReliability> PropulsionReliability() const
    { return m_PropulsionReliability; }
    void setPropulsionReliability(QSharedPointer<ScenarioPropulsionReliability> PropulsionReliability)
    { m_PropulsionReliability = PropulsionReliability; }
    double overallComponentFailureRate() const
    { return m_overallComponentFailureRate; }
    void setOverallComponentFailureRate(double overallComponentFailureRate)
    { m_overallComponentFailureRate = overallComponentFailureRate; }

private:
    QSharedPointer<ScenarioSeparation> m_Separation;
    double m_avionicsFailureRate;
    double m_structureFailureRate;
    double m_tpsFailureRate;
    QSharedPointer<ScenarioPropulsionReliability> m_PropulsionReliability;
    double m_overallComponentFailureRate;
};


// ScenarioSeparation
class ScenarioSeparation : public ScenarioObject
{
public:
    ScenarioSeparation();
    void load(const QDomElement& e);

    double stageSeparationReliability() const
    { return m_stageSeparationReliability; }
    void setStageSeparationReliability(double stageSeparationReliability)
    { m_stageSeparationReliability = stageSeparationReliability; }
    double boostersSetSeparationReliability() const
    { return m_boostersSetSeparationReliability; }
    void setBoostersSetSeparationReliability(double boostersSetSeparationReliability)
    { m_boostersSetSeparationReliability = boostersSetSeparationReliability; }
    double fairingSeparationReliability() const
    { return m_fairingSeparationReliability; }
    void setFairingSeparationReliability(double fairingSeparationReliability)
    { m_fairingSeparationReliability = fairingSeparationReliability; }
    double payloadSeparationReliability() const
    { return m_payloadSeparationReliability; }
    void setPayloadSeparationReliability(double payloadSeparationReliability)
    { m_payloadSeparationReliability = payloadSeparationReliability; }

private:
    double m_stageSeparationReliability;
    double m_boostersSetSeparationReliability;
    double m_fairingSeparationReliability;
    double m_payloadSeparationReliability;
};


// ScenarioPropulsionReliability
class ScenarioPropulsionReliability : public ScenarioObject
{
public:
    ScenarioPropulsionReliability();
    void load(const QDomElement& e);

    double igniterReliability() const
    { return m_igniterReliability; }
    void setIgniterReliability(double igniterReliability)
    { m_igniterReliability = igniterReliability; }
    double feedFailureRate() const
    { return m_feedFailureRate; }
    void setFeedFailureRate(double feedFailureRate)
    { m_feedFailureRate = feedFailureRate; }
    double grainFailureRate() const
    { return m_grainFailureRate; }
    void setGrainFailureRate(double grainFailureRate)
    { m_grainFailureRate = grainFailureRate; }
    double chamberFailureRate() const
    { return m_chamberFailureRate; }
    void setChamberFailureRate(double chamberFailureRate)
    { m_chamberFailureRate = chamberFailureRate; }
    double nozzleFailureRate() const
    { return m_nozzleFailureRate; }
    void setNozzleFailureRate(double nozzleFailureRate)
    { m_nozzleFailureRate = nozzleFailureRate; }
    double tvcFailureRate() const
    { return m_tvcFailureRate; }
    void setTvcFailureRate(double tvcFailureRate)
    { m_tvcFailureRate = tvcFailureRate; }
    double singleEngineOverallFailureRate() const
    { return m_singleEngineOverallFailureRate; }
    void setSingleEngineOverallFailureRate(double singleEngineOverallFailureRate)
    { m_singleEngineOverallFailureRate = singleEngineOverallFailureRate; }
    double propulsionOverallFailureRate() const
    { return m_propulsionOverallFailureRate; }
    void setPropulsionOverallFailureRate(double propulsionOverallFailureRate)
    { m_propulsionOverallFailureRate = propulsionOverallFailureRate; }

private:
    double m_igniterReliability;
    double m_feedFailureRate;
    double m_grainFailureRate;
    double m_chamberFailureRate;
    double m_nozzleFailureRate;
    double m_tvcFailureRate;
    double m_singleEngineOverallFailureRate;
    double m_propulsionOverallFailureRate;
};


// ScenarioSystemReliability
class ScenarioSystemReliability : public ScenarioObject
{
public:
    ScenarioSystemReliability();
    void load(const QDomElement& e);

    double globalVehicleReliability() const
    { return m_globalVehicleReliability; }
    void setGlobalVehicleReliability(double globalVehicleReliability)
    { m_globalVehicleReliability = globalVehicleReliability; }
    double selfDestructReliability() const
    { return m_selfDestructReliability; }
    void setSelfDestructReliability(double selfDestructReliability)
    { m_selfDestructReliability = selfDestructReliability; }
    double missionSafety() const
    { return m_missionSafety; }
    void setMissionSafety(double missionSafety)
    { m_missionSafety = missionSafety; }
    double preLaunchReliability() const
    { return m_preLaunchReliability; }
    void setPreLaunchReliability(double preLaunchReliability)
    { m_preLaunchReliability = preLaunchReliability; }
    QSharedPointer<ScenarioLaunch> Launch() const
    { return m_Launch; }
    void setLaunch(QSharedPointer<ScenarioLaunch> Launch)
    { m_Launch = Launch; }
    int numberOfAscentPhases() const
    { return m_numberOfAscentPhases; }
    void setNumberOfAscentPhases(int numberOfAscentPhases)
    { m_numberOfAscentPhases = numberOfAscentPhases; }
    const QList<double>& ascentPhasesDurations() const
    { return m_ascentPhasesDurations; }
    QList<double>& ascentPhasesDurations()
    { return m_ascentPhasesDurations; }
    void setAscentPhasesDurations(QList<double> ascentPhasesDurations)
    { m_ascentPhasesDurations = ascentPhasesDurations; }
    const QList<double>& ascentPhasesFailureRate() const
    { return m_ascentPhasesFailureRate; }
    QList<double>& ascentPhasesFailureRate()
    { return m_ascentPhasesFailureRate; }
    void setAscentPhasesFailureRate(QList<double> ascentPhasesFailureRate)
    { m_ascentPhasesFailureRate = ascentPhasesFailureRate; }

private:
    double m_globalVehicleReliability;
    double m_selfDestructReliability;
    double m_missionSafety;
    double m_preLaunchReliability;
    QSharedPointer<ScenarioLaunch> m_Launch;
    int m_numberOfAscentPhases;
    QList<double> m_ascentPhasesDurations;
    QList<double> m_ascentPhasesFailureRate;
};


// ScenarioLaunch
class ScenarioLaunch : public ScenarioObject
{
public:
    ScenarioLaunch();
    void load(const QDomElement& e);

    double launchSiteReliability() const
    { return m_launchSiteReliability; }
    void setLaunchSiteReliability(double launchSiteReliability)
    { m_launchSiteReliability = launchSiteReliability; }
    double ignitionReliability() const
    { return m_ignitionReliability; }
    void setIgnitionReliability(double ignitionReliability)
    { m_ignitionReliability = ignitionReliability; }
    double controlMarginReliability() const
    { return m_controlMarginReliability; }
    void setControlMarginReliability(double controlMarginReliability)
    { m_controlMarginReliability = controlMarginReliability; }
    double launchOverallReliability() const
    { return m_launchOverallReliability; }
    void setLaunchOverallReliability(double launchOverallReliability)
    { m_launchOverallReliability = launchOverallReliability; }

private:
    double m_launchSiteReliability;
    double m_ignitionReliability;
    double m_controlMarginReliability;
    double m_launchOverallReliability;
};


// ScenarioTrajectory
class ScenarioTrajectory : public ScenarioObject
{
public:
    ScenarioTrajectory();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioTakeOff> TakeOff() const
    { return m_TakeOff; }
    void setTakeOff(QSharedPointer<ScenarioTakeOff> TakeOff)
    { m_TakeOff = TakeOff; }
    QSharedPointer<ScenarioIgnitions> Ignitions() const
    { return m_Ignitions; }
    void setIgnitions(QSharedPointer<ScenarioIgnitions> Ignitions)
    { m_Ignitions = Ignitions; }
    QSharedPointer<ScenarioAtmosphericFlight> AtmosphericFlight() const
    { return m_AtmosphericFlight; }
    void setAtmosphericFlight(QSharedPointer<ScenarioAtmosphericFlight> AtmosphericFlight)
    { m_AtmosphericFlight = AtmosphericFlight; }
    QSharedPointer<ScenarioExoatmosphericFlight> ExoatmosphericFlight() const
    { return m_ExoatmosphericFlight; }
    void setExoatmosphericFlight(QSharedPointer<ScenarioExoatmosphericFlight> ExoatmosphericFlight)
    { m_ExoatmosphericFlight = ExoatmosphericFlight; }
    QSharedPointer<ScenarioConstraints> Constraints() const
    { return m_Constraints; }
    void setConstraints(QSharedPointer<ScenarioConstraints> Constraints)
    { m_Constraints = Constraints; }

private:
    QSharedPointer<ScenarioTakeOff> m_TakeOff;
    QSharedPointer<ScenarioIgnitions> m_Ignitions;
    QSharedPointer<ScenarioAtmosphericFlight> m_AtmosphericFlight;
    QSharedPointer<ScenarioExoatmosphericFlight> m_ExoatmosphericFlight;
    QSharedPointer<ScenarioConstraints> m_Constraints;
};


// ScenarioTakeOff
class ScenarioTakeOff : public ScenarioObject
{
public:
    ScenarioTakeOff();
    void load(const QDomElement& e);

    double padClearingAlt() const
    { return m_padClearingAlt; }
    void setPadClearingAlt(double padClearingAlt)
    { m_padClearingAlt = padClearingAlt; }
    double maxPitchOverAngle() const
    { return m_maxPitchOverAngle; }
    void setMaxPitchOverAngle(double maxPitchOverAngle)
    { m_maxPitchOverAngle = maxPitchOverAngle; }
    double pitchOverDuration() const
    { return m_pitchOverDuration; }
    void setPitchOverDuration(double pitchOverDuration)
    { m_pitchOverDuration = pitchOverDuration; }
    double pitchOverDecayTime() const
    { return m_pitchOverDecayTime; }
    void setPitchOverDecayTime(double pitchOverDecayTime)
    { m_pitchOverDecayTime = pitchOverDecayTime; }
    double pitchOverHeadingAngle() const
    { return m_pitchOverHeadingAngle; }
    void setPitchOverHeadingAngle(double pitchOverHeadingAngle)
    { m_pitchOverHeadingAngle = pitchOverHeadingAngle; }

private:
    double m_padClearingAlt;
    double m_maxPitchOverAngle;
    double m_pitchOverDuration;
    double m_pitchOverDecayTime;
    double m_pitchOverHeadingAngle;
};


// ScenarioIgnitions
class ScenarioIgnitions : public ScenarioObject
{
public:
    ScenarioIgnitions();
    void load(const QDomElement& e);

    bool coreIgnitionDelayOption() const
    { return m_coreIgnitionDelayOption; }
    void setCoreIgnitionDelayOption(bool coreIgnitionDelayOption)
    { m_coreIgnitionDelayOption = coreIgnitionDelayOption; }
    double coreIgnitionDelayTime() const
    { return m_coreIgnitionDelayTime; }
    void setCoreIgnitionDelayTime(double coreIgnitionDelayTime)
    { m_coreIgnitionDelayTime = coreIgnitionDelayTime; }
    bool secondBoostersSetIgnitionDelayOption() const
    { return m_secondBoostersSetIgnitionDelayOption; }
    void setSecondBoostersSetIgnitionDelayOption(bool secondBoostersSetIgnitionDelayOption)
    { m_secondBoostersSetIgnitionDelayOption = secondBoostersSetIgnitionDelayOption; }
    double secondBoostersSetIgnitionDelayTime() const
    { return m_secondBoostersSetIgnitionDelayTime; }
    void setSecondBoostersSetIgnitionDelayTime(double secondBoostersSetIgnitionDelayTime)
    { m_secondBoostersSetIgnitionDelayTime = secondBoostersSetIgnitionDelayTime; }

private:
    bool m_coreIgnitionDelayOption;
    double m_coreIgnitionDelayTime;
    bool m_secondBoostersSetIgnitionDelayOption;
    double m_secondBoostersSetIgnitionDelayTime;
};


// ScenarioAtmosphericFlight
class ScenarioAtmosphericFlight : public ScenarioObject
{
public:
    ScenarioAtmosphericFlight();
    void load(const QDomElement& e);

    const QList<double>& optimizedPitchValues() const
    { return m_optimizedPitchValues; }
    QList<double>& optimizedPitchValues()
    { return m_optimizedPitchValues; }
    void setOptimizedPitchValues(QList<double> optimizedPitchValues)
    { m_optimizedPitchValues = optimizedPitchValues; }
    const QList<double>& optimizedYawValues() const
    { return m_optimizedYawValues; }
    QList<double>& optimizedYawValues()
    { return m_optimizedYawValues; }
    void setOptimizedYawValues(QList<double> optimizedYawValues)
    { m_optimizedYawValues = optimizedYawValues; }
    const QList<double>& optimizedThrustValues() const
    { return m_optimizedThrustValues; }
    QList<double>& optimizedThrustValues()
    { return m_optimizedThrustValues; }
    void setOptimizedThrustValues(QList<double> optimizedThrustValues)
    { m_optimizedThrustValues = optimizedThrustValues; }

private:
    QList<double> m_optimizedPitchValues;
    QList<double> m_optimizedYawValues;
    QList<double> m_optimizedThrustValues;
};


// ScenarioExoatmosphericFlight
class ScenarioExoatmosphericFlight : public ScenarioObject
{
public:
    ScenarioExoatmosphericFlight();
    void load(const QDomElement& e);

    double bilinearLawInitPitch() const
    { return m_bilinearLawInitPitch; }
    void setBilinearLawInitPitch(double bilinearLawInitPitch)
    { m_bilinearLawInitPitch = bilinearLawInitPitch; }
    double bilinearLawFinalPitch() const
    { return m_bilinearLawFinalPitch; }
    void setBilinearLawFinalPitch(double bilinearLawFinalPitch)
    { m_bilinearLawFinalPitch = bilinearLawFinalPitch; }
    double bilinearLawParam() const
    { return m_bilinearLawParam; }
    void setBilinearLawParam(double bilinearLawParam)
    { m_bilinearLawParam = bilinearLawParam; }
    bool circBurnOption() const
    { return m_circBurnOption; }
    void setCircBurnOption(bool circBurnOption)
    { m_circBurnOption = circBurnOption; }
    double circBurnTime() const
    { return m_circBurnTime; }
    void setCircBurnTime(double circBurnTime)
    { m_circBurnTime = circBurnTime; }

private:
    double m_bilinearLawInitPitch;
    double m_bilinearLawFinalPitch;
    double m_bilinearLawParam;
    bool m_circBurnOption;
    double m_circBurnTime;
};


// ScenarioConstraints
class ScenarioConstraints : public ScenarioObject
{
public:
    ScenarioConstraints();
    void load(const QDomElement& e);

    double finalSemiaxisError() const
    { return m_finalSemiaxisError; }
    void setFinalSemiaxisError(double finalSemiaxisError)
    { m_finalSemiaxisError = finalSemiaxisError; }
    double finalEccError() const
    { return m_finalEccError; }
    void setFinalEccError(double finalEccError)
    { m_finalEccError = finalEccError; }
    double finalInclError() const
    { return m_finalInclError; }
    void setFinalInclError(double finalInclError)
    { m_finalInclError = finalInclError; }
    double axialAccCstrViolation() const
    { return m_axialAccCstrViolation; }
    void setAxialAccCstrViolation(double axialAccCstrViolation)
    { m_axialAccCstrViolation = axialAccCstrViolation; }
    double heatFluxCstrViolation() const
    { return m_heatFluxCstrViolation; }
    void setHeatFluxCstrViolation(double heatFluxCstrViolation)
    { m_heatFluxCstrViolation = heatFluxCstrViolation; }
    double dynPressCstrViolation() const
    { return m_dynPressCstrViolation; }
    void setDynPressCstrViolation(double dynPressCstrViolation)
    { m_dynPressCstrViolation = dynPressCstrViolation; }
    double controllabilityCstrViolation() const
    { return m_controllabilityCstrViolation; }
    void setControllabilityCstrViolation(double controllabilityCstrViolation)
    { m_controllabilityCstrViolation = controllabilityCstrViolation; }

private:
    double m_finalSemiaxisError;
    double m_finalEccError;
    double m_finalInclError;
    double m_axialAccCstrViolation;
    double m_heatFluxCstrViolation;
    double m_dynPressCstrViolation;
    double m_controllabilityCstrViolation;
};


// ScenarioInitialPositionType
class ScenarioInitialPositionType : public ScenarioObject
{
public:
    ScenarioInitialPositionType();
    void load(const QDomElement& e);

    QString CoordinateSystem() const
    { return m_CoordinateSystem; }
    void setCoordinateSystem(QString CoordinateSystem)
    { m_CoordinateSystem = CoordinateSystem; }
    QSharedPointer<ScenarioAbstract6DOFPositionType> Abstract6DOFPosition() const
    { return m_Abstract6DOFPosition; }
    void setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType> Abstract6DOFPosition)
    { m_Abstract6DOFPosition = Abstract6DOFPosition; }

private:
    QString m_CoordinateSystem;
    QSharedPointer<ScenarioAbstract6DOFPositionType> m_Abstract6DOFPosition;
};


// ScenarioSC
class ScenarioSC : public ScenarioParticipantsType
{
public:
    ScenarioSC();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioSCProgram> SCProgram() const
    { return m_SCProgram; }
    void setSCProgram(QSharedPointer<ScenarioSCProgram> SCProgram)
    { m_SCProgram = SCProgram; }
    QSharedPointer<ScenarioSCMission> SCMission() const
    { return m_SCMission; }
    void setSCMission(QSharedPointer<ScenarioSCMission> SCMission)
    { m_SCMission = SCMission; }
    QSharedPointer<ScenarioSCSystemType> System() const
    { return m_System; }
    void setSystem(QSharedPointer<ScenarioSCSystemType> System)
    { m_System = System; }
    QSharedPointer<ScenarioOptimization> Optimization() const
    { return m_Optimization; }
    void setOptimization(QSharedPointer<ScenarioOptimization> Optimization)
    { m_Optimization = Optimization; }
    QSharedPointer<ScenarioOutputFiles> OutputFiles() const
    { return m_OutputFiles; }
    void setOutputFiles(QSharedPointer<ScenarioOutputFiles> OutputFiles)
    { m_OutputFiles = OutputFiles; }

private:
    QSharedPointer<ScenarioSCProgram> m_SCProgram;
    QSharedPointer<ScenarioSCMission> m_SCMission;
    QSharedPointer<ScenarioSCSystemType> m_System;
    QSharedPointer<ScenarioOptimization> m_Optimization;
    QSharedPointer<ScenarioOutputFiles> m_OutputFiles;
};


// ScenarioSCProgram
class ScenarioSCProgram : public ScenarioObject
{
public:
    ScenarioSCProgram();
    void load(const QDomElement& e);


private:
};


// ScenarioSCMission
class ScenarioSCMission : public ScenarioObject
{
public:
    ScenarioSCMission();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioPayload> Payload() const
    { return m_Payload; }
    void setPayload(QSharedPointer<ScenarioPayload> Payload)
    { m_Payload = Payload; }
    QSharedPointer<ScenarioTrajectoryPlan> TrajectoryPlan() const
    { return m_TrajectoryPlan; }
    void setTrajectoryPlan(QSharedPointer<ScenarioTrajectoryPlan> TrajectoryPlan)
    { m_TrajectoryPlan = TrajectoryPlan; }

private:
    QSharedPointer<ScenarioPayload> m_Payload;
    QSharedPointer<ScenarioTrajectoryPlan> m_TrajectoryPlan;
};


// ScenarioPayload
class ScenarioPayload : public ScenarioObject
{
public:
    ScenarioPayload();
    void load(const QDomElement& e);

    double mass() const
    { return m_mass; }
    void setMass(double mass)
    { m_mass = mass; }
    double power() const
    { return m_power; }
    void setPower(double power)
    { m_power = power; }
    QSharedPointer<ScenarioObservationAntenna> ObservationAntenna() const
    { return m_ObservationAntenna; }
    void setObservationAntenna(QSharedPointer<ScenarioObservationAntenna> ObservationAntenna)
    { m_ObservationAntenna = ObservationAntenna; }

private:
    double m_mass;
    double m_power;
    QSharedPointer<ScenarioObservationAntenna> m_ObservationAntenna;
};


// ScenarioTrajectoryPlan
class ScenarioTrajectoryPlan : public ScenarioObject
{
public:
    ScenarioTrajectoryPlan();
    void load(const QDomElement& e);

    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& AbstractTrajectory() const
    { return m_AbstractTrajectory; }
    QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& AbstractTrajectory()
    { return m_AbstractTrajectory; }
    void setAbstractTrajectory(QList<QSharedPointer<ScenarioAbstractTrajectoryType> > AbstractTrajectory)
    { m_AbstractTrajectory = AbstractTrajectory; }

private:
    QList<QSharedPointer<ScenarioAbstractTrajectoryType> > m_AbstractTrajectory;
};


// ScenarioAbstractTrajectoryType
class ScenarioAbstractTrajectoryType : public ScenarioObject
{
public:
    ScenarioAbstractTrajectoryType();
    void load(const QDomElement& e);


private:
};


// ScenarioSCEnvironmentType
class ScenarioSCEnvironmentType : public ScenarioEnvironmentType
{
public:
    ScenarioSCEnvironmentType();
    void load(const QDomElement& e);

    const QList<QString>& perturbingBody() const
    { return m_perturbingBody; }
    QList<QString>& perturbingBody()
    { return m_perturbingBody; }
    void setPerturbingBody(QList<QString> perturbingBody)
    { m_perturbingBody = perturbingBody; }
    bool atmosphericDrag() const
    { return m_atmosphericDrag; }
    void setAtmosphericDrag(bool atmosphericDrag)
    { m_atmosphericDrag = atmosphericDrag; }
    bool solarPressure() const
    { return m_solarPressure; }
    void setSolarPressure(bool solarPressure)
    { m_solarPressure = solarPressure; }

private:
    QList<QString> m_perturbingBody;
    bool m_atmosphericDrag;
    bool m_solarPressure;
};


// ScenarioLoiteringType
class ScenarioLoiteringType : public ScenarioAbstractTrajectoryType
{
public:
    ScenarioLoiteringType();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioSCEnvironmentType> Environment() const
    { return m_Environment; }
    void setEnvironment(QSharedPointer<ScenarioSCEnvironmentType> Environment)
    { m_Environment = Environment; }
    QSharedPointer<ScenarioTimeLine> TimeLine() const
    { return m_TimeLine; }
    void setTimeLine(QSharedPointer<ScenarioTimeLine> TimeLine)
    { m_TimeLine = TimeLine; }
    QSharedPointer<ScenarioInitialPositionType> InitialPosition() const
    { return m_InitialPosition; }
    void setInitialPosition(QSharedPointer<ScenarioInitialPositionType> InitialPosition)
    { m_InitialPosition = InitialPosition; }
    QSharedPointer<ScenarioInitialAttitude> InitialAttitude() const
    { return m_InitialAttitude; }
    void setInitialAttitude(QSharedPointer<ScenarioInitialAttitude> InitialAttitude)
    { m_InitialAttitude = InitialAttitude; }
    QSharedPointer<ScenarioPropagationPosition> PropagationPosition() const
    { return m_PropagationPosition; }
    void setPropagationPosition(QSharedPointer<ScenarioPropagationPosition> PropagationPosition)
    { m_PropagationPosition = PropagationPosition; }
    QSharedPointer<ScenarioPropagationAttitude> PropagationAttitude() const
    { return m_PropagationAttitude; }
    void setPropagationAttitude(QSharedPointer<ScenarioPropagationAttitude> PropagationAttitude)
    { m_PropagationAttitude = PropagationAttitude; }

private:
    QSharedPointer<ScenarioSCEnvironmentType> m_Environment;
    QSharedPointer<ScenarioTimeLine> m_TimeLine;
    QSharedPointer<ScenarioInitialPositionType> m_InitialPosition;
    QSharedPointer<ScenarioInitialAttitude> m_InitialAttitude;
    QSharedPointer<ScenarioPropagationPosition> m_PropagationPosition;
    QSharedPointer<ScenarioPropagationAttitude> m_PropagationAttitude;
};


// ScenarioInitialAttitude
class ScenarioInitialAttitude : public ScenarioObject
{
public:
    ScenarioInitialAttitude();
    void load(const QDomElement& e);

    QString CoordinateSystem() const
    { return m_CoordinateSystem; }
    void setCoordinateSystem(QString CoordinateSystem)
    { m_CoordinateSystem = CoordinateSystem; }
    QSharedPointer<ScenarioAbstract6DOFAttitudeType> Abstract6DOFAttitude() const
    { return m_Abstract6DOFAttitude; }
    void setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType> Abstract6DOFAttitude)
    { m_Abstract6DOFAttitude = Abstract6DOFAttitude; }

private:
    QString m_CoordinateSystem;
    QSharedPointer<ScenarioAbstract6DOFAttitudeType> m_Abstract6DOFAttitude;
};


// ScenarioPropagationPosition
class ScenarioPropagationPosition : public ScenarioObject
{
public:
    ScenarioPropagationPosition();
    void load(const QDomElement& e);

    QString propagator() const
    { return m_propagator; }
    void setPropagator(QString propagator)
    { m_propagator = propagator; }
    QString integrator() const
    { return m_integrator; }
    void setIntegrator(QString integrator)
    { m_integrator = integrator; }
    double timeStep() const
    { return m_timeStep; }
    void setTimeStep(double timeStep)
    { m_timeStep = timeStep; }

private:
    QString m_propagator;
    QString m_integrator;
    double m_timeStep;
};


// ScenarioPropagationAttitude
class ScenarioPropagationAttitude : public ScenarioObject
{
public:
    ScenarioPropagationAttitude();
    void load(const QDomElement& e);

    QString integrator() const
    { return m_integrator; }
    void setIntegrator(QString integrator)
    { m_integrator = integrator; }
    double timeStep() const
    { return m_timeStep; }
    void setTimeStep(double timeStep)
    { m_timeStep = timeStep; }

private:
    QString m_integrator;
    double m_timeStep;
};


// ScenarioRendezvousType
class ScenarioRendezvousType : public ScenarioAbstractTrajectoryType
{
public:
    ScenarioRendezvousType();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioSCEnvironmentType> Environment() const
    { return m_Environment; }
    void setEnvironment(QSharedPointer<ScenarioSCEnvironmentType> Environment)
    { m_Environment = Environment; }
    QSharedPointer<ScenarioParameters> Parameters() const
    { return m_Parameters; }
    void setParameters(QSharedPointer<ScenarioParameters> Parameters)
    { m_Parameters = Parameters; }
    QString Target() const
    { return m_Target; }
    void setTarget(QString Target)
    { m_Target = Target; }
    QSharedPointer<ScenarioManoeuvrePlan> ManoeuvrePlan() const
    { return m_ManoeuvrePlan; }
    void setManoeuvrePlan(QSharedPointer<ScenarioManoeuvrePlan> ManoeuvrePlan)
    { m_ManoeuvrePlan = ManoeuvrePlan; }

private:
    QSharedPointer<ScenarioSCEnvironmentType> m_Environment;
    QSharedPointer<ScenarioParameters> m_Parameters;
    QString m_Target;
    QSharedPointer<ScenarioManoeuvrePlan> m_ManoeuvrePlan;
};


// ScenarioParameters
class ScenarioParameters : public ScenarioObject
{
public:
    ScenarioParameters();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioTimeLine> TimeLine() const
    { return m_TimeLine; }
    void setTimeLine(QSharedPointer<ScenarioTimeLine> TimeLine)
    { m_TimeLine = TimeLine; }
    QSharedPointer<ScenarioInitialPositionType> InitialPosition() const
    { return m_InitialPosition; }
    void setInitialPosition(QSharedPointer<ScenarioInitialPositionType> InitialPosition)
    { m_InitialPosition = InitialPosition; }

private:
    QSharedPointer<ScenarioTimeLine> m_TimeLine;
    QSharedPointer<ScenarioInitialPositionType> m_InitialPosition;
};


// ScenarioManoeuvrePlan
class ScenarioManoeuvrePlan : public ScenarioObject
{
public:
    ScenarioManoeuvrePlan();
    void load(const QDomElement& e);

    const QList<QSharedPointer<ScenarioManoeuvreType> >& AbstractManoeuvre() const
    { return m_AbstractManoeuvre; }
    QList<QSharedPointer<ScenarioManoeuvreType> >& AbstractManoeuvre()
    { return m_AbstractManoeuvre; }
    void setAbstractManoeuvre(QList<QSharedPointer<ScenarioManoeuvreType> > AbstractManoeuvre)
    { m_AbstractManoeuvre = AbstractManoeuvre; }

private:
    QList<QSharedPointer<ScenarioManoeuvreType> > m_AbstractManoeuvre;
};


// ScenarioManoeuvreType
class ScenarioManoeuvreType : public ScenarioObject
{
public:
    ScenarioManoeuvreType();
    void load(const QDomElement& e);


private:
};


// ScenarioSTA_MANOEUVRE_DURATION
class ScenarioSTA_MANOEUVRE_DURATION : public ScenarioManoeuvreType
{
public:
    ScenarioSTA_MANOEUVRE_DURATION();
    void load(const QDomElement& e);

    double Duration() const
    { return m_Duration; }
    void setDuration(double Duration)
    { m_Duration = Duration; }

private:
    double m_Duration;
};


// ScenarioSTA_MANOEUVRE_DELTAV
class ScenarioSTA_MANOEUVRE_DELTAV : public ScenarioManoeuvreType
{
public:
    ScenarioSTA_MANOEUVRE_DELTAV();
    void load(const QDomElement& e);

    double DeltaVx() const
    { return m_DeltaVx; }
    void setDeltaVx(double DeltaVx)
    { m_DeltaVx = DeltaVx; }
    double DeltaVy() const
    { return m_DeltaVy; }
    void setDeltaVy(double DeltaVy)
    { m_DeltaVy = DeltaVy; }
    double DeltaVz() const
    { return m_DeltaVz; }
    void setDeltaVz(double DeltaVz)
    { m_DeltaVz = DeltaVz; }
    double Duration() const
    { return m_Duration; }
    void setDuration(double Duration)
    { m_Duration = Duration; }

private:
    double m_DeltaVx;
    double m_DeltaVy;
    double m_DeltaVz;
    double m_Duration;
};


// ScenarioSTA_MANOEUVRE_V_POSITION
class ScenarioSTA_MANOEUVRE_V_POSITION : public ScenarioManoeuvreType
{
public:
    ScenarioSTA_MANOEUVRE_V_POSITION();
    void load(const QDomElement& e);

    double DisplacementVbar() const
    { return m_DisplacementVbar; }
    void setDisplacementVbar(double DisplacementVbar)
    { m_DisplacementVbar = DisplacementVbar; }

private:
    double m_DisplacementVbar;
};


// ScenarioSTA_MANOEUVRE_R_POSITION
class ScenarioSTA_MANOEUVRE_R_POSITION : public ScenarioManoeuvreType
{
public:
    ScenarioSTA_MANOEUVRE_R_POSITION();
    void load(const QDomElement& e);

    double DisplacementRbar() const
    { return m_DisplacementRbar; }
    void setDisplacementRbar(double DisplacementRbar)
    { m_DisplacementRbar = DisplacementRbar; }

private:
    double m_DisplacementRbar;
};


// ScenarioSTA_MANOEUVRE_V_R_POSITION
class ScenarioSTA_MANOEUVRE_V_R_POSITION : public ScenarioManoeuvreType
{
public:
    ScenarioSTA_MANOEUVRE_V_R_POSITION();
    void load(const QDomElement& e);

    double DisplacementVbar() const
    { return m_DisplacementVbar; }
    void setDisplacementVbar(double DisplacementVbar)
    { m_DisplacementVbar = DisplacementVbar; }
    double DisplacementRbar() const
    { return m_DisplacementRbar; }
    void setDisplacementRbar(double DisplacementRbar)
    { m_DisplacementRbar = DisplacementRbar; }

private:
    double m_DisplacementVbar;
    double m_DisplacementRbar;
};


// ScenarioSTA_MANOEUVRE_V_R_DURATION
class ScenarioSTA_MANOEUVRE_V_R_DURATION : public ScenarioManoeuvreType
{
public:
    ScenarioSTA_MANOEUVRE_V_R_DURATION();
    void load(const QDomElement& e);

    double DisplacementVbar() const
    { return m_DisplacementVbar; }
    void setDisplacementVbar(double DisplacementVbar)
    { m_DisplacementVbar = DisplacementVbar; }
    double DisplacementRbar() const
    { return m_DisplacementRbar; }
    void setDisplacementRbar(double DisplacementRbar)
    { m_DisplacementRbar = DisplacementRbar; }
    double Duration() const
    { return m_Duration; }
    void setDuration(double Duration)
    { m_Duration = Duration; }

private:
    double m_DisplacementVbar;
    double m_DisplacementRbar;
    double m_Duration;
};


// ScenarioLagrangianType
class ScenarioLagrangianType : public ScenarioAbstractTrajectoryType
{
public:
    ScenarioLagrangianType();
    void load(const QDomElement& e);


private:
};


// ScenarioFlyByType
class ScenarioFlyByType : public ScenarioAbstractTrajectoryType
{
public:
    ScenarioFlyByType();
    void load(const QDomElement& e);


private:
};


// ScenarioSCSystemType
class ScenarioSCSystemType : public ScenarioObject
{
public:
    ScenarioSCSystemType();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioSystemBudgets> SystemBudgets() const
    { return m_SystemBudgets; }
    void setSystemBudgets(QSharedPointer<ScenarioSystemBudgets> SystemBudgets)
    { m_SystemBudgets = SystemBudgets; }
    QSharedPointer<ScenarioSCAerodynamics> SCAerodynamics() const
    { return m_SCAerodynamics; }
    void setSCAerodynamics(QSharedPointer<ScenarioSCAerodynamics> SCAerodynamics)
    { m_SCAerodynamics = SCAerodynamics; }
    QSharedPointer<ScenarioPropulsion> Propulsion() const
    { return m_Propulsion; }
    void setPropulsion(QSharedPointer<ScenarioPropulsion> Propulsion)
    { m_Propulsion = Propulsion; }
    QSharedPointer<ScenarioStructure> Structure() const
    { return m_Structure; }
    void setStructure(QSharedPointer<ScenarioStructure> Structure)
    { m_Structure = Structure; }
    QSharedPointer<ScenarioTCS> TCS() const
    { return m_TCS; }
    void setTCS(QSharedPointer<ScenarioTCS> TCS)
    { m_TCS = TCS; }
    QSharedPointer<ScenarioEPS> EPS() const
    { return m_EPS; }
    void setEPS(QSharedPointer<ScenarioEPS> EPS)
    { m_EPS = EPS; }
    QSharedPointer<ScenarioTTC> TTC() const
    { return m_TTC; }
    void setTTC(QSharedPointer<ScenarioTTC> TTC)
    { m_TTC = TTC; }
    QSharedPointer<ScenarioAOCS> AOCS() const
    { return m_AOCS; }
    void setAOCS(QSharedPointer<ScenarioAOCS> AOCS)
    { m_AOCS = AOCS; }
    QSharedPointer<ScenarioOBDH> OBDH() const
    { return m_OBDH; }
    void setOBDH(QSharedPointer<ScenarioOBDH> OBDH)
    { m_OBDH = OBDH; }

private:
    QSharedPointer<ScenarioSystemBudgets> m_SystemBudgets;
    QSharedPointer<ScenarioSCAerodynamics> m_SCAerodynamics;
    QSharedPointer<ScenarioPropulsion> m_Propulsion;
    QSharedPointer<ScenarioStructure> m_Structure;
    QSharedPointer<ScenarioTCS> m_TCS;
    QSharedPointer<ScenarioEPS> m_EPS;
    QSharedPointer<ScenarioTTC> m_TTC;
    QSharedPointer<ScenarioAOCS> m_AOCS;
    QSharedPointer<ScenarioOBDH> m_OBDH;
};


// ScenarioSystemBudgets
class ScenarioSystemBudgets : public ScenarioObject
{
public:
    ScenarioSystemBudgets();
    void load(const QDomElement& e);

    QSharedPointer<ScenarioMass> Mass() const
    { return m_Mass; }
    void setMass(QSharedPointer<ScenarioMass> Mass)
    { m_Mass = Mass; }
    QSharedPointer<ScenarioPower> Power() const
    { return m_Power; }
    void setPower(QSharedPointer<ScenarioPower> Power)
    { m_Power = Power; }
    QSharedPointer<ScenarioLink> Link() const
    { return m_Link; }
    void setLink(QSharedPointer<ScenarioLink> Link)
    { m_Link = Link; }

private:
    QSharedPointer<ScenarioMass> m_Mass;
    QSharedPointer<ScenarioPower> m_Power;
    QSharedPointer<ScenarioLink> m_Link;
};


// ScenarioMass
class ScenarioMass : public ScenarioObject
{
public:
    ScenarioMass();
    void load(const QDomElement& e);

    double dryMass() const
    { return m_dryMass; }
    void setDryMass(double dryMass)
    { m_dryMass = dryMass; }
    double wetMass() const
    { return m_wetMass; }
    void setWetMass(double wetMass)
    { m_wetMass = wetMass; }

private:
    double m_dryMass;
    double m_wetMass;
};


// ScenarioPower
class ScenarioPower : public ScenarioObject
{
public:
    ScenarioPower();
    void load(const QDomElement& e);

    double totalPowerBoL() const
    { return m_totalPowerBoL; }
    void setTotalPowerBoL(double totalPowerBoL)
    { m_totalPowerBoL = totalPowerBoL; }
    double totalPowerEoL() const
    { return m_totalPowerEoL; }
    void setTotalPowerEoL(double totalPowerEoL)
    { m_totalPowerEoL = totalPowerEoL; }

private:
    double m_totalPowerBoL;
    double m_totalPowerEoL;
};


// ScenarioLink
class ScenarioLink : public ScenarioObject
{
public:
    ScenarioLink();
    void load(const QDomElement& e);

    double uplinkMargin() const
    { return m_uplinkMargin; }
    void setUplinkMargin(double uplinkMargin)
    { m_uplinkMargin = uplinkMargin; }
    double downlinkMargin() const
    { return m_downlinkMargin; }
    void setDownlinkMargin(double downlinkMargin)
    { m_downlinkMargin = downlinkMargin; }

private:
    double m_uplinkMargin;
    double m_downlinkMargin;
};


// ScenarioSCAerodynamics
class ScenarioSCAerodynamics : public ScenarioObject
{
public:
    ScenarioSCAerodynamics();
    void load(const QDomElement& e);

    double surfaceArea() const
    { return m_surfaceArea; }
    void setSurfaceArea(double surfaceArea)
    { m_surfaceArea = surfaceArea; }
    double Cd() const
    { return m_Cd; }
    void setCd(double Cd)
    { m_Cd = Cd; }

private:
    double m_surfaceArea;
    double m_Cd;
};


// ScenarioPropulsion
class ScenarioPropulsion : public ScenarioObject
{
public:
    ScenarioPropulsion();
    void load(const QDomElement& e);

    int numberOfEngines() const
    { return m_numberOfEngines; }
    void setNumberOfEngines(int numberOfEngines)
    { m_numberOfEngines = numberOfEngines; }
    double thrustPerEngine() const
    { return m_thrustPerEngine; }
    void setThrustPerEngine(double thrustPerEngine)
    { m_thrustPerEngine = thrustPerEngine; }
    double specificImpulse() const
    { return m_specificImpulse; }
    void setSpecificImpulse(double specificImpulse)
    { m_specificImpulse = specificImpulse; }
    double propellantMass() const
    { return m_propellantMass; }
    void setPropellantMass(double propellantMass)
    { m_propellantMass = propellantMass; }
    double totalPropulsionDryMass() const
    { return m_totalPropulsionDryMass; }
    void setTotalPropulsionDryMass(double totalPropulsionDryMass)
    { m_totalPropulsionDryMass = totalPropulsionDryMass; }

private:
    int m_numberOfEngines;
    double m_thrustPerEngine;
    double m_specificImpulse;
    double m_propellantMass;
    double m_totalPropulsionDryMass;
};


// ScenarioStructure
class ScenarioStructure : public ScenarioObject
{
public:
    ScenarioStructure();
    void load(const QDomElement& e);

    double totalStructureMass() const
    { return m_totalStructureMass; }
    void setTotalStructureMass(double totalStructureMass)
    { m_totalStructureMass = totalStructureMass; }

private:
    double m_totalStructureMass;
};


// ScenarioTCS
class ScenarioTCS : public ScenarioObject
{
public:
    ScenarioTCS();
    void load(const QDomElement& e);

    double totalTCSMass() const
    { return m_totalTCSMass; }
    void setTotalTCSMass(double totalTCSMass)
    { m_totalTCSMass = totalTCSMass; }

private:
    double m_totalTCSMass;
};


// ScenarioEPS
class ScenarioEPS : public ScenarioObject
{
public:
    ScenarioEPS();
    void load(const QDomElement& e);

    double totalEPSMass() const
    { return m_totalEPSMass; }
    void setTotalEPSMass(double totalEPSMass)
    { m_totalEPSMass = totalEPSMass; }

private:
    double m_totalEPSMass;
};


// ScenarioTTC
class ScenarioTTC : public ScenarioObject
{
public:
    ScenarioTTC();
    void load(const QDomElement& e);

    double totalTTCMass() const
    { return m_totalTTCMass; }
    void setTotalTTCMass(double totalTTCMass)
    { m_totalTTCMass = totalTTCMass; }
    const QList<QSharedPointer<ScenarioCommunicationAntenna> >& CommunicationAntenna() const
    { return m_CommunicationAntenna; }
    QList<QSharedPointer<ScenarioCommunicationAntenna> >& CommunicationAntenna()
    { return m_CommunicationAntenna; }
    void setCommunicationAntenna(QList<QSharedPointer<ScenarioCommunicationAntenna> > CommunicationAntenna)
    { m_CommunicationAntenna = CommunicationAntenna; }

private:
    double m_totalTTCMass;
    QList<QSharedPointer<ScenarioCommunicationAntenna> > m_CommunicationAntenna;
};


// ScenarioAOCS
class ScenarioAOCS : public ScenarioObject
{
public:
    ScenarioAOCS();
    void load(const QDomElement& e);

    double totalAOCSMass() const
    { return m_totalAOCSMass; }
    void setTotalAOCSMass(double totalAOCSMass)
    { m_totalAOCSMass = totalAOCSMass; }

private:
    double m_totalAOCSMass;
};


// ScenarioOBDH
class ScenarioOBDH : public ScenarioObject
{
public:
    ScenarioOBDH();
    void load(const QDomElement& e);

    double totalOBDHMass() const
    { return m_totalOBDHMass; }
    void setTotalOBDHMass(double totalOBDHMass)
    { m_totalOBDHMass = totalOBDHMass; }

private:
    double m_totalOBDHMass;
};


// ScenarioSpaceScenario
class ScenarioSpaceScenario : public ScenarioObject
{
public:
    ScenarioSpaceScenario();
    void load(const QDomElement& e);

    const QList<QSharedPointer<ScenarioParticipantsType> >& AbstractParticipants() const
    { return m_AbstractParticipants; }
    QList<QSharedPointer<ScenarioParticipantsType> >& AbstractParticipants()
    { return m_AbstractParticipants; }
    void setAbstractParticipants(QList<QSharedPointer<ScenarioParticipantsType> > AbstractParticipants)
    { m_AbstractParticipants = AbstractParticipants; }

private:
    QList<QSharedPointer<ScenarioParticipantsType> > m_AbstractParticipants;
};

