######################################################################################
# This program is free software; you can redistribute it and/or modify it under      #
# the terms of the European Union Public Licence - EUPL v.1.1 as published by        #
# the European Commission.                                                           #
#                                                                                    #
# This program is distributed in the hope that it will be useful, but WITHOUT        #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS      #
# FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1       #
# for more details.                                                                  #
#                                                                                    #
# You should have received a copy of the European Union Public Licence - EUPL v.1.1  #
# along with this program.                                                           #
#                                                                                    #
# Further information about the European Union Public Licence - EUPL v.1.1 can       #
# also be found on the world wide web at http://ec.europa.eu/idabc/eupl              #
#                                                                                    #
#                                                                                    #
# - Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) - #
#                                                                                    #
######################################################################################

#CONFIG += debug_and_release
#CONFIG += release
#CONFIG += warn_off

#CONFIG(release, debug|release):message(Release build)
#CONFIG(debug, debug|release):message(Debug build)


TEMPLATE = app

TARGET = STA

QT += opengl
QT += xml xmlpatterns
QT += network
QT += qt3support
QT += svg

CONFIG += assistant
CONFIG += designer
CONFIG += qt warn_off thread




# ## Specifiying folders on the tree #####
WINDOWS_LIBRARIES_DIR = thirdparty/windows
MACOSX_LIBRARIES_DIR = thirdparty/macosx
LINUX_LIBRARIES_DIR = thirdparty/linux
QWT3D_DIR = thirdparty/qwtplot3d
EIGEN_DIR = thirdparty/Eigen
QTVESTA_DIR = thirdparty/qtvesta
QTIPLOT_DIR = thirdparty/qtiplot

# ################### Main files ################################
MAIN_SOURCES = \
    sta-src/Main/initialstateeditor.cpp \
    sta-src/Main/main.cpp \
    sta-src/Main/mainwindow.cpp \
    sta-src/Main/propagatedscenario.cpp \
    sta-src/Main/scenarioelementbox.cpp \
    sta-src/Main/scenariotree.cpp \
    sta-src/Main/scenarioview.cpp \
    sta-src/Main/timelineview.cpp \
    sta-src/Main/timelinewidget.cpp \
    sta-src/Main/aerodynamicproperties.cpp \
    sta-src/Main/physicalproperties.cpp \
    sta-src/Main/payloadproperties.cpp \
    sta-src/Main/propulsionproperties.cpp \
    sta-src/Main/initialstateThreebodyEditor.cpp \
    sta-src/Main/exportdialog.cpp \
    sta-src/Main/entrymass.cpp \
    sta-src/Main/about.cpp \
    sta-src/Main/preferences.cpp \
    sta-src/Main/ViewActionGroup.cpp \
    sta-src/Main/OemImporter.cpp \
    sta-src/Main/findDataFolder.cpp

MAIN_HEADERS = \
    sta-src/Main/initialstateeditor.h \
    sta-src/Main/main.h \
    sta-src/Main/mainwindow.h \
    sta-src/Main/propagatedscenario.h \
    sta-src/Main/scenarioelementbox.h \
    sta-src/Main/scenariotree.h \
    sta-src/Main/scenarioview.h \
    sta-src/Main/sheetDelegate.h \
    sta-src/Main/treeItemDelegate.h \
    sta-src/Main/timelineview.h \
    sta-src/Main/timelinewidget.h \
    sta-src/Main/aerodynamicproperties.h \
    sta-src/Main/physicalproperties.h \
    sta-src/Main/payloadproperties.h \
    sta-src/Main/propulsionproperties.h \
    sta-src/Main/initialstateThreebodyEditor.h \
    sta-src/Main/exportdialog.h \
    sta-src/Main/entrymass.h \
    sta-src/Main/about.h \
    sta-src/Main/preferences.h \
    sta-src/Main/ViewActionGroup.h \
    sta-src/Main/OemImporter.h \
    sta-src/Main/findDataFolder.h

MAIN_FORMS = sta-src/Main/initialstateeditor.ui \
    sta-src/Main/mainwindow.ui \
    sta-src/Main/scenarioelementbox.ui \
    sta-src/Main/scenarioview.ui \
    sta-src/Main/aerodynamicproperties.ui \
    sta-src/Main/physicalproperties.ui \
    sta-src/Main/payloadproperties.ui \
    sta-src/Main/propulsionproperties.ui \
    sta-src/Main/initialstateThreebodyEditor.ui \
    sta-src/Main/exportdialog.ui \
    sta-src/Main/entrymass.ui \
    sta-src/Main/about.ui \
    sta-src/Main/preferences.ui

# ##################### Astro Core ############################
ASTROCORE_SOURCES = sta-src/Astro-Core/calendarTOjulian.cpp \
    sta-src/Astro-Core/date.cpp \
    sta-src/Astro-Core/EarthRotationState.cpp \
    sta-src/Astro-Core/ephparms.cpp \
    sta-src/Astro-Core/getGreenwichHourAngle.cpp \
    sta-src/Astro-Core/GravityModel.cpp \
    sta-src/Astro-Core/inertialTOfixed.cpp \
    sta-src/Astro-Core/jplephemeris.cpp \
    sta-src/Astro-Core/SpiceEphemeris.cpp \
    sta-src/Astro-Core/stamath.cpp \
    sta-src/Astro-Core/nutate.cpp \
    sta-src/Astro-Core/orbitalTOcartesian.cpp \
    sta-src/Astro-Core/propagateJ2.cpp \
    sta-src/Astro-Core/propagateTWObody.cpp \
    sta-src/Astro-Core/propagateGAUSS.cpp \
    sta-src/Astro-Core/propagateENCKE.cpp \
    sta-src/Astro-Core/propagateCOWELL.cpp \
    sta-src/Astro-Core/perturbations.cpp \
    sta-src/Astro-Core/rectangularTOpolar.cpp \
    sta-src/Astro-Core/reduce.cpp \
    sta-src/Astro-Core/stabody.cpp \
    sta-src/Astro-Core/stacoordsys.cpp \
    sta-src/Astro-Core/UniformRotationState.cpp \
    sta-src/Astro-Core/cartesianTOorbital.cpp \
    sta-src/Astro-Core/EODE/arithmetics.cpp \
    sta-src/Astro-Core/EODE/eode.cpp \
    sta-src/Astro-Core/DayNumber.cpp \
    sta-src/Astro-Core/DayNumberToCalendar.cpp \
    sta-src/Astro-Core/TLEtoOrbitalElements.cpp \
    sta-src/Astro-Core/eci2lhlv.cpp \
    sta-src/Astro-Core/cartesianTOspherical.cpp \
    sta-src/Astro-Core/fixedTOinertial.cpp \
    sta-src/Astro-Core/trajectorypropagation.cpp \
    sta-src/Astro-Core/AngleConversion.cpp \
    sta-src/Astro-Core/Atmosphere/AtmosphereModel.cpp \
    sta-src/Astro-Core/sphericalTOcartesian.cpp \
    sta-src/Astro-Core/cartesianTOrotating.cpp \
    sta-src/Astro-Core/threebodyParametersComputation.cpp \
    sta-src/Astro-Core/ascendingNode.cpp \
#    sta-src/Astro-Core/propagateTHREEbody.cpp \
    sta-src/Astro-Core/rotatingTOcartesian.cpp \
    sta-src/Astro-Core/Interpolators.cpp \
    sta-src/Astro-Core/trueAnomalyTOmeanAnomaly.cpp \
    sta-src/Astro-Core/surfaceVelocity.cpp \
    sta-src/Astro-Core/EclipseDuration.cpp \
    sta-src/Astro-Core/bodyTOwind.cpp \
    sta-src/Astro-Core/nedTOfixed.cpp \
    sta-src/Astro-Core/calculateElements.cpp \
    sta-src/Astro-Core/attitudeintegration.cpp \
    sta-src/Astro-Core/attitudetransformations.cpp

ASTROCORE_HEADERS = sta-src/Astro-Core/calendarTOjulian.h \
    sta-src/Astro-Core/date.h \
    sta-src/Astro-Core/EarthRotationState.h \
    sta-src/Astro-Core/ephemeris.h \
    sta-src/Astro-Core/ephparms.h \
    sta-src/Astro-Core/getGreenwichHourAngle.h \
    sta-src/Astro-Core/GravityModel.h \
    sta-src/Astro-Core/inertialTOfixed.h \
    sta-src/Astro-Core/jplephemeris.h \
    sta-src/Astro-Core/SpiceEphemeris.h \
    sta-src/Astro-Core/nutate.h \
    sta-src/Astro-Core/orbitalTOcartesian.h \
    sta-src/Astro-Core/propagateJ2.h \
    sta-src/Astro-Core/propagateTWObody.h \
    sta-src/Astro-Core/propagateGAUSS.h \
    sta-src/Astro-Core/propagateENCKE.h \
    sta-src/Astro-Core/propagateCOWELL.h \
    sta-src/Astro-Core/perturbations.h \
    sta-src/Astro-Core/rectangularTOpolar.h \
    sta-src/Astro-Core/reduce.h \
    sta-src/Astro-Core/RotationState.h \
    sta-src/Astro-Core/stabody.h \
    sta-src/Astro-Core/stacoordsys.h \
    sta-src/Astro-Core/stamath.h \
    sta-src/Astro-Core/statevector.h \
    sta-src/Astro-Core/UniformRotationState.h \
    sta-src/Astro-Core/cartesianTOorbital.h \
    sta-src/Astro-Core/constants.h \
    sta-src/Astro-Core/EODE/arithmetics.h \
    sta-src/Astro-Core/EODE/eode.h \
    sta-src/Astro-Core/DayNumber.h \
    sta-src/Astro-Core/DayNumberToCalendar.h \
    sta-src/Astro-Core/TLEtoOrbitalElements.h \
    sta-src/Astro-Core/eci2lhlv.h \
    sta-src/Astro-Core/cartesianTOspherical.h \
    sta-src/Astro-Core/fixedTOinertial.h \
    sta-src/Astro-Core/trajectorypropagation.h \
    sta-src/Astro-Core/AngleConversion.h \
    sta-src/Astro-Core/Atmosphere/AtmosphereModel.h \
    sta-src/Astro-Core/sphericalTOcartesian.h \
    sta-src/Astro-Core/cartesianTOrotating.h \
    sta-src/Astro-Core/threebodyParametersComputation.h \
    sta-src/Astro-Core/ascendingNode.h \
#    sta-src/Astro-Core/propagateTHREEbody.h \
    sta-src/Astro-Core/rotatingTOcartesian.h \
    sta-src/Astro-Core/Interpolators.h \
    sta-src/Astro-Core/trueAnomalyTOmeanAnomaly.h \
    sta-src/Astro-Core/surfaceVelocity.h \
    sta-src/Astro-Core/EclipseDuration.h \
    sta-src/Astro-Core/bodyTOwind.h \
    sta-src/Astro-Core/nedTOfixed.h \
    sta-src/Astro-Core/calculateElements.h \
    sta-src/Astro-Core/attitudevector.h \
    sta-src/Astro-Core/attitudeintegration.h \
    sta-src/Astro-Core/attitudetransformations.h

ASTROCORE_FORMS = sta-src/Astro-Core/trajectorypropagation.ui

# ############# Entry module ##################
ENTRY_SOURCES = sta-src/Entry/reentry.cpp \
    sta-src/Entry/reentrystructures.cpp \
    sta-src/Entry/capsule.cpp \
    sta-src/Entry/BodyREM.cpp \
    sta-src/Entry/CelestialBody.cpp \
    sta-src/Entry/EntryTrajectory.cpp \
    sta-src/Entry/HeatRateClass.cpp
ENTRY_HEADERS = sta-src/Entry/reentry.h \
    sta-src/Entry/reentrystructures.h \
    sta-src/Entry/capsule.h \
    sta-src/Entry/BodyREM.h \
    sta-src/Entry/CelestialBody.h \
    sta-src/Entry/EntryTrajectory.h \
    sta-src/Entry/HeatRateClass.h
ENTRY_FORMS = sta-src/Entry/reentry.ui

# ############# Loitering Module ##############
LOITERING_SOURCES = sta-src/Loitering/loitering.cpp \
    sta-src/Loitering/loiteringTLE.cpp
LOITERING_HEADERS = sta-src/Loitering/loitering.h \
    sta-src/Loitering/loiteringTLE.h
LOITERING_FORMS = sta-src/Loitering/loitering.ui \
    sta-src/Loitering/loiteringTLE.ui

# ############# Calculator Module ##############
CALCULATOR_SOURCES = sta-src/Calculator/STAcalculator.cpp
CALCULATOR_HEADERS = sta-src/Calculator/STAcalculator.h
CALCULATOR_FORMS = sta-src/Calculator/STAcalculator.ui

# ############# External Trajectories Module ##############
EXTERNAL_SOURCES = sta-src/External/external.cpp
EXTERNAL_HEADERS = sta-src/External/external.h
EXTERNAL_FORMS = sta-src/External/external.ui

# ############## Locations Module ################
LOCATIONS_SOURCES = sta-src/Locations/locationeditor.cpp \
    sta-src/Locations/environmentdialog.cpp
LOCATIONS_HEADERS = sta-src/Locations/locationeditor.h \
    sta-src/Locations/environmentdialog.h
LOCATIONS_FORMS = sta-src/Locations/locationeditor.ui \
    sta-src/Locations/environmentDialog.ui

# ############# Rendezvous Module ##############
RENDEZVOUS_SOURCES = sta-src/RendezVous/rendezVousDialog.cpp \
    sta-src/RendezVous/rendezVousScheduledManoeuvresTable.cpp \
    sta-src/RendezVous/rendezVousXMLInterface.cpp \
    sta-src/RendezVous/rendezVousManoeuvres.cpp \
    sta-src/RendezVous/rendezVousManoeuvresTreeDrag.cpp
RENDEZVOUS_HEADERS = sta-src/RendezVous/rendezVousDialog.h \
    sta-src/RendezVous/rendezVousScheduledManoeuvresTable.h \
    sta-src/RendezVous/rendezVousXMLInterface.h \
    sta-src/RendezVous/rendezVousManoeuvres.h \
    sta-src/RendezVous/rendezVousConstants.h \
    sta-src/RendezVous/rendezVousManoeuvresTreeDrag.h
RENDEZVOUS_FORMS = sta-src/RendezVous/rendezVousDialog.ui


# ############# Coverage Module ##############
COVERAGE_SOURCES = sta-src/Coverage/commanalysis.cpp \
    sta-src/Coverage/coverageanalysis.cpp
COVERAGE_HEADERS = sta-src/Coverage/commanalysis.h \
    sta-src/Coverage/coverageanalysis.h

# ############# SEM Module ##############
SEM_SOURCES = sta-src/SEM/sem.cpp \
    sta-src/SEM/TTCSubsystem.cpp \
    sta-src/SEM/ThermalSubsystem.cpp \
    sta-src/SEM/thermalgui.cpp \
    sta-src/SEM/StructureSubsystem.cpp \
    sta-src/SEM/structuregui.cpp \
    sta-src/SEM/semwizard.cpp \
    sta-src/SEM/semmaingui.cpp \
    sta-src/SEM/SemMain.cpp \
    sta-src/SEM/PowerSubsystem.cpp \
    sta-src/SEM/powergui.cpp \
    sta-src/SEM/PayloadSubsystem.cpp \
    sta-src/SEM/OBDHSubsystem.cpp \
    sta-src/SEM/MissionDetails.cpp \
    sta-src/SEM/datacommgui.cpp \
    sta-src/SEM/Launcher.cpp \
    sta-src/SEM/platformDialog.cpp
SEM_HEADERS = sta-src/SEM/sem.h \
    sta-src/SEM/TTCSubsystem.h \
    sta-src/SEM/ThermalSubsystem.h \
    sta-src/SEM/thermalgui.h \
    sta-src/SEM/StructureSubsystem.h \
    sta-src/SEM/structuregui.h \
    sta-src/SEM/semwizard.h \
    sta-src/SEM/semmaingui.h \
    sta-src/SEM/SemMain.h \
    sta-src/SEM/PowerSubsystem.h \
    sta-src/SEM/powergui.h \
    sta-src/SEM/PayloadSubsystem.h \
    sta-src/SEM/OBDHSubsystem.h \
    sta-src/SEM/MissionDetails.h \
    sta-src/SEM/datacommgui.h \
    sta-src/SEM/Launcher.h \
    sta-src/SEM/platformDialog.h
SEM_FORMS = sta-src/SEM/sem.ui \
    sta-src/SEM/ThermalGUI.ui \
    sta-src/SEM/StructureGUI.ui \
    sta-src/SEM/SEMWizard.ui \
    sta-src/SEM/SemMainGUI.ui \
    sta-src/SEM/PowerGUI.ui \
    sta-src/SEM/DataCommGUI.ui \
    sta-src/SEM/platform.ui


# ############# Interplanetary Module ##############
INTERPLANETARY_SOURCES = sta-src/Interplanetary/interplanetaryDialog.cpp
INTERPLANETARY_HEADERS = sta-src/Interplanetary/interplanetaryDialog.h
INTERPLANETARY_FORMS = sta-src/Interplanetary/interplanetaryDialog.ui


# ############# 3-Body Module ##############
LAGRANGIAN_SOURCES = sta-src/Lagrangian/lagrangianDialog.cpp
LAGRANGIAN_HEADERS = sta-src/Lagrangian/lagrangianDialog.h
LAGRANGIAN_FORMS = sta-src/Lagrangian/lagrangianDialog.ui

# ############# Optimization ##############
#OPTIMIZATION_SOURCES = sta-src/Optimization/
#OPTIMIZATION_HEADERS = sta-src/Optimization/

# ############# Visualization Module ##############
VISUALIZATION_SOURCES = \
    sta-src/Visualization/ClippedDraw.cpp \
    sta-src/Visualization/FrameAxesVisualizer.cpp \
    sta-src/Visualization/GroundTrack.cpp \
    sta-src/Visualization/GroundTrackPlotTool.cpp \
    sta-src/Visualization/GroundTrackView.cpp \
    sta-src/Visualization/ThreeDView.cpp \
    sta-src/Visualization/ThreeDVisualizationTool.cpp \
    sta-src/Visualization/VisualizationToolBar.cpp

VISUALIZATION_HEADERS = \
    sta-src/Visualization/ClippedDraw.h \
    sta-src/Visualization/FrameAxesVisualizer.h \
    sta-src/Visualization/GroundTrack.h \
    sta-src/Visualization/GroundTrackPlotTool.h \
    sta-src/Visualization/GroundTrackView.h \
    sta-src/Visualization/ThreeDView.h \
    sta-src/Visualization/ThreeDVisualizationTool.h \
    sta-src/Visualization/VisualizationToolBar.h

# ################ RAM ############
RAM_SOURCES = sta-src/RAM/parametrization.cpp \
    sta-src/RAM/aerodynamicmethods.cpp \
    sta-src/RAM/partgeometry.cpp \
    sta-src/RAM/vehiclegeometry.cpp \
    sta-src/RAM/advancedselectionGUI.cpp \
    sta-src/RAM/aeroanalysis.cpp
RAM_HEADERS = sta-src/RAM/parametrization.h \
    sta-src/RAM/aerodynamicmethods.h \
    sta-src/RAM/partgeometry.h \
    sta-src/RAM/vehiclegeometry.h \
    sta-src/RAM/advancedselectionGUI.h \
    sta-src/RAM/aeroanalysis.h
RAM_FORMS = sta-src/RAM/parametrizedgeometry.ui \
    sta-src/RAM/advancedselectionGUI.ui \
    sta-src/RAM/aerodynamicmethods.ui

# ################ Analysis ############
ANALYSIS_SOURCES = \
           sta-src/Analysis/analysisParametersChoice.cpp \
           sta-src/Analysis/qtiplotmain.cpp
ANALYSIS_HEADERS = \
           sta-src/Analysis/analysisParametersChoice.h \
           sta-src/Analysis/qtiplotmain.h
ANALYSIS_FORMS = \
           sta-src/Analysis/analysisParametersChoice.ui

# ################ Payloads ############
PAYLOAD_SOURCES = sta-src/Payloads/transmitterPayloadDialog.cpp \
    sta-src/Payloads/receiverPayloadDialog.cpp \
    sta-src/Payloads/opticalPayloadDialog.cpp \
    sta-src/Payloads/radarPayloadDialog.cpp
PAYLOAD_HEADERS = sta-src/Payloads/transmitterPayloadDialog.h \
    sta-src/Payloads/receiverPayloadDialog.h \
    sta-src/Payloads/opticalPayloadDialog.h \
    sta-src/Payloads/radarPayloadDialog.h
PAYLOAD_FORMS = sta-src/Payloads/transmitterPayloadDialog.ui \
    sta-src/Payloads/receiverPayloadDialog.ui \
    sta-src/Payloads/opticalPayloadDialog.ui \
    sta-src/Payloads/radarPayloadDialog.ui

# ########################## Scenario  #################################
SCENARIO_SOURCES = sta-src/Scenario/staschema.cpp \
    sta-src/Scenario/stascenarioutil.cpp \
    sta-src/Scenario/scenario.cpp \
    sta-src/Scenario/scenarioPropagator.cpp \
    sta-src/Scenario/missionsDefaults.cpp \
    sta-src/Scenario/missionAspectDialog.cpp
SCENARIO_HEADERS = sta-src/Scenario/staschema.h \
    sta-src/Scenario/stascenarioutil.h \
    sta-src/Scenario/propagationfeedback.h \
    sta-src/Scenario/scenario.h \
    sta-src/Scenario/scenarioPropagator.h \
    sta-src/Scenario/missionsDefaults.h \
    sta-src/Scenario/missionAspectDialog.h
SCENARIO_FORMS = sta-src/Scenario/missionAspectDialog.ui


# ############# Constellations Module ##############
CONSTELLATIONS_SOURCES = sta-src/Constellations/cwizard.cpp \
    sta-src/Constellations/constellationwizard.cpp \
    sta-src/Constellations/constellationmodule.cpp \
    sta-src/Constellations/discretization.cpp \
    sta-src/Constellations/cstudy.cpp
CONSTELLATIONS_HEADERS = sta-src/Constellations/cwizard.h \
    sta-src/Constellations/constellationwizard.h \
    sta-src/Constellations/constellationmodule.h \
    sta-src/Constellations/discretization.h \
    sta-src/Constellations/cstudy.h
CONSTELLATIONS_FORMS = sta-src/Constellations/cwizard.ui \
    sta-src/Constellations/constellationwizard.ui

# ############# Help Browser Module ##############
HELPBROWSER_SOURCES = sta-src/Help/HelpBrowser.cpp
HELPBROWSER_HEADERS = sta-src/Help/HelpBrowser.h
HELPBROWSER_FORMS =

# ############# Maneuvers module ##############
MANEUVERS_SOURCES = \
                    sta-src/Maneuvers/deltaVDialog.cpp \
                    sta-src/Maneuvers/rendezVousManoeuvreDialogClass.cpp
MANEUVERS_HEADERS = \
                    sta-src/Maneuvers/deltaVDialog.h \
                    sta-src/Maneuvers/rendezVousManoeuvreDialogClass.h
MANEUVERS_FORMS =   \
                    sta-src/Maneuvers/deltaVDialog.ui \
                    sta-src/Maneuvers/rendezVousManoeuvreDialog.ui

# ############# Services module ###################
SERVICES_SOURCES = sta-src/Services/serviceTimeParameter.cpp \
                   sta-src/Services/serviceTimeUnit.cpp \
                   sta-src/Services/stoppingConditionsTrajectoryDialog.cpp \
                   sta-src/Services/perturbationForcesDialog.cpp \
                   sta-src/Services/perturbationTorquesDialog.cpp \
                   sta-src/Services/serviceAngleUnit.cpp \
                   sta-src/Services/serviceDistanceUnit.cpp \
                   sta-src/Services/serviceDistanceRateUnit.cpp \
                   sta-src/Services/serviceAngleRateUnit.cpp \
                   sta-src/Services/serviceEpoch.cpp \
                   sta-src/Services/serviceGenericParameter.cpp
SERVICES_HEADERS = sta-src/Services/serviceTimeParameter.h \
                   sta-src/Services/serviceTimeUnit.h \
                   sta-src/Services/stoppingConditionsTrajectoryDialog.h \
                   sta-src/Services/perturbationForcesDialog.h \
                   sta-src/Services/perturbationTorquesDialog.h \
                   sta-src/Services/serviceAngleUnit.h \
                   sta-src/Services/serviceDistanceUnit.h \
                   sta-src/Services/serviceDistanceRateUnit.h \
                   sta-src/Services/serviceAngleRateUnit.h \
                   sta-src/Services/serviceEpoch.h \
                   sta-src/Services/serviceGenericParameter.h
SERVICES_FORMS =   sta-src/Services/serviceTimeParameter.ui \
                   sta-src/Services/serviceTimeUnit.ui \
                   sta-src/Services/stoppingConditionsTrajectory.ui \
                   sta-src/Services/perturbationForces.ui \
                   sta-src/Services/perturbationTorques.ui \
                   sta-src/Services/serviceAngleUnit.ui \
                   sta-src/Services/serviceDistanceUnit.ui \
                   sta-src/Services/serviceDistanceRateUnit.ui \
                   sta-src/Services/serviceAngleRateUnit.ui \
                   sta-src/Services/serviceEpoch.ui \
                   sta-src/Services/serviceGenericParameter.ui

# ########### GL extension wrangler ################
GLEW_SOURCES = thirdparty/glew/src/glew.c
GLEW_HEADERS = thirdparty/glew/include/GL/glew.h \
    thirdparty/glew/include/GL/glxew.h \
    thirdparty/glew/include/GL/wglew.h

# ############# TLEs support ##############
NORAD_HEADERS += thirdparty/noradtle/norad.h
NORAD_SOURCES += thirdparty/noradtle/basics.cpp \
    thirdparty/noradtle/common.cpp \
    thirdparty/noradtle/deep.cpp \
    thirdparty/noradtle/get_el.cpp \
    thirdparty/noradtle/sdp4.cpp \
    thirdparty/noradtle/sdp8.cpp \
    thirdparty/noradtle/sgp.cpp \
    thirdparty/noradtle/sgp4.cpp \
    thirdparty/noradtle/sgp8.cpp

# ############# VESTA ##############

QTVESTA_SOURCES = \
    $$QTVESTA_DIR/LocalImageLoader.cpp \
    $$QTVESTA_DIR/NetworkTextureLoader.cpp \
    $$QTVESTA_DIR/WMSRequester.cpp \
    $$QTVESTA_DIR/WMSTiledMap.cpp \
    $$QTVESTA_DIR/MultiWMSTiledMap.cpp

QTVESTA_HEADERS = \
    $$QTVESTA_DIR/LocalImageLoader.h \
    $$QTVESTA_DIR/NetworkTextureLoader.h \
    $$QTVESTA_DIR/WMSRequester.h \
    $$QTVESTA_DIR/WMSTiledMap.h \
    $$QTVESTA_DIR/MultiWMSTiledMap.h

DEFINES += GLEW_STATIC

# ############### All app sources, headers, and forms ############
SOURCES = $$MAIN_SOURCES \
    $$ASTROCORE_SOURCES \
    $$SEM_SOURCES \
    $$VISUALIZATION_SOURCES \
    $$PLOT_SOURCES \
    $$SCENARIO_SOURCES \
    $$ENTRY_SOURCES \
    $$LOCATIONS_SOURCES \
    $$LOITERING_SOURCES \
    $$EXTERNAL_SOURCES \
    $$RENDEZVOUS_SOURCES \
    $$RAM_SOURCES \
    $$LAGRANGIAN_SOURCES \
    $$INTERPLANETARY_SOURCES \
#    $$OPTIMIZATION_SOURCES \
    $$CALCULATOR_SOURCES \
    $$GLEW_SOURCES \
    $$QTVESTA_SOURCES \
    $$NORAD_SOURCES \
    $$HELPBROWSER_SOURCES \
    $$PAYLOAD_SOURCES \
    $$ANALYSIS_SOURCES \
    $$COVERAGE_SOURCES \
    $$CONSTELLATIONS_SOURCES \
    $$MANEUVERS_SOURCES \
    $$SERVICES_SOURCES
HEADERS = $$MAIN_HEADERS \
    $$ASTROCORE_HEADERS \
    $$SEM_HEADERS \
    $$VISUALIZATION_HEADERS \
    $$PLOT_HEADERS \
    $$SCENARIO_HEADERS \
    $$ENTRY_HEADERS \
    $$LOCATIONS_HEADERS \
    $$LOITERING_HEADERS \
    $$EXTERNAL_HEADERS \
    $$RAM_HEADERS \
    $$RENDEZVOUS_HEADERS \
    $$LAGRANGIAN_HEADERS \
    $$INTERPLANETARY_HEADERS \
#    $$OPTIMIZATION_HEADER \
    $$CALCULATOR_HEADERS \
    $$GLEW_HEADERS \
    $$QTVESTA_HEADERS \
    $$NORAD_HEADERS \
    $$HELPBROWSER_HEADERS \
    $$PAYLOAD_HEADERS \
    $$ANALYSIS_HEADERS \
    $$COVERAGE_HEADERS \
    $$CONSTELLATIONS_HEADERS \
    $$MANEUVERS_HEADERS \
    $$SERVICES_HEADERS
FORMS = $$MAIN_FORMS \
    $$ASTROCORE_FORMS \
    $$SEM_FORMS \
    $$PLOT_FORMS \
    $$SCENARIO_FORMS \
    $$RAM_FORMS \
    $$ENTRY_FORMS \
    $$LOCATIONS_FORMS \
    $$CALCULATOR_FORMS \
    $$LOITERING_FORMS \
    $$EXTERNAL_FORMS \
    $$RENDEZVOUS_FORMS \
    $$LAGRANGIAN_FORMS \
    $$INTERPLANETARY_FORMS \
    $$GLEW_FORMS \
    $$NORAD_FORMS \
    $$HELPBROWSER_FORMS \
    $$PAYLOAD_FORMS \
    $$ANALYSIS_FORMS \
    $$COVERAGE_FORMS \
    $$CONSTELLATIONS_FORMS \
    $$MANEUVERS_FORMS \
    $$SERVICES_FORMS

RESOURCES = \
    iconary/sta-icons.qrc

UI_HEADERS_DIR = sta-src/ui/include
UI_SOURCES_DIR = sta-src/ui/src

INCLUDEPATH += .
INCLUDEPATH += sta-src
INCLUDEPATH += include/spice
INCLUDEPATH += thirdparty
INCLUDEPATH += thirdparty/glew/include
INCLUDEPATH += thirdparty/qwtplot3d/include
INCLUDEPATH += thirdparty/noradtle
INCLUDEPATH += thirdparty/qtiplot
INCLUDEPATH += thirdparty/qtiplot/qtiplot/src
INCLUDEPATH += thirdparty/qtiplot/3rdparty/qwt/src
INCLUDEPATH += thirdparty/qtiplot/3rdparty/liborigin
INCLUDEPATH += thirdparty/qtiplot/3rdparty/gsl
INCLUDEPATH += thirdparty/qtiplot/3rdparty/gsl/include
INCLUDEPATH += thirdparty/qtiplot/3rdparty/muparser/include

LIBS += -lvesta -Lthirdparty/vesta/build

DEFINES += EIGEN_USE_NEW_STDVECTOR

########################################## Windows ######################################################
win32 {
    # Removing teh SSL support to allow proper compilation in Windows
    DEFINES += QT_NO_SSL

    # SPICE support
    # SOURCES += $$SPICE_SOURCES
    # HEADERS += $$SPICE_HEADERS
    # DEFINES += USE_SPICE
    # INCLUDEPATH += windows/inc/spice

    RC_FILE = sta-src/sta.rc
    DEFINES += _CRT_SECURE_NO_WARNINGS

    # Disable the regrettable min and max macros in windows.h
    DEFINES += NOMINMAX

    # Work around alignment problems with MinGW. Fixed-size Eigen matrices
    # are sometimes allocated on the stack at unaligned addresses even though
    # __alignof e.g. Vector4d is 16. Until we figure out what's going on, we'll
    # just disable the assertions. Since Eigen never generates SSE2 instructions
    # for the g++ 3.4 compiler in MinGW, this should be harmless.
    message("Warning: Disabling Eigen unaligned memory assertion and vectorization")
    DEFINES += EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
    DEFINES += EIGEN_DONT_VECTORIZE
    #message("Enabling Eigen in Windows")
    QMAKE_CXXFLAGS += -mincoming-stack-boundary=2
    QMAKE_CXXFLAGS_RELEASE = -ffast-math \
        -fexpensive-optimizations \
        -O3 \
        -Bdynamic

    #DEFINES += QT_DLL QT_THREAD_SUPPORT

    LIBS += $$PWD/lib/win32-x86-gcc/cspice.a

    # Case of dynamic libs
    LIBS += $$PWD/lib/win32-x86-gcc/qtiplot.dll
    LIBS += $$PWD/lib/win32-x86-gcc/muparser.dll
    LIBS += $$PWD/lib/win32-x86-gcc/qwtplot3d.dll
    #LIBS += $$PWD/lib/win32-x86-gcc/qwtmathml5.dll
    LIBS += $$PWD/lib/win32-x86-gcc/qwt5.dll
    LIBS += $$PWD/lib/win32-x86-gcc/libgsl.dll
    LIBS += $$PWD/lib/win32-x86-gcc/libgslcblas.dll
    LIBS += $$PWD/lib/win32-x86-gcc/zlib1.dll
    #LIBS += $$PWD/lib/win32-x86-gcc/msvcrt.dll
    LIBS += $$PWD/lib/win32-x86-gcc/QtAssistantClient4.dll


    # The below libraries are already included automatically by Qt, but they need to appear
    # *after* libvesta on the command line. This seems to work pretty well.
    LIBS += -lopengl32 -lglu32

    # Copy DLLs to target path
    WIN_DLL_PATH = $$PWD/lib/win32-x86-gcc
    EXTRA_BINFILES += \
        $$WIN_DLL_PATH/qtiplot.dll \
        $$WIN_DLL_PATH/muparser.dll \
        $$WIN_DLL_PATH/qwtplot3d.dll \
        $$WIN_DLL_PATH/qwt5.dll \
        $$WIN_DLL_PATH/libgsl.dll \
        $$WIN_DLL_PATH/libgslcblas.dll \
        $$WIN_DLL_PATH/zlib1.dll \
        $$WIN_DLL_PATH/QtAssistantClient4.dll

    CONFIG(debug, debug|release) {
        DESTDIR_WIN = $$OUT_PWD/debug
    } else {
        DESTDIR_WIN = $$OUT_PWD/release
    }

    # Convert forward slashes to backslashes for Windows shell commands
    EXTRA_BINFILES ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,EXTRA_BINFILES) {
        QMAKE_POST_LINK +=$$quote(copy /y $${FILE} $${DESTDIR_WIN}$$escape_expand(\\n\\t))
    }
}





############################################# Linux #######################################################
linux-g++ {
    QT += dbus

    QMAKE_CXXFLAGS_RELEASE = -ffast-math \
        -fexpensive-optimizations \
        -O3 \
        -Bdynamic 
    INCLUDEPATH += $$LINUX_LIBRARIES_DIR
    INCLUDEPATH += /usr/include
    CXXFLAGS += -std=c++0x

    HARDWARE_PLATFORM = $$system(uname -a)
    contains( HARDWARE_PLATFORM, x86_64 ) {
        # 64-bit Linux
    message("Warning: compiling a 64-bit linux version")
    LIBS += -L$$PWD/lib/linux-x64
    LIBS += -lcspice
    LIBS += -lgsl
    LIBS += -lgslcblas
    LIBS += $$PWD/thirdparty/qtiplot/libqtiplot.so.1.0.0
    LIBS += $$PWD/thirdparty/qtiplot/3rdparty/qwt/lib/libqwt.so.5.2.2
    LIBS += $$PWD/thirdparty/qtiplot/3rdparty/qwtplot3d/lib/libqwtplot3d.so.1.0.0
    LIBS += -lGLU
    } else {
        # 32-bit Linux
    message("Warning: compiling a 32-bit linux version")
    LIBS += -L$$PWD/lib/linux-x86
    LIBS += -lcspice
    LIBS += -lgsl
    LIBS += -lgslcblas
    LIBS += $$PWD/lib/linux-x86/libqwt.so.5.2.2
    LIBS += $$PWD/lib/linux-x86/libqwtplot3d.so.1.0.0
    LIBS += $$PWD/lib/linux-x86/libqtiplot.so.1.0.0
    LIBS += -lGLU
    }
}



# ######################################## MAC OS X bundle ##################################################
VIS3D_SOURCE = sta-data/vis3d
VIS3D_FILES =
SEMMISCELANEOUS_SOURCE = sta-data/data
SEMMISCELANEOUS_FILES = $$SEMMISCELANEOUS_SOURCE/EclipseDetailedReport.stad \
    $$SEMMISCELANEOUS_SOURCE/EclipseStarLight.stad \
    $$SEMMISCELANEOUS_SOURCE/PlanetThermalProperties.stad
EPHEMERIS_SOURCE = sta-data/ephemerides
EPHEMERIS_FILES = sta-data/ephemerides/de406_1800-2100.dat
TEXTURE_SOURCE = sta-data/textures/medres
TEXTURE_FILES =
MODEL_SOURCE = sta-data/models
MODEL_FILES =
ESTIMATIONS_SOURCE = sta-data/data/Estimations
ESTIMATIONS_FILES =
MATERIALS_SOURCE = sta-data/data/MaterialDatabase
MATERIALS_FILES =
SEMREPORTS_SOURCE = sta-data/data/SystemsEngineeringReports
SEMREPORTS_FILES =
HEATRATE_SOURCE = sta-data/data/heatrates
HEATRATE_FILES =
AERO_SOURCE = sta-data/data/aerodynamics
AERO_FILES =
RAMOUTPUT_SOURCE = sta-data/data/ramoutput
RAMOUTPUT_FILES =
VEHICLEWGS_SOURCE = sta-data/data/vehiclewgs
VEHICLEWGS_FILES =
ATMOSPHERES_SOURCE = sta-data/data/atmospheres
ATMOSPHERES_FILES =
BODIES_SOURCE = sta-data/data/bodies
BODIES_FILES =
SCHEMA_SOURCE = sta-data/schema/spacescenario/2.0
SCHEMA_FILES =
TLEs_SOURCE = sta-data/TLEs
TLEs_FILES =
MACOSXIconFiles_SOURCE = iconary
MACOSXIconFiles_FILES =
USERMANUAL_SOURCE = sta-data/help
USERMANUAL_FILES =
MAPS_SOURCE = sta-data/data/maps
MAPS_FILES =


macx {
     message( "Warning: building on MAC OS X for x86 architecture only" )
     QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk

     #HARDWARE_PLATFORM = $$system(uname -a)
     #contains( HARDWARE_PLATFORM, x86_64 ) {
     #   # 64-bit Linux
     #message("Warning: compiling a 64-bit binary in MAC OS X")
     #CONFIG += x86_64
     #} else {
     #   # 32-bit Linux
     #message("Warning: compiling a 32-bit binary in MAC OS X")
     #CONFIG += x86
     #}

     CONFIG += x86

     # Guillermo: We need DBus for the report part of the ANALYSIS module only on MAC and Linux
     QT += dbus

     # Guillermo: we use LLVM in MAC OS X instead of gcc (2X fast compilation)
     QMAKE_CC = llvm-gcc -arch i386
     QMAKE_CXX = llvm-g++ -arch i386

     QMAKE_CFLAGS_RELEASE = -ffast-math \
    -fexpensive-optimizations \
    -O3 \
    -Bdynamic \
    -ftree-vectorize

     QMAKE_CXXFLAGS_RELEASE = -ffast-math \
    -fexpensive-optimizations \
    -O3 \
    -Bdynamic \
    -ftree-vectorize

    LIBS += $$PWD/lib/mac/cspice.a
    LIBS += $$PWD/lib/mac/libmuparser.a
    LIBS += $$PWD/lib/mac/libgslcblas.a
    LIBS += $$PWD/lib/mac/libgsl.a
    LIBS += $$PWD/lib/mac/libz.a
    LIBS += $$PWD/lib/mac/libqwt.a
    LIBS += $$PWD/lib/mac/libqwtplot3d.a
    LIBS += $$PWD/lib/mac/libqtiplot.a

    QMAKE_LFLAGS += -framework CoreFoundation

    # Scan directories for files for Mac bundle
    FILES = $$system(ls $$VIS3D_SOURCE)
    VIS3D_FILES = $$join(FILES, " $$VIS3D_SOURCE/", $$VIS3D_SOURCE/)
    FILES = $$system(ls $$TEXTURE_SOURCE)
    TEXTURE_FILES = $$join(FILES, " $$TEXTURE_SOURCE/", $$TEXTURE_SOURCE/)
    FILES = $$system(ls $$MODEL_SOURCE)
    MODEL_FILES = $$join(FILES, " $$MODEL_SOURCE/", $$MODEL_SOURCE/)
    FILES = $$system(ls $$ESTIMATIONS_SOURCE)
    ESTIMATIONS_FILES = $$join(FILES, " $$ESTIMATIONS_SOURCE/", $$ESTIMATIONS_SOURCE/)
    FILES = $$system(ls $$MATERIALS_SOURCE)
    MATERIALS_FILES = $$join(FILES, " $$MATERIALS_SOURCE/", $$MATERIALS_SOURCE/)
    FILES = $$system(ls $$SEMREPORTS_SOURCE)
    SEMREPORTS_FILES = $$join(FILES, " $$SEMREPORTS_SOURCE/", $$SEMREPORTS_SOURCE/)
    FILES = $$system(ls $$HEATRATE_SOURCE)
    HEATRATE_FILES = $$join(FILES, " $$HEATRATE_SOURCE/", $$HEATRATE_SOURCE/)
    FILES = $$system(ls $$AERO_SOURCE)
    AERO_FILES = $$join(FILES, " $$AERO_SOURCE/", $$AERO_SOURCE/)
    FILES = $$system(ls $$VEHICLEWGS_SOURCE)
    VEHICLEWGS_FILES = $$join(FILES, " $$VEHICLEWGS_SOURCE/", $$VEHICLEWGS_SOURCE/)
    FILES = $$system(ls $$RAMOUTPUT_SOURCE)
    RAMOUTPUT_FILES = $$join(FILES, " $$RAMOUTPUT_SOURCE/", $$RAMOUTPUT_SOURCE/)
    FILES = $$system(ls $$ATMOSPHERES_SOURCE)
    ATMOSPHERES_FILES = $$join(FILES, " $$ATMOSPHERES_SOURCE/", $$ATMOSPHERES_SOURCE/)
    FILES = $$system(ls $$BODIES_SOURCE)
    BODIES_FILES = $$join(FILES, " $$BODIES_SOURCE/", $$BODIES_SOURCE/)
    FILES = $$system(ls $$SCHEMA_SOURCE)
    SCHEMA_FILES = $$join(FILES, " $$SCHEMA_SOURCE/", $$SCHEMA_SOURCE/)
    FILES = $$system(ls $$TLEs_SOURCE)
    TLEs_FILES = $$join(FILES, " $$TLEs_SOURCE/", $$TLEs_SOURCE/)
    FILES = $$system(ls $$USERMANUAL_SOURCE)
    USERMANUAL_FILES = $$join(FILES, " $$USERMANUAL_SOURCE/", $$USERMANUAL_SOURCE/)
    FILES = $$system(ls $$MACOSXIconFiles_SOURCE)
    MACOSXIconFiles_FILES = $$join(FILES, " $$MACOSXIconFiles_SOURCE/", $$MACOSXIconFiles_SOURCE/)
    FILES = $$system(ls $$MAPS_SOURCE)
    MAPS_FILES = $$join(FILES, " $$MAPS_SOURCE/", $$MAPS_SOURCE/)

    INCLUDEPATH += $$MACOSX_LIBRARIES_DIR
    FRAMEWORKS.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += FRAMEWORKS
    VIS3D.path = Contents/Resources/STAResources/vis3d
    VIS3D.files = $$VIS3D_FILES
    CATALOGS.path = Contents/Resources/STAResources/data
    CATALOGS.files = $$CATALOG_FILES
    TEXTURES.path = Contents/Resources/STAResources/textures/medres
    TEXTURES.files = $$TEXTURE_FILES
    MODELS.path = Contents/Resources/STAResources/models
    MODELS.files = $$MODEL_FILES
    EPHEMERIDES.path = Contents/Resources/STAResources/ephemerides
    EPHEMERIDES.files = $$EPHEMERIS_FILES
    ESTIMATIONS.path = Contents/Resources/STAResources/data/Estimations
    ESTIMATIONS.files = $$ESTIMATIONS_FILES
    MATERIALS.path = Contents/Resources/STAResources/data/MaterialDatabase
    MATERIALS.files = $$MATERIALS_FILES
    SEMREPORTS.path = Contents/Resources/STAResources/data/SystemsEngineeringReports
    SEMREPORTS.files = $$SEMREPORTS_FILES
    HEATRATES.path = Contents/Resources/STAResources/data/heatrates
    HEATRATES.files = $$HEATRATE_FILES
    AERODYNAMICS.path = Contents/Resources/STAResources/data/aerodynamics
    AERODYNAMICS.files = $$AERO_FILES
    RAMOUTPUT.path = Contents/Resources/STAResources/data/ramoutput
    RAMOUTPUT.files = $$RAMOUTPUT_FILES
    VEHICLEWGS.path = Contents/Resources/STAResources/data/vehiclewgs
    VEHICLEWGS.files = $$VEHICLEWGS_FILES
    ATMOSPHERES.path = Contents/Resources/STAResources/data/atmospheres
    ATMOSPHERES.files = $$ATMOSPHERES_FILES
    SCHEMA.path = Contents/Resources/STAResources/schema/spacescenario/2.0
    SCHEMA.files = $$SCHEMA_FILES
    BODIES.path = Contents/Resources/STAResources/data/bodies
    BODIES.files = $$BODIES_FILES
    TLEs.path = Contents/Resources/STAResources/TLEs
    TLEs.files = $$TLEs_FILES
    SEMMISCELANEOUS.path = Contents/Resources/STAResources/data
    SEMMISCELANEOUS.files = $$SEMMISCELANEOUS_FILES
    USERMANUAL.path = Contents/Resources/STAResources/help
    USERMANUAL.files = $$USERMANUAL_FILES
    MACOSXIconFiles.path = Contents/Resources
    MACOSXIconFiles.files = $$MACOSXIconFiles_FILES
    MAPS.path = Contents/Resources/STAResources/data/maps
    MAPS.files = $$MAPS_FILES

    QMAKE_BUNDLE_DATA += \
        MACOSXIconFiles \
        VIS3D \
        CATALOGS \
        TEXTURES \
        MODELS \
        EPHEMERIDES \
        ESTIMATIONS \
        MATERIALS \
        SEMREPORTS \
        TLEs \
        SCHEMA \
        SEMMISCELANEOUS \
        USERMANUAL \
        HEATRATES \
        AERODYNAMICS \
        RAMOUTPUT \
        VEHICLEWGS \
        ATMOSPHERES \
        BODIES \
        MAPS

    ## MAC OS X specifics to load inside the bundle the Qt frameworks to avoid separated installation
    ## and make STA a droppable and callable application, MAC alike
    QMAKE_INFO_PLIST= ./sta-data/macosx/Info.plist

    # Killing the annoying bug of Apple when telling:
    # ld: warning: directory '/tmp/qt-stuff-18929/source/qt-everywhere-opensource-src-4.7.0/lib' following -F not found
    #QMAKE_PRL_LIBS = -framework Qt<ModuleDep>

        # Next command has been moved to the Qt Projects Build Steps list
        ## Deploys Qt frameworks inside the MAC bundle but efficiently
    #STAMACDEPLOY = $$system(macdeployqt ../STA-build-desktop/STA.app)
    #message($$STAMACDEPLOY)

    # Application icon
    #ICON = iconary/STAlogo.icns
}
