message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
#target.path =
#INSTALLS += target

CONFIG += debug_and_release
CONFIG += release

TEMPLATE = app
TARGET = STA
QT += opengl
QT += xml

### Specifiying folders on the tree #####
WINDOWS_LIBRARIES_DIR = thirdparty/windows
MACOSX_LIBRARIES_DIR = thirdparty/macosx
LINUX_LIBRARIES_DIR = thirdparty/linux
CEL = thirdparty/celestia-src
QWT3D_DIR = thirdparty/qwtplot3d
CATALOG_SOURCE = sta-data/data
EIGEN_DIR = thirdparty/Eigen


#################### Main files ################################
MAIN_SOURCES = sta-src/Main/celestiainterface.cpp \
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
    sta-src/Main/about.cpp
MAIN_HEADERS = sta-src/Main/celestiainterface.h \
    sta-src/Main/initialstateeditor.h \
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
    sta-src/Main/about.h
MAIN_FORMS = sta-src/Main/initialstateeditor.ui \
    sta-src/Main/mainwindow.ui \
    sta-src/Main/scenarioelementbox.ui \
    sta-src/Main/scenarioview.ui \
    sta-src/Main/aerodynamicproperties.ui \
    sta-src/Main/physicalproperties.ui \
    sta-src/Main/payloadproperties.ui \
    sta-src/Main/propulsionproperties.ui \
    sta-src/Main/initialstateThreebodyEditor.ui \
    sta-src/Main/exportdialog.ui\
    sta-src/Main/about.ui

###################### Astro Core ############################
ASTROCORE_SOURCES = \
    sta-src/Astro-Core/calendarTOjulian.cpp \
    sta-src/Astro-Core/date.cpp \
    sta-src/Astro-Core/EarthRotationState.cpp \
    sta-src/Astro-Core/ephparms.cpp \
    sta-src/Astro-Core/getGreenwichHourAngle.cpp \
    sta-src/Astro-Core/GravityModel.cpp \
    sta-src/Astro-Core/inertialTOfixed.cpp \
    sta-src/Astro-Core/jplephemeris.cpp \
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
    sta-src/Astro-Core/propagateTHREEbody.cpp \
    sta-src/Astro-Core/rotatingTOcartesian.cpp \
    sta-src/Astro-Core/Interpolators.cpp \
    sta-src/Astro-Core/trueAnomalyTOmeanAnomaly.cpp
ASTROCORE_HEADERS = \
    sta-src/Astro-Core/calendarTOjulian.h \
    sta-src/Astro-Core/date.h \
    sta-src/Astro-Core/EarthRotationState.h \
    sta-src/Astro-Core/ephemeris.h \
    sta-src/Astro-Core/ephparms.h \
    sta-src/Astro-Core/getGreenwichHourAngle.h \
    sta-src/Astro-Core/GravityModel.h \
    sta-src/Astro-Core/inertialTOfixed.h \
    sta-src/Astro-Core/jplephemeris.h \
    sta-src/Astro-Core/nutate.h \
    sta-src/Astro-Core/orbitalTOcartesian.h \
    sta-src/Astro-Core/propagateJ2.h \
    sta-src/Astro-Core/propagateTWObody.h \
    sta-src/Astro-Core/propagateGAUSS.h \
    sta-src/Astro-Core/propagateENCKE.h \
    sta-src/Astro-Core/propagateCOWELL.h \
    sta-src/Astro-Core/perturbations.h \
    sta-src/Astro-Core/rectangularTOpolar.h \
    sta-src/Astro-Core/reduce.cpp \
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
    sta-src/Astro-Core/propagateTHREEbody.h \
    sta-src/Astro-Core/rotatingTOcartesian.h \
    sta-src/Astro-Core/Interpolators.h \
    sta-src/Astro-Core/trueAnomalyTOmeanAnomaly.h
ASTROCORE_FORMS = sta-src/Astro-Core/trajectorypropagation.ui

############## Entry module ##################
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
LOCATIONS_SOURCES = sta-src/Locations/locationeditor.cpp
LOCATIONS_HEADERS = sta-src/Locations/locationeditor.h
LOCATIONS_FORMS = sta-src/Locations/locationeditor.ui

# ############# Rendezvous Module ##############
RENDEZVOUS_SOURCES = sta-src/RendezVous/rendezvous.cpp
RENDEZVOUS_HEADERS = sta-src/RendezVous/rendezvous.h
RENDEZVOUS_FORMS = sta-src/RendezVous/rendezvous.ui

# ############# Coverage Module ##############
COVERAGE_SOURCES = sta-src/Coverage/coverage.cpp
COVERAGE_HEADERS = sta-src/Coverage/coverage.h
COVERAGE_FORMS = sta-src/Coverage/coverage.ui

# ############# SEM Module ##############
SEM_SOURCES = sta-src/SEM/Planets.cpp \
    sta-src/SEM/preliminaryTables.cpp \
    sta-src/SEM/sem.cpp \
    sta-src/SEM/semvehicle.cpp
SEM_HEADERS = sta-src/SEM/Planets.h \
    sta-src/SEM/preliminaryTables.h \
    sta-src/SEM/sem.h \
    sta-src/SEM/semvehicle.h
SEM_FORMS = sta-src/SEM/semvehicle.ui \
    sta-src/SEM/sem.ui

# ############# 3-Body Module ##############
LAGRANGIAN_SOURCES = sta-src/Lagrangian/trajectoryprinting.cpp \
    sta-src/Lagrangian/halorbitcomputation.cpp \
    sta-src/Lagrangian/lagrangian.cpp \
    sta-src/Lagrangian/InizializeOptimizer.cpp \
    sta-src/Lagrangian/EvaluateModel.cpp \
    sta-src/Lagrangian/EarthMoonTransfer.cpp \
    sta-src/Lagrangian/lagrangianAdvanced.cpp
LAGRANGIAN_HEADERS = sta-src/Lagrangian/halorbitcomputation.h \
    sta-src/Lagrangian/trajectoryprinting.h \
    sta-src/Lagrangian/lagrangian.h \
    sta-src/Lagrangian/EarthMoonTransfer.h \
    sta-src/Lagrangian/lagrangianAdvanced.h
LAGRANGIAN_FORMS = sta-src/Lagrangian/lagrangian.ui \
    sta-src/Lagrangian/lagrangianAdvanced.ui

############## Optimization ##############
OPTIMIZATION_SOURCES = sta-src/Optimization/pso1D.cpp \
    sta-src/Optimization/nsga2.cpp \
    sta-src/Optimization/dgmopso.cpp \
    sta-src/Optimization/RunOptimizer.cpp \
    sta-src/Optimization/arrays.cpp
OPTIMIZATION_HEADERS = sta-src/Optimization/GlobalOptimizers.h \
    sta-src/Optimization/arrays.h \
    sta-src/Optimization/random.h \
    sta-src/Optimization/input.h \
    sta-src/Optimization/realinit.h \
    sta-src/Optimization/init.h \
    sta-src/Optimization/decode.h \
    sta-src/Optimization/ranking.h \
    sta-src/Optimization/rancon.h \
    sta-src/Optimization/dfit.h \
    sta-src/Optimization/select.h \
    sta-src/Optimization/crossover.h \
    sta-src/Optimization/uniformxr.h \
    sta-src/Optimization/realcross2.h \
    sta-src/Optimization/mut.h \
    sta-src/Optimization/realmut1.h \
    sta-src/Optimization/keepaliven.h

############## Plotting Module ##############
PLOT_SOURCES = \
    sta-src/Plotting/groundtrackplottool.cpp \
    sta-src/Plotting/plottingtool.cpp \
    sta-src/Plotting/threedvisualizationtool.cpp \
    sta-src/Plotting/visualizationtoolbar.cpp
PLOT_HEADERS = \
    sta-src/Plotting/groundtrackplottool.h \
    sta-src/Plotting/plottingtool.h \
    sta-src/Plotting/threedvisualizationtool.h \
    sta-src/Plotting/visualizationtoolbar.h
PLOT_FORMS = \
    sta-src/Plotting/plottingtool.ui

########################### Scenario  #################################
SCENARIO_SOURCES = sta-src/Scenario/scenarioabstracttrajectory.cpp \
    sta-src/Scenario/scenarioappearance.cpp \
    sta-src/Scenario/scenarioascent.cpp \
    sta-src/Scenario/scenariobody.cpp \
    sta-src/Scenario/scenariodesignoverview.cpp \
    sta-src/Scenario/scenarioenvironment.cpp \
    sta-src/Scenario/scenariogroundelement.cpp \
    sta-src/Scenario/scenarioinitialstate.cpp \
    sta-src/Scenario/scenariogravitymodel.cpp \
    sta-src/Scenario/scenariomanoeuvre.cpp \
    sta-src/Scenario/scenariomanoeuvreplan.cpp \
    sta-src/Scenario/scenariolocation.cpp \
    sta-src/Scenario/scenarioloitering.cpp \
    sta-src/Scenario/scenarioloiteringtle.cpp \
    sta-src/Scenario/scenarioobject.cpp \
    sta-src/Scenario/scenarioparticipant.cpp \
    sta-src/Scenario/scenariopayload.cpp \
    sta-src/Scenario/scenarioproperties.cpp \
    sta-src/Scenario/scenariopropulsion.cpp \
    sta-src/Scenario/scenarioreentry.cpp \
    sta-src/Scenario/scenarioreentrymode.cpp \
    sta-src/Scenario/scenarioreentryproperties.cpp \
    sta-src/Scenario/scenariorendezvous.cpp \
    sta-src/Scenario/scenariosimulationparameters.cpp \
    sta-src/Scenario/scenariospacevehicle.cpp \
    sta-src/Scenario/scenariotrajectoryplan.cpp \
    sta-src/Scenario/scenariotrajectorypropagation.cpp \
    sta-src/Scenario/spacescenario.cpp \
    sta-src/Scenario/spacescenarioreader.cpp \
    sta-src/Scenario/scenariothreebodyenvironment.cpp \
    sta-src/Scenario/scenariothreebodytransfer.cpp \
    sta-src/Scenario/scenariolagrangian.cpp \
    sta-src/Scenario/scenariohalo.cpp \
    sta-src/Scenario/scenariomanifold.cpp \
    sta-src/Scenario/scenarioperturbations.cpp \
    sta-src/Scenario/scenarioexternal.cpp
SCENARIO_HEADERS = sta-src/Scenario/propagationfeedback.h \
    sta-src/Scenario/scenario.h \
    sta-src/Scenario/scenarioabstracttrajectory.h \
    sta-src/Scenario/scenarioappearance.h \
    sta-src/Scenario/scenarioascent.h \
    sta-src/Scenario/scenariobody.h \
    sta-src/Scenario/scenariodesignoverview.h \
    sta-src/Scenario/scenarioenvironment.h \
    sta-src/Scenario/scenariogravitymodel.h \
    sta-src/Scenario/scenariogroundelement.h \
    sta-src/Scenario/scenarioinitialstate.h \
    sta-src/Scenario/scenariolocation.h \
    sta-src/Scenario/scenarioloitering.h \
    sta-src/Scenario/scenarioloiteringtle.h \
    sta-src/Scenario/scenariomanoeuvre.h \
    sta-src/Scenario/scenariomanoeuvreplan.h \
    sta-src/Scenario/scenarioobject.h \
    sta-src/Scenario/scenarioparticipant.h \
    sta-src/Scenario/scenariopayload.h \
    sta-src/Scenario/scenarioproperties.h \
    sta-src/Scenario/scenarioreentry.h \
    sta-src/Scenario/scenarioreentrymode.h \
    sta-src/Scenario/scenarioreentryproperties.h \
    sta-src/Scenario/scenariopropulsion.h \
    sta-src/Scenario/scenariorendezvous.h \
    sta-src/Scenario/scenariosimulationparameters.h \
    sta-src/Scenario/scenariospacevehicle.h \
    sta-src/Scenario/scenariotrajectoryplan.h \
    sta-src/Scenario/scenariotrajectorypropagation.h \
    sta-src/Scenario/spacescenario.h \
    sta-src/Scenario/spacescenarioreader.h \
    sta-src/Scenario/scenariothreebodyenvironment.h \
    sta-src/Scenario/scenariothreebodytransfer.h \
    sta-src/Scenario/scenariolagrangian.h \
    sta-src/Scenario/scenariohalo.h \
    sta-src/Scenario/scenariomanifold.h \
    sta-src/Scenario/scenarioperturbations.h \
    sta-src/Scenario/scenarioexternal.h

############## Constellations Module ##############
CONSTELLATIONS_SOURCES = sta-src/Constellations/cwizard.cpp
CONSTELLATIONS_HEADERS = sta-src/Constellations/cwizard.h
CONSTELLATIONS_FORMS = sta-src/Constellations/cwizard.ui

################ Celestia sources ####################
UTIL_SOURCES = $$CEL/celutil/bigfix.cpp \
    $$CEL/celutil/color.cpp \
    $$CEL/celutil/debug.cpp \
    $$CEL/celutil/directory.cpp \
    $$CEL/celutil/filetype.cpp \
    $$CEL/celutil/formatnum.cpp \
    $$CEL/celutil/utf8.cpp \
    $$CEL/celutil/util.cpp
UTIL_HEADERS = $$CEL/celutil/basictypes.h \
    $$CEL/celutil/bigfix.h \
    $$CEL/celutil/bytes.h \
    $$CEL/celutil/color.h \
    $$CEL/celutil/debug.h \
    $$CEL/celutil/directory.h \
    $$CEL/celutil/filetype.h \
    $$CEL/celutil/formatnum.h \
    $$CEL/celutil/reshandle.h \
    $$CEL/celutil/resmanager.h \
    $$CEL/celutil/timer.h \
    $$CEL/celutil/utf8.h \
    $$CEL/celutil/util.h \
    $$CEL/celutil/watcher.h

win32 { 
    UTIL_SOURCES += $$CEL/celutil/windirectory.cpp \
        $$CEL/celutil/wintimer.cpp
    UTIL_HEADERS += $$CEL/celutil/winutil.h
}
unix:UTIL_SOURCES += $$CEL/celutil/unixdirectory.cpp \
    $$CEL/celutil/unixtimer.cpp

############# Celestia Math library ################
MATH_SOURCES = $$CEL/celmath/frustum.cpp \
    $$CEL/celmath/perlin.cpp
MATH_HEADERS = $$CEL/celmath/aabox.h \
    $$CEL/celmath/capsule.h \
    $$CEL/celmath/distance.h \
    $$CEL/celmath/ellipsoid.h \
    $$CEL/celmath/frustum.h \
    $$CEL/celmath/intersect.h \
    $$CEL/celmath/mathlib.h \
    $$CEL/celmath/perlin.h \
    $$CEL/celmath/plane.h \
    $$CEL/celmath/quaternion.h \
    $$CEL/celmath/ray.h \
    $$CEL/celmath/solve.h \
    $$CEL/celmath/sphere.h \
    $$CEL/celmath/vecmath.h

############ Celestia 3DS Mesh library ##########
TDS_SOURCES = $$CEL/cel3ds/3dsmodel.cpp \
    $$CEL/cel3ds/3dsread.cpp
TDS_HEADERS = $$CEL/cel3ds/3dschunk.h \
    $$CEL/cel3ds/3dsmodel.h \
    $$CEL/cel3ds/3dsread.h

############# Celestia Texture font library #########
TXF_SOURCES = $$CEL/celtxf/texturefont.cpp
TXF_HEADERS = $$CEL/celtxf/texturefont.h

################## Celestia Engine ############
ENGINE_SOURCES = $$CEL/celengine/asterism.cpp \
    $$CEL/celengine/astro.cpp \
    $$CEL/celengine/axisarrow.cpp \
    $$CEL/celengine/body.cpp \
    $$CEL/celengine/boundaries.cpp \
    $$CEL/celengine/catalogxref.cpp \
    $$CEL/celengine/cmdparser.cpp \
    $$CEL/celengine/command.cpp \
    $$CEL/celengine/console.cpp \
    $$CEL/celengine/constellation.cpp \
    $$CEL/celengine/customorbit.cpp \
    $$CEL/celengine/customrotation.cpp \
    $$CEL/celengine/dds.cpp \
    $$CEL/celengine/deepskyobj.cpp \
    $$CEL/celengine/dispmap.cpp \
    $$CEL/celengine/dsodb.cpp \
    $$CEL/celengine/dsoname.cpp \
    $$CEL/celengine/dsooctree.cpp \
    $$CEL/celengine/execution.cpp \
    $$CEL/celengine/fragmentprog.cpp \
    $$CEL/celengine/frame.cpp \
    $$CEL/celengine/frametree.cpp \
    $$CEL/celengine/galaxy.cpp \
    $$CEL/celengine/glcontext.cpp \
    $$CEL/celengine/glext.cpp \
    $$CEL/celengine/globular.cpp \
    $$CEL/celengine/glshader.cpp \
    $$CEL/celengine/image.cpp \
    $$CEL/celengine/jpleph.cpp \
    $$CEL/celengine/location.cpp \
    $$CEL/celengine/lodspheremesh.cpp \
    $$CEL/celengine/marker.cpp \
    $$CEL/celengine/mesh.cpp \
    $$CEL/celengine/meshmanager.cpp \
    $$CEL/celengine/model.cpp \
    $$CEL/celengine/modelfile.cpp \
    $$CEL/celengine/multitexture.cpp \
    $$CEL/celengine/nebula.cpp \
    $$CEL/celengine/nutation.cpp \
    $$CEL/celengine/observer.cpp \
    $$CEL/celengine/opencluster.cpp \
    $$CEL/celengine/orbit.cpp \
    $$CEL/celengine/overlay.cpp \
    $$CEL/celengine/parseobject.cpp \
    $$CEL/celengine/parser.cpp \
    $$CEL/celengine/particlesystem.cpp \
    $$CEL/celengine/particlesystemfile.cpp \
    $$CEL/celengine/planetgrid.cpp \
    $$CEL/celengine/precession.cpp \
    $$CEL/celengine/regcombine.cpp \
    $$CEL/celengine/rendcontext.cpp \
    $$CEL/celengine/render.cpp \
    $$CEL/celengine/renderglsl.cpp \
    $$CEL/celengine/rotation.cpp \
    $$CEL/celengine/rotationmanager.cpp \
    $$CEL/celengine/samporbit.cpp \
    $$CEL/celengine/samporient.cpp \
    $$CEL/celengine/scriptobject.cpp \
    $$CEL/celengine/scriptorbit.cpp \
    $$CEL/celengine/scriptrotation.cpp \
    $$CEL/celengine/selection.cpp \
    $$CEL/celengine/shadermanager.cpp \
    $$CEL/celengine/simulation.cpp \
    $$CEL/celengine/skygrid.cpp \
    $$CEL/celengine/solarsys.cpp \
    $$CEL/celengine/spheremesh.cpp \
    $$CEL/celengine/star.cpp \
    $$CEL/celengine/starcolors.cpp \
    $$CEL/celengine/stardb.cpp \
    $$CEL/celengine/starname.cpp \
    $$CEL/celengine/staroctree.cpp \
    $$CEL/celengine/stellarclass.cpp \
    $$CEL/celengine/texmanager.cpp \
    $$CEL/celengine/texture.cpp \
    $$CEL/celengine/timeline.cpp \
    $$CEL/celengine/timelinephase.cpp \
    $$CEL/celengine/tokenizer.cpp \
    $$CEL/celengine/trajmanager.cpp \
    $$CEL/celengine/univcoord.cpp \
    $$CEL/celengine/universe.cpp \
    $$CEL/celengine/vertexlist.cpp \
    $$CEL/celengine/vertexprog.cpp \
    $$CEL/celengine/virtualtex.cpp \
    $$CEL/celengine/visibleregion.cpp \
    $$CEL/celengine/vsop87.cpp
ENGINE_HEADERS = $$CEL/celengine/asterism.h \
    $$CEL/celengine/astro.h \
    $$CEL/celengine/atmosphere.h \
    $$CEL/celengine/axisarrow.h \
    $$CEL/celengine/body.h \
    $$CEL/celengine/boundaries.h \
    $$CEL/celengine/catalogxref.h \
    $$CEL/celengine/celestia.h \
    $$CEL/celengine/cmdparser.h \
    $$CEL/celengine/command.h \
    $$CEL/celengine/console.h \
    $$CEL/celengine/constellation.h \
    $$CEL/celengine/customorbit.h \
    $$CEL/celengine/customrotation.h \
    $$CEL/celengine/deepskyobj.h \
    $$CEL/celengine/dispmap.h \
    $$CEL/celengine/dsodb.h \
    $$CEL/celengine/dsoname.h \
    $$CEL/celengine/dsooctree.h \
    $$CEL/celengine/execenv.h \
    $$CEL/celengine/execution.h \
    $$CEL/celengine/fragmentprog.h \
    $$CEL/celengine/frame.h \
    $$CEL/celengine/frametree.h \
    $$CEL/celengine/galaxy.h \
    $$CEL/celengine/gl.h \
    $$CEL/celengine/glcontext.h \
    $$CEL/celengine/glext.h \
    $$CEL/celengine/globular.h \
    $$CEL/celengine/glshader.h \
    $$CEL/celengine/image.h \
    $$CEL/celengine/jpleph.h \
    $$CEL/celengine/lightenv.h \
    $$CEL/celengine/location.h \
    $$CEL/celengine/lodspheremesh.h \
    $$CEL/celengine/marker.h \
    $$CEL/celengine/mesh.h \
    $$CEL/celengine/meshmanager.h \
    $$CEL/celengine/model.h \
    $$CEL/celengine/modelfile.h \
    $$CEL/celengine/multitexture.h \
    $$CEL/celengine/nebula.h \
    $$CEL/celengine/nutation.h \
    $$CEL/celengine/observer.h \
    $$CEL/celengine/octree.h \
    $$CEL/celengine/opencluster.h \
    $$CEL/celengine/orbit.h \
    $$CEL/celengine/overlay.h \
    $$CEL/celengine/parseobject.h \
    $$CEL/celengine/parser.h \
    $$CEL/celengine/particlesystem.h \
    $$CEL/celengine/particlesystemfile.h \
    $$CEL/celengine/planetgrid.h \
    $$CEL/celengine/precession.h \
    $$CEL/celengine/referencemark.h \
    $$CEL/celengine/regcombine.h \
    $$CEL/celengine/rendcontext.h \
    $$CEL/celengine/render.h \
    $$CEL/celengine/renderglsl.h \
    $$CEL/celengine/renderinfo.h \
    $$CEL/celengine/rotation.h \
    $$CEL/celengine/rotationmanager.h \
    $$CEL/celengine/samporbit.h \
    $$CEL/celengine/samporient.h \
    $$CEL/celengine/scriptobject.h \
    $$CEL/celengine/scriptorbit.h \
    $$CEL/celengine/scriptrotation.h \
    $$CEL/celengine/selection.h \
    $$CEL/celengine/shadermanager.h \
    $$CEL/celengine/simulation.h \
    $$CEL/celengine/skygrid.h \
    $$CEL/celengine/solarsys.h \
    $$CEL/celengine/spheremesh.h \
    $$CEL/celengine/star.h \
    $$CEL/celengine/starcolors.h \
    $$CEL/celengine/stardb.h \
    $$CEL/celengine/starname.h \
    $$CEL/celengine/staroctree.h \
    $$CEL/celengine/stellarclass.h \
    $$CEL/celengine/surface.h \
    $$CEL/celengine/texmanager.h \
    $$CEL/celengine/texture.h \
    $$CEL/celengine/timeline.h \
    $$CEL/celengine/timelinephase.h \
    $$CEL/celengine/tokenizer.h \
    $$CEL/celengine/trajmanager.h \
    $$CEL/celengine/univcoord.h \
    $$CEL/celengine/universe.h \
    $$CEL/celengine/vecgl.h \
    $$CEL/celengine/vertexlist.h \
    $$CEL/celengine/vertexprog.h \
    $$CEL/celengine/virtualtex.h \
    $$CEL/celengine/visibleregion.h \
    $$CEL/celengine/vsop87.h
CELESTIA_SPICE_SOURCES = $$CEL/celengine/spiceinterface.cpp \
    $$CEL/celengine/spiceorbit.cpp \
    $$CEL/celengine/spicerotation.cpp
CELESTIA_SPICE_HEADERS = $$CEL/celengine/spiceinterface.h \
    $$CEL/celengine/spiceorbit.h \
    $$CEL/celengine/spicerotation.h

############### Celestia App sources ##################
CELESTIA_APP_SOURCES = $$CEL/celestia/celestiacore.cpp \
    $$CEL/celestia/configfile.cpp \
    $$CEL/celestia/destination.cpp \
    $$CEL/celestia/eclipsefinder.cpp \
    $$CEL/celestia/favorites.cpp \
    $$CEL/celestia/imagecapture.cpp \
    $$CEL/celestia/scriptmenu.cpp \
    $$CEL/celestia/url.cpp \
    $$CEL/celestia/celx.cpp \
    $$CEL/celestia/celx_celestia.cpp \
    $$CEL/celestia/celx_frame.cpp \
    $$CEL/celestia/celx_gl.cpp \
    $$CEL/celestia/celx_object.cpp \
    $$CEL/celestia/celx_observer.cpp \
    $$CEL/celestia/celx_phase.cpp \
    $$CEL/celestia/celx_position.cpp \
    $$CEL/celestia/celx_rotation.cpp \
    $$CEL/celestia/celx_vector.cpp
CELESTIA_APP_HEADERS = $$CEL/celestia/celestiacore.h \
    $$CEL/celestia/configfile.h \
    $$CEL/celestia/destination.h \
    $$CEL/celestia/eclipsefinder.h \
    $$CEL/celestia/favorites.h \
    $$CEL/celestia/imagecapture.h \
    $$CEL/celestia/scriptmenu.h \
    $$CEL/celestia/url.h \
    $$CEL/celestia/celx.h \
    $$CEL/celestia/celx_celestia.h \
    $$CEL/celestia/celx_internal.h \
    $$CEL/celestia/celx_frame.h \
    $$CEL/celestia/celx_gl.h \
    $$CEL/celestia/celx_object.h \
    $$CEL/celestia/celx_observer.h \
    $$CEL/celestia/celx_phase.h \
    $$CEL/celestia/celx_position.h \
    $$CEL/celestia/celx_rotation.h \
    $$CEL/celestia/celx_vector.h
macx { 
    CELESTIA_APP_SOURCES -= $$CEL/celestia/imagecapture.cpp
    CELESTIA_APP_SOURCES += $$MACOSX_LIBRARIES_DIR/POSupport.cpp
    CELESTIA_APP_HEADERS += $$MACOSX_LIBRARIES_DIR/POSupport.h
}

############# Celestia Qt4 front-end sources ###############
CELESTIA_QTAPP_SOURCES = $$CEL/celestia/qt/qtbookmark.cpp \
    $$CEL/celestia/qt/qtglwidget.cpp \
    $$CEL/celestia/qt/qtpreferencesdialog.cpp \
    $$CEL/celestia/qt/qtcelestialbrowser.cpp \
    $$CEL/celestia/qt/qtdeepskybrowser.cpp \
    $$CEL/celestia/qt/qtsolarsystembrowser.cpp \
    $$CEL/celestia/qt/qtselectionpopup.cpp \
    $$CEL/celestia/qt/qtcolorswatchwidget.cpp \
    $$CEL/celestia/qt/qttimetoolbar.cpp \
    $$CEL/celestia/qt/qtcelestiaactions.cpp \
    $$CEL/celestia/qt/qtinfopanel.cpp \
    $$CEL/celestia/qt/qteventfinder.cpp \
    $$CEL/celestia/qt/qtsettimedialog.cpp \
    $$CEL/celestia/qt/xbel.cpp
CELESTIA_QTAPP_HEADERS = $$CEL/celestia/qt/qtbookmark.h \
    $$CEL/celestia/qt/qtglwidget.h \
    $$CEL/celestia/qt/qtpreferencesdialog.h \
    $$CEL/celestia/qt/qtcelestialbrowser.h \
    $$CEL/celestia/qt/qtdeepskybrowser.h \
    $$CEL/celestia/qt/qtsolarsystembrowser.h \
    $$CEL/celestia/qt/qtselectionpopup.h \
    $$CEL/celestia/qt/qtcolorswatchwidget.h \
    $$CEL/celestia/qt/qttimetoolbar.h \
    $$CEL/celestia/qt/qtcelestiaactions.h \
    $$CEL/celestia/qt/qtinfopanel.h \
    $$CEL/celestia/qt/qteventfinder.h \
    $$CEL/celestia/qt/qtsettimedialog.h \
    $$CEL/celestia/qt/xbel.h
CELESTIA_RESOURCES = $$CEL/celestia/qt/icons.qrc
CELESTIA_FORMS = $$CEL/celestia/qt/addbookmark.ui \
    $$CEL/celestia/qt/newbookmarkfolder.ui \
    $$CEL/celestia/qt/organizebookmarks.ui \
    $$CEL/celestia/qt/preferences.ui
CELESTIA_SOURCES = $$UTIL_SOURCES \
    $$MATH_SOURCES \
    $$TXF_SOURCES \
    $$TDS_SOURCES \
    $$ENGINE_SOURCES \
    $$CELESTIA_APP_SOURCES \
    $$CELESTIA_QTAPP_SOURCES
CELESTIA_HEADERS = $$UTIL_HEADERS \
    $$MATH_HEADERS \
    $$TXF_HEADERS \
    $$TDS_HEADERS \
    $$ENGINE_HEADERS \
    $$CELESTIA_APP_HEADERS \
    $$CELESTIA_QTAPP_HEADERS

############ GL extension wrangler ################
GLEW_SOURCES = \
    thirdparty/glew/src/glew.c
GLEW_HEADERS = \
    thirdparty/glew/include/GL/glew.h \
    thirdparty/glew/include/GL/glxew.h \
    thirdparty/glew/include/GL/wglew.h

############ CurvePlot (high precision OpenGL curve drawing) ###########
CURVEPLOT_SOURCES = \
    thirdparty/curveplot/src/curveplot.cpp
CURVEPLOT_HEADERS = \
    thirdparty/curveplot/include/curveplot.h

############## QwtPlot3d main ####################
QWT3D_SOURCES += \
    $$QWT3D_DIR/src/qwt3d_axis.cpp \
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
QWT3D_SOURCES += \
    $$QWT3D_DIR/src/qwt3d_gridmapping.cpp \
    $$QWT3D_DIR/src/qwt3d_parametricsurface.cpp \
    $$QWT3D_DIR/src/qwt3d_function.cpp
QWT3D_SOURCES += \
    $$QWT3D_DIR/src/qwt3d_surfaceplot.cpp \
    $$QWT3D_DIR/src/qwt3d_gridplot.cpp \
    $$QWT3D_DIR/src/qwt3d_meshplot.cpp
QWT3D_HEADERS += \
    $$QWT3D_DIR/include/qwt3d_color.h \
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
QWT3D_HEADERS += \
    $$QWT3D_DIR/include/qwt3d_mapping.h \
    $$QWT3D_DIR/include/qwt3d_gridmapping.h \
    $$QWT3D_DIR/include/qwt3d_parametricsurface.h \
    $$QWT3D_DIR/include/qwt3d_function.h
QWT3D_HEADERS += \
    $$QWT3D_DIR/include/qwt3d_surfaceplot.h \
    $$QWT3D_DIR/include/qwt3d_volumeplot.h \
    $$QWT3D_DIR/include/qwt3d_graphplot.h \
    $$QWT3D_DIR/include/qwt3d_multiplot.h

############## QwtPlot3d gl2ps support ##############
QWT3D_HEADERS += \
    $$QWT3D_DIR/3rdparty/gl2ps/gl2ps.h \
    $$QWT3D_DIR/include/qwt3d_io_gl2ps.h
QWT3D_SOURCES += \
    $$QWT3D_DIR/src/qwt3d_io_gl2ps.cpp \
    $$QWT3D_DIR/3rdparty/gl2ps/gl2ps.c

############## TLEs support ##############
NORAD_HEADERS += \
    thirdparty/noradtle/norad.h
NORAD_SOURCES += \
    thirdparty/noradtle/basics.cpp \
    thirdparty/noradtle/common.cpp \
    thirdparty/noradtle/deep.cpp \
    thirdparty/noradtle/get_el.cpp \
    thirdparty/noradtle/sdp4.cpp \
    thirdparty/noradtle/sdp8.cpp \
    thirdparty/noradtle/sgp.cpp \
    thirdparty/noradtle/sgp4.cpp \
    thirdparty/noradtle/sgp8.cpp


THIRDPARTY_SOURCES = $$GLEW_SOURCES $$CURVEPLOT_SOURCES $$QWT3D_SOURCES $$NORAD_SOURCES
THIRDPARTY_HEADERS = $$GLEW_HEADERS $$CURVEPLOT_HEADERS $$QWT3D_HEADERS $$NORAD_HEADERS

DEFINES += GLEW_STATIC


################ All app sources, headers, and forms ############
SOURCES = $$MAIN_SOURCES \
    $$ASTROCORE_SOURCES \
    $$SEM_SOURCES \
    $$PLOT_SOURCES \
    $$SCENARIO_SOURCES \
    $$ENTRY_SOURCES \
    $$LOCATIONS_SOURCES \
    $$LOITERING_SOURCES \
    $$EXTERNAL_SOURCES \
    $$RENDEZVOUS_SOURCES \
    $$LAGRANGIAN_SOURCES \
    $$OPTIMIZATION_SOURCES \
    $$CALCULATOR_SOURCES \
    $$CELESTIA_SOURCES \
    $$THIRDPARTY_SOURCES
HEADERS = $$MAIN_HEADERS \
    $$ASTROCORE_HEADERS \
    $$SEM_HEADERS \
    $$PLOT_HEADERS \
    $$SCENARIO_HEADERS \
    $$ENTRY_HEADERS \
    $$LOCATIONS_HEADERS \
    $$LOITERING_HEADERS \
    $$EXTERNAL_HEADERS \
    $$RENDEZVOUS_HEADERS \
    $$LAGRANGIAN_HEADERS \
    $$OPTIMIZATION_HEADER \
    $$CALCULATOR_HEADERS \
    $$CELESTIA_HEADERS \
    $$THIRDPARTY_HEADERS
FORMS = \
    $$MAIN_FORMS \
    $$ASTROCORE_FORMS \
    $$SEM_FORMS \
    $$PLOT_FORMS \
    $$ENTRY_FORMS \
    $$LOCATIONS_FORMS \
    $$CALCULATOR_FORMS \
    $$LOITERING_FORMS \
    $$EXTERNAL_FORMS \
    $$RENDEZVOUS_FORMS \
    $$LAGRANGIAN_FORMS \
    $$CELESTIA_FORMS

RESOURCES = $$CELESTIA_RESOURCES iconary/sta-icons.qrc

UI_HEADERS_DIR = sta-src/ui/include
UI_SOURCES_DIR = sta-src/ui/src

INCLUDEPATH += .
INCLUDEPATH += sta-src
INCLUDEPATH += thirdparty
INCLUDEPATH += thirdparty/celestia-src
INCLUDEPATH += thirdparty/glew/include
INCLUDEPATH += thirdparty/curveplot/include
INCLUDEPATH += thirdparty/qwtplot3d/include
INCLUDEPATH += thirdparty/noradtle


win32 { 
    # SPICE support
    # SOURCES += $$SPICE_SOURCES
    # HEADERS += $$SPICE_HEADERS
    # DEFINES += USE_SPICE
    # INCLUDEPATH += windows/inc/spice
    INCLUDEPATH += $$WINDOWS_LIBRARIES_DIR/inc/libintl \
        $$WINDOWS_LIBRARIES_DIR/inc/libz \
        $$WINDOWS_LIBRARIES_DIR/inc/libpng \
        $$WINDOWS_LIBRARIES_DIR/inc/libjpeg \
        $$WINDOWS_LIBRARIES_DIR/inc/lua-5.1
    LIBS += -L$$WINDOWS_LIBRARIES_DIR/lib \
        -lzlib \
        -llibpng \
        -llibjpeg2 \
        -lintl \
        -llua5.1
    RC_FILE = sta-src/sta.rc
    DEFINES += _CRT_SECURE_NO_WARNINGS

    # Disable the regrettable min and max macros in windows.h
    DEFINES += NOMINMAX
}

win32-g++ { 
    # Work around alignment problems with MinGW. Fixed-size Eigen matrices
    # are sometimes allocated on the stack at unaligned addresses even though
    # __alignof e.g. Vector4d is 16. Until we figure out what's going on, we'll
    # just disable the assertions. Since Eigen never generates SSE2 instructions
    # for the g++ 3.4 compiler in MinGW, this should be harmless.
    #message("Warning: Disabling Eigen unaligned memory assertion")
    #DEFINES += EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
    message("Enabling Eigen in Windows")
    QMAKE_CXXFLAGS += -mincoming-stack-boundary=2

    QMAKE_CXXFLAGS_RELEASE = \
        -ffast-math \
        -fexpensive-optimizations \
        -O3 \
        -Bdynamic
}

# Application icon
macx:ICON = iconary/STAlogoARCHEAN.icns

macx{
    QMAKE_CXXFLAGS_RELEASE = \
        -ffast-math \
        -fexpensive-optimizations \
        -O3 \
        -Bdynamic
}

linux-g++ {
    message("Warning: compiling a linux version")

    QMAKE_CXXFLAGS_RELEASE = \
        -ffast-math \
        -fexpensive-optimizations \
        -O3 \
        -Bdynamic

    INCLUDEPATH += $$LINUX_LIBRARIES_DIR
    INCLUDEPATH += /usr/include

    LIBS += -ljpeg
    LIBS += -lpng

    exists( /usr/include/lua5.1/lua.h ) {
       message( " - Configuring LUA for Debian-type installation..." )
       INCLUDEPATH += /usr/include/lua5.1
       LIBS += -llua5.1
    }
    exists( /usr/include/lua.h ) {
       message( " - Configuring LUA for RPM-type installation..." )
       LIBS += -llua
    }
}

################### Package files ###################
CATALOG_FILES = $$CATALOG_SOURCE/stars.dat \
    $$CATALOG_SOURCE/starnames.dat \
    $$CATALOG_SOURCE/saoxindex.dat \
    $$CATALOG_SOURCE/hdxindex.dat \
    $$CATALOG_SOURCE/nearstars.stc \
    $$CATALOG_SOURCE/extrasolar.stc \
#    $$CATALOG_SOURCE/visualbins.stc \
#    $$CATALOG_SOURCE/spectbins.stc \
    $$CATALOG_SOURCE/revised.stc \
#    $$CATALOG_SOURCE/deepsky.dsc \
    $$CATALOG_SOURCE/solarsys.ssc \
    $$CATALOG_SOURCE/asteroids.ssc \
    $$CATALOG_SOURCE/comets.ssc \
    $$CATALOG_SOURCE/minormoons.ssc \
    $$CATALOG_SOURCE/numberedmoons.ssc \
    $$CATALOG_SOURCE/outersys.ssc \
    $$CATALOG_SOURCE/world-capitals.ssc \
    $$CATALOG_SOURCE/merc_locs.ssc \
    $$CATALOG_SOURCE/venus_locs.ssc \
    $$CATALOG_SOURCE/mars_locs.ssc \
    $$CATALOG_SOURCE/moon_locs.ssc \
    $$CATALOG_SOURCE/marsmoons_locs.ssc \
    $$CATALOG_SOURCE/jupitermoons_locs.ssc \
    $$CATALOG_SOURCE/saturnmoons_locs.ssc \
    $$CATALOG_SOURCE/uranusmoons_locs.ssc \
    $$CATALOG_SOURCE/neptunemoons_locs.ssc \
    $$CATALOG_SOURCE/extrasolar.ssc \
    $$CATALOG_SOURCE/asterisms.dat \
    $$CATALOG_SOURCE/boundaries.dat
CONFIGURATION_SOURCE = sta-data
CONFIGURATION_FILES = $$CONFIGURATION_SOURCE/STA.cfg \
    $$CONFIGURATION_SOURCE/ESA_tour.cel \
    $$CONFIGURATION_SOURCE/start.cel

EPHEMERIS_SOURCE = sta-data/ephemerides
EPHEMERIS_FILES = sta-data/ephemerides/de406_1800-2100.dat
TEXTURE_SOURCE = sta-data/textures/medres
TEXTURE_FILES = 
MODEL_SOURCE = sta-data/models
MODEL_FILES = 
SHADER_SOURCE = sta-data/shaders
SHADER_FILES = 
FONT_SOURCE = sta-data/fonts
FONT_FILES = 
HEATRATE_SOURCE = sta-data/data/heatrates
HEATRATE_FILES = 
AERO_SOURCE = sta-data/data/aerodynamics
AERO_FILES = 
ATMOSPHERES_SOURCE = sta-data/data/atmospheres
ATMOSPHERES_FILES = 
BODIES_SOURCE = sta-data/data/bodies
BODIES_FILES = 
TLEs_SOURCE = sta-data/TLEs
TLEs_FILES =
EXAMPLES_SOURCE = sta-data/scenario-examples
EXAMPLES_FILES =
MACOSXIconFiles_SOURCE = iconary
MACOSXIconFiles_FILES =
USERMANUAL_SOURCE = sta-data/UM
USERMANUAL_FILES =

################ MAC OS X bundle ########################
macx { 
    # Scan directories for files for Mac bundle
    FILES = $$system(ls $$TEXTURE_SOURCE)
    TEXTURE_FILES = $$join(FILES, " $$TEXTURE_SOURCE/", $$TEXTURE_SOURCE/)
    FILES = $$system(ls $$MODEL_SOURCE)
    MODEL_FILES = $$join(FILES, " $$MODEL_SOURCE/", $$MODEL_SOURCE/)
    FILES = $$system(ls $$SHADER_SOURCE)
    SHADER_FILES = $$join(FILES, " $$SHADER_SOURCE/", $$SHADER_SOURCE/)
    FILES = $$system(ls $$FONT_SOURCE)
    FONT_FILES = $$join(FILES, " $$FONT_SOURCE/", $$FONT_SOURCE/)
    FILES = $$system(ls $$HEATRATE_SOURCE)
    HEATRATE_FILES = $$join(FILES, " $$HEATRATE_SOURCE/", $$HEATRATE_SOURCE/)
    FILES = $$system(ls $$AERO_SOURCE)
    AERO_FILES = $$join(FILES, " $$AERO_SOURCE/", $$AERO_SOURCE/)
    FILES = $$system(ls $$ATMOSPHERES_SOURCE)
    ATMOSPHERES_FILES = $$join(FILES, " $$ATMOSPHERES_SOURCE/", $$ATMOSPHERES_SOURCE/)
    FILES = $$system(ls $$BODIES_SOURCE)
    BODIES_FILES = $$join(FILES, " $$BODIES_SOURCE/", $$BODIES_SOURCE/)
    FILES = $$system(ls $$TLEs_SOURCE)
    TLEs_FILES = $$join(FILES, " $$TLEs_SOURCE/", $$TLEs_SOURCE/)
    FILES = $$system(ls $$EXAMPLES_SOURCE)
    EXAMPLES_FILES = $$join(FILES, " $$EXAMPLES_SOURCE/", $$EXAMPLES_SOURCE/)
    FILES = $$system(ls $$USERMANUAL_SOURCE)
    USERMANUAL_FILES = $$join(FILES, " $$USERMANUAL_SOURCE/", $$USERMANUAL_SOURCE/)
    FILES = $$system(ls $$MACOSXIconFiles_SOURCE)
    MACOSXIconFiles_FILES = $$join(FILES, " $$MACOSXIconFiles_SOURCE/", $$MACOSXIconFiles_SOURCE/)
}

# INCLUDEPATH += /usr/local/cspice/include
# LIBS += -ljpeg -llua /usr/local/cspice/lib/cspice.a
macx { 
    #QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
    #CONFIG += x86 ppc
    message( "Warning: building on MAC OS X for x86 architecture only" )
    CONFIG += x86
    INCLUDEPATH += $$MACOSX_LIBRARIES_DIR
    DEFINES += TARGET_OS_MAC \
        __AIFF__
    PRECOMPILED_HEADER += thirdparty/macosx/Util.h
    FRAMEWORKPATH = thirdparty/macosx/Frameworks
    LIBS -= -ljpeg
    LIBS += -llua
    LIBS += -L$$FRAMEWORKPATH
    DEFINES += PNG_SUPPORT
    FRAMEWORKS.files = $$FRAMEWORKPATH/liblua.dylib \
                       $$FRAMEWORKPATH/libpng.dylib
    FRAMEWORKS.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += FRAMEWORKS
    CONFIGURATION.path = Contents/Resources/STAResources
    CONFIGURATION.files = $$CONFIGURATION_FILES
    CATALOGS.path = Contents/Resources/STAResources/data
    CATALOGS.files = $$CATALOG_FILES
    TEXTURES.path = Contents/Resources/STAResources/textures/medres
    TEXTURES.files = $$TEXTURE_FILES
    MODELS.path = Contents/Resources/STAResources/models
    MODELS.files = $$MODEL_FILES
    FONTS.path = Contents/Resources/STAResources/fonts
    FONTS.files = $$FONT_FILES
    SHADERS.path = Contents/Resources/STAResources/shaders
    SHADERS.files = $$SHADER_FILES
    EPHEMERIDES.path = Contents/Resources/STAResources/ephemerides
    EPHEMERIDES.files = $$EPHEMERIS_FILES
    HEATRATES.path = Contents/Resources/STAResources/data/heatrates
    HEATRATES.files = $$HEATRATE_FILES
    AERODYNAMICS.path = Contents/Resources/STAResources/data/aerodynamics
    AERODYNAMICS.files = $$AERO_FILES
    ATMOSPHERES.path = Contents/Resources/STAResources/data/atmospheres
    ATMOSPHERES.files = $$ATMOSPHERES_FILES
    BODIES.path = Contents/Resources/STAResources/data/bodies
    BODIES.files = $$BODIES_FILES
    TLEs.path = Contents/Resources/STAResources/TLEs
    TLEs.files = $$TLEs_FILES
    EXAMPLES.path = Contents/Resources/STAResources/scenario-examples
    EXAMPLES.files = $$EXAMPLES_FILES
    USERMANUAL.path = Contents/Resources/STAResources/UM
    USERMANUAL.files = $$USERMANUAL_FILES
    MACOSXIconFiles.path = Contents/Resources
    MACOSXIconFiles.files = $$MACOSXIconFiles_FILES
    QMAKE_BUNDLE_DATA += CONFIGURATION \
        CATALOGS \
        TEXTURES \
        MODELS \
        FONTS \
        SHADERS \
        EPHEMERIDES \
        TLEs \
        EXAMPLES \
        USERMANUAL \
        MACOSXIconFiles
    QMAKE_BUNDLE_DATA += HEATRATES \
        AERODYNAMICS \
        ATMOSPHERES \
        BODIES
}

DEFINES += CELX \
    LUA_VER=0x050100
