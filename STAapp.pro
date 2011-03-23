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
# - Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) - #
#                                                                                    #
######################################################################################


CONFIG(release, debug|release):message(Release build)
CONFIG(debug, debug|release):message(Debug build)

#CONFIG += debug_and_release
#CONFIG += release
#CONFIG += warn_off

TEMPLATE = app

TARGET = STA

QT += opengl
QT += xml xmlpatterns
QT += network



# ## Specifiying folders on the tree #####
WINDOWS_LIBRARIES_DIR = thirdparty/windows
MACOSX_LIBRARIES_DIR = thirdparty/macosx
LINUX_LIBRARIES_DIR = thirdparty/linux
QWT3D_DIR = thirdparty/qwtplot3d
EIGEN_DIR = thirdparty/Eigen
QTVESTA_DIR = thirdparty/qtvesta

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
    sta-src/Main/OemImporter.cpp

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
    sta-src/Main/OemImporter.h

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
    sta-src/Astro-Core/nedTOfixed.cpp
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
    sta-src/Astro-Core/nedTOfixed.h
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
#RENDEZVOUS_SOURCES = sta-src/RendezVous/rendezvous.cpp
#RENDEZVOUS_HEADERS = sta-src/RendezVous/rendezvous.h
#RENDEZVOUS_FORMS = sta-src/RendezVous/rendezvous.ui

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
    sta-src/SEM/Launcher.cpp
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
    sta-src/SEM/Launcher.h
SEM_FORMS = sta-src/SEM/sem.ui \
    sta-src/SEM/ThermalGUI.ui \
    sta-src/SEM/StructureGUI.ui \
    sta-src/SEM/SEMWizard.ui \
    sta-src/SEM/SemMainGUI.ui \
    sta-src/SEM/PowerGUI.ui \
    sta-src/SEM/DataCommGUI.ui

# ############# 3-Body Module ##############
#LAGRANGIAN_SOURCES = sta-src/Lagrangian/trajectoryprinting.cpp \
#    sta-src/Lagrangian/halorbitcomputation.cpp \
#    sta-src/Lagrangian/lagrangian.cpp \
#    sta-src/Lagrangian/InizializeOptimizer.cpp \
#    sta-src/Lagrangian/EvaluateModel.cpp \
#    sta-src/Lagrangian/EarthMoonTransfer.cpp \
#    sta-src/Lagrangian/lagrangianAdvanced.cpp
#LAGRANGIAN_HEADERS = sta-src/Lagrangian/halorbitcomputation.h \
#    sta-src/Lagrangian/trajectoryprinting.h \
#    sta-src/Lagrangian/lagrangian.h \
#    sta-src/Lagrangian/EarthMoonTransfer.h \
#    sta-src/Lagrangian/lagrangianAdvanced.h
#LAGRANGIAN_FORMS = sta-src/Lagrangian/lagrangian.ui \
#    sta-src/Lagrangian/lagrangianAdvanced.ui

# ############# Optimization ##############
#OPTIMIZATION_SOURCES = sta-src/Optimization/pso1D.cpp \
#    sta-src/Optimization/nsga2.cpp \
#    sta-src/Optimization/dgmopso.cpp \
#    sta-src/Optimization/RunOptimizer.cpp \
#    sta-src/Optimization/arrays.cpp
#OPTIMIZATION_HEADERS = sta-src/Optimization/GlobalOptimizers.h \
#    sta-src/Optimization/arrays.h \
#    sta-src/Optimization/random.h \
#    sta-src/Optimization/input.h \
#    sta-src/Optimization/realinit.h \
#    sta-src/Optimization/init.h \
#    sta-src/Optimization/decode.h \
#    sta-src/Optimization/ranking.h \
#    sta-src/Optimization/rancon.h \
#    sta-src/Optimization/dfit.h \
#    sta-src/Optimization/select.h \
#    sta-src/Optimization/crossover.h \
#    sta-src/Optimization/uniformxr.h \
#    sta-src/Optimization/realcross2.h \
#    sta-src/Optimization/mut.h \
#    sta-src/Optimization/realmut1.h \
#    sta-src/Optimization/keepaliven.h

# ############# Visualization Module ##############
VISUALIZATION_SOURCES = \
    sta-src/Visualization/ClippedDraw.cpp \
    sta-src/Visualization/GroundTrack.cpp \
    sta-src/Visualization/GroundTrackPlotTool.cpp \
    sta-src/Visualization/GroundTrackView.cpp \
    sta-src/Visualization/ThreeDView.cpp \
    sta-src/Visualization/ThreeDVisualizationTool.cpp \
    sta-src/Visualization/VisualizationToolBar.cpp

VISUALIZATION_HEADERS = \
    sta-src/Visualization/ClippedDraw.h \
    sta-src/Visualization/GroundTrack.h \
    sta-src/Visualization/GroundTrackPlotTool.h \
    sta-src/Visualization/GroundTrackView.h \
    sta-src/Visualization/ThreeDView.h \
    sta-src/Visualization/ThreeDVisualizationTool.h \
    sta-src/Visualization/VisualizationToolBar.h

# ############# Plotting Module ##############
PLOT_SOURCES = \
    sta-src/Plotting/plottingtool.cpp \
    sta-src/Plotting/PlotLabelFormatter.cpp \
    sta-src/Plotting/PlotScale.cpp \
    sta-src/Plotting/PlotStyle.cpp \
    sta-src/Plotting/PlotView.cpp \
    sta-src/Plotting/PlotView3D.cpp \
    sta-src/Plotting/PlotGraphFromFile.cpp \
    sta-src/Plotting/TimePlotScale.cpp

PLOT_HEADERS = \
    sta-src/Plotting/plottingtool.h \
    sta-src/Plotting/PlotDataSource.h \
    sta-src/Plotting/PlotLabelFormatter.h \
    sta-src/Plotting/PlotScale.h \
    sta-src/Plotting/PlotStyle.h \
    sta-src/Plotting/PlotView.h \
    sta-src/Plotting/PlotView3D.h \
    sta-src/Plotting/PlotGraphFromFile.h \
    sta-src/Plotting/TimePlotScale.h

PLOT_FORMS = sta-src/Plotting/plottingtool.ui

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
ANALYSIS_SOURCES = sta-src/Analysis/analysis.cpp \
		   sta-src/Analysis/AnalysisPlot.cpp \
		   sta-src/Analysis/analysisTable.cpp
ANALYSIS_HEADERS = sta-src/Analysis/analysis.h\
		    sta-src/Analysis/AnalysisPlot.h \
		   sta-src/Analysis/analysisTable.h
ANALYSIS_FORMS = sta-src/Analysis/analysis.ui

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
    sta-src/Constellations/canalysis.cpp \
    sta-src/Constellations/analysisprop.cpp
CONSTELLATIONS_HEADERS = sta-src/Constellations/cwizard.h \
    sta-src/Constellations/constellationwizard.h \
    sta-src/Constellations/constellationmodule.h \
    sta-src/Constellations/discretization.h \
    sta-src/Constellations/canalysis.h \
    sta-src/Constellations/analysisprop.h
CONSTELLATIONS_FORMS = sta-src/Constellations/cwizard.ui \
    sta-src/Constellations/constellationwizard.ui \
    sta-src/Constellations/analysisprop.ui

# ############# Help Browser Module ##############
HELPBROWSER_SOURCES = sta-src/Help/HelpBrowser.cpp
HELPBROWSER_HEADERS = sta-src/Help/HelpBrowser.h
HELPBROWSER_FORMS = 

# ############# Maneuvers module ##############
MANEUVERS_SOURCES = sta-src/Maneuvers/deltaVDialog.cpp
MANEUVERS_HEADERS = sta-src/Maneuvers/deltaVDialog.h
MANEUVERS_FORMS = sta-src/Maneuvers/deltaVDialog.ui


# ########### GL extension wrangler ################
GLEW_SOURCES = thirdparty/glew/src/glew.c
GLEW_HEADERS = thirdparty/glew/include/GL/glew.h \
    thirdparty/glew/include/GL/glxew.h \
    thirdparty/glew/include/GL/wglew.h


# ############# QwtPlot3d main ####################
QWT3D_SOURCES += $$QWT3D_DIR/src/qwt3d_axis.cpp \
    $$QWT3D_DIR/src/qwt3d_color.cpp \
    $$QWT3D_DIR/src/qwt3d_coordsys.cpp \
    $$QWT3D_DIR/src/qwt3d_drawable.cpp \
    $$QWT3D_DIR/src/qwt3d_mousekeyboard.cpp \
    $$QWT3D_DIR/src/qwt3d_movements.cpp \
    $$QWT3D_DIR/src/qwt3d_lighting.cpp \
    $$QWT3D_DIR/src/qwt3d_colorlegend.cpp \
    $$QWT3D_DIR/src/qwt3d_plot.cpp \
    $$QWT3D_DIR/src/qwt3d_label.cpp \
    $$QWT3D_DIR/src/qwt3d_types.cpp \
    $$QWT3D_DIR/src/qwt3d_enrichment_std.cpp \
    $$QWT3D_DIR/src/qwt3d_autoscaler.cpp \
    $$QWT3D_DIR/src/qwt3d_io_reader.cpp \
    $$QWT3D_DIR/src/qwt3d_io.cpp \
    $$QWT3D_DIR/src/qwt3d_scale.cpp
QWT3D_SOURCES += $$QWT3D_DIR/src/qwt3d_gridmapping.cpp \
    $$QWT3D_DIR/src/qwt3d_parametricsurface.cpp \
    $$QWT3D_DIR/src/qwt3d_function.cpp
QWT3D_SOURCES += $$QWT3D_DIR/src/qwt3d_surfaceplot.cpp \
    $$QWT3D_DIR/src/qwt3d_gridplot.cpp \
    $$QWT3D_DIR/src/qwt3d_meshplot.cpp
QWT3D_HEADERS += $$QWT3D_DIR/include/qwt3d_color.h \
    $$QWT3D_DIR/include/qwt3d_global.h \
    $$QWT3D_DIR/include/qwt3d_types.h \
    $$QWT3D_DIR/include/qwt3d_axis.h \
    $$QWT3D_DIR/include/qwt3d_coordsys.h \
    $$QWT3D_DIR/include/qwt3d_drawable.h \
    $$QWT3D_DIR/include/qwt3d_helper.h \
    $$QWT3D_DIR/include/qwt3d_label.h \
    $$QWT3D_DIR/include/qwt3d_openglhelper.h \
    $$QWT3D_DIR/include/qwt3d_colorlegend.h \
    $$QWT3D_DIR/include/qwt3d_plot.h \
    $$QWT3D_DIR/include/qwt3d_enrichment.h \
    $$QWT3D_DIR/include/qwt3d_enrichment_std.h \
    $$QWT3D_DIR/include/qwt3d_autoscaler.h \
    $$QWT3D_DIR/include/qwt3d_autoptr.h \
    $$QWT3D_DIR/include/qwt3d_io.h \
    $$QWT3D_DIR/include/qwt3d_io_reader.h \
    $$QWT3D_DIR/include/qwt3d_scale.h \
    $$QWT3D_DIR/include/qwt3d_portability.h
QWT3D_HEADERS += $$QWT3D_DIR/include/qwt3d_mapping.h \
    $$QWT3D_DIR/include/qwt3d_gridmapping.h \
    $$QWT3D_DIR/include/qwt3d_parametricsurface.h \
    $$QWT3D_DIR/include/qwt3d_function.h
QWT3D_HEADERS += $$QWT3D_DIR/include/qwt3d_surfaceplot.h \
    $$QWT3D_DIR/include/qwt3d_volumeplot.h \
    $$QWT3D_DIR/include/qwt3d_graphplot.h \
    $$QWT3D_DIR/include/qwt3d_multiplot.h

# ############# QwtPlot3d gl2ps support ##############
QWT3D_HEADERS += $$QWT3D_DIR/3rdparty/gl2ps/gl2ps.h \
    $$QWT3D_DIR/include/qwt3d_io_gl2ps.h
QWT3D_SOURCES += $$QWT3D_DIR/src/qwt3d_io_gl2ps.cpp \
    $$QWT3D_DIR/3rdparty/gl2ps/gl2ps.c

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
#    $$RENDEZVOUS_SOURCES \
    $$RAM_SOURCES \
#    $$LAGRANGIAN_SOURCES \
#    $$OPTIMIZATION_SOURCES \
    $$CALCULATOR_SOURCES \
    $$GLEW_SOURCES \
    $$QWT3D_SOURCES \
    $$QTVESTA_SOURCES \
    $$NORAD_SOURCES \
    $$HELPBROWSER_SOURCES \
    $$PAYLOAD_SOURCES \
    $$ANALYSIS_SOURCES \
    $$COVERAGE_SOURCES \
    $$CONSTELLATIONS_SOURCES \
    $$MANEUVERS_SOURCES
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
#    $$RENDEZVOUS_HEADERS \
#    $$LAGRANGIAN_HEADERS \
#    $$OPTIMIZATION_HEADER \
    $$CALCULATOR_HEADERS \
    $$GLEW_HEADERS \
    $$QWT3D_HEADERS \
    $$QTVESTA_HEADERS \
    $$NORAD_HEADERS \
    $$HELPBROWSER_HEADERS \
    $$PAYLOAD_HEADERS \
    $$ANALYSIS_HEADERS \
    $$COVERAGE_HEADERS \
    $$CONSTELLATIONS_HEADERS \
    $$MANEUVERS_HEADERS
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
#    $$RENDEZVOUS_FORMS \
#    $$LAGRANGIAN_FORMS \
    $$GLEW_FORMS \
    $$QWT3D_FORMS \
    $$NORAD_FORMS \
    $$HELPBROWSER_FORMS \
    $$PAYLOAD_FORMS \
    $$ANALYSIS_FORMS \
    $$COVERAGE_FORMS \
    $$CONSTELLATIONS_FORMS \
    $$MANEUVERS_FORMS
RESOURCES = \
    iconary/sta-icons.qrc
UI_HEADERS_DIR = sta-src/ui/include
UI_SOURCES_DIR = sta-src/ui/src
INCLUDEPATH += .
INCLUDEPATH += sta-src
INCLUDEPATH += thirdparty
INCLUDEPATH += thirdparty/glew/include
INCLUDEPATH += thirdparty/qwtplot3d/include
INCLUDEPATH += thirdparty/noradtle

LIBS += -lvesta -Lthirdparty/vesta/build

# Add library path for static libraries
win32-g++:LIBS += $$PWD/lib/win32-x86-gcc/cspice.a

INCLUDEPATH += include/spice

DEFINES += EIGEN_USE_NEW_STDVECTOR

win32 { 
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
    # message("Warning: Disabling Eigen unaligned memory assertion")
    # DEFINES += EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
    message("Enabling Eigen in Windows")
    QMAKE_CXXFLAGS += -mincoming-stack-boundary=2
    QMAKE_CXXFLAGS_RELEASE = -ffast-math \
        -fexpensive-optimizations \
        -O3 \
        -Bdynamic

    # The below libraries are already included automatically by Qt, but they need to appear
    # *after* libvesta on the command line. This seems to work pretty well.
    LIBS += -lopengl32 -lglu32
}


linux-g++ { 
    # We use gcc newer versions to allow 64 bits compilations
    QMAKE_CXX     >= g++-4.3
    QMAKE_CC      >= gcc-4.3
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
    message("Warning: compiling a 64-bit linux version with >= gcc v4.3")
    LIBS += -L$$PWD/lib/linux-x64
    LIBS += -lcspice
    } else {
        # 32-bit Linux
    message("Warning: compiling a 32-bit linux version with >= gcc v4.3")
    LIBS += -L$$PWD/lib/linux-x86
    LIBS += -lcspice
    }
}

# ################## Package files ###################
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

# ############### MAC OS X bundle ########################
macx { 
     message( "Warning: building on MAC OS X for x86 architecture only" )
     QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
     CONFIG += x86

     # Guillermo: we use LLVM in MAC OS X instead of gcc
     QMAKE_CC = llvm-gcc
     QMAKE_CXX = llvm-g++

     QT += dbus

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

    QMAKE_LFLAGS += -framework CoreFoundation

    LIBS += $$PWD/lib/mac/cspice.a

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
        BODIES

    ## MAC OS X specifics to load inside the bundle the Qt frameworks to avoid separated installation
    ## and make STA a droppable and callable application, MAC alike
    QMAKE_INFO_PLIST= ./sta-data/macosx/Info.plist

    # Killing the annoying bug of Apple when telling:
    # ld: warning: directory '/tmp/qt-stuff-18929/source/qt-everywhere-opensource-src-4.7.0/lib' following -F not found
    QMAKE_PRL_LIBS = -framework Qt<ModuleDep>

	# Next command has been moved to the Qt Projects Build Steps list
	## Deploys Qt frameworks inside the MAC bundle but efficiently
    STAMACDEPLOY = $$system(macdeployqt ../STA-build-desktop/STA.app)
    message($$STAMACDEPLOY)

    # Application icon
    #ICON = iconary/STAlogo.icns
}

