# building without muParser doesn't work yet
SCRIPTING_LANGS += muParser
#SCRIPTING_LANGS += Python

# a console displaying output of scripts; particularly useful on Windows
# where running QtiPlot from a terminal is inconvenient
#DEFINES         += SCRIPTING_CONSOLE
# a dialog for selecting the scripting language on a per-project basis
DEFINES         += SCRIPTING_DIALOG
#DEFINES         += QTIPLOT_DEMO
# comment the following lines to disable donations start-up message
#DEFINES         += QTIPLOT_SUPPORT
# comment the following lines if you haven't subscribed for a QtiPlot binaries maintenance contract
#RESTRICTED_MODULES += EMF

########## To make a binary ##################
######################################################################################
# Uncomment the following line if you want to perform a custom installation using
# the *.path variables defined bellow.
######################################################################################
CONFIG += CustomInstall

CONFIG += x86

CONFIG += debug_and_release
CONFIG += release
#CONFIG += warn_off
#CONFIG   += ordered

#win32: CONFIG   += console

#TEMPLATE = subdirs
#TARGET = qtiplot


######### To make a library  ###############
QMAKE_PROJECT_DEPTH = 0

TARGET         = qtiplot
TEMPLATE       = lib

CONFIG += assistant
CONFIG += designer
CONFIG += qt warn_off exceptions opengl thread
#CONFIG += staticlib
#CONFIG += create_prl

macx {
     # Guillermo: we use LLVM in MAC OS X instead of gcc (2X fast compilation)
     QMAKE_CC = llvm-gcc -arch i386
     QMAKE_CXX = llvm-g++ -arch i386
     DESTDIR = ../../lib/mac
     CONFIG += staticlib
}
win32 {
     DESTDIR = ../../lib/win32-x86-gcc
}
linux {
     DESTDIR = ../../lib/linux-x86
}

QT += qt3support
QT += network
QT += xml xmlpatterns
QT += opengl
QT += svg




##################### 3rd PARTY HEADER FILES SECTION ########################
#!!! Warning: You must modify these paths according to your computer settings
#############################################################################

INCLUDEPATH       += ../
INCLUDEPATH       += ../../sta-src
INCLUDEPATH       += ../../sta-src/ui/include
INCLUDEPATH       += $$PWD
INCLUDEPATH       += $$PWD/qtiplot/src
INCLUDEPATH       += $$PWD/3rdparty/muparser/include
INCLUDEPATH       += $$PWD/3rdparty/qwtplot3d/include
INCLUDEPATH       += $$PWD/3rdparty/qwt/src
INCLUDEPATH       += $$PWD/3rdparty/liborigin
INCLUDEPATH       += $$PWD/3rdparty/gsl/include
INCLUDEPATH       += $$PWD/3rdparty/zlib123/include



##################### 3rd PARTY LIBRARIES SECTION ###########################
#!!! Warning: You must modify these paths according to your computer settings
#############################################################################

##################### Linux (Mac OS X) ######################################

# statically link against libraries in 3rdparty
#linux:LIBS         += $$PWD/3rdparty/muparser/lib/libmuparser.a
linux:LIBS         += ../../lib/linux-x86/libqwtplot3d.so.1.0.0
linux:LIBS         += ../../lib/linux-x86/libqwt.so.5.2.2
#linux:LIBS         += $$PWD/3rdparty/gsl/lib/libgsl.a
#linux:LIBS         += $$PWD/3rdparty/gsl/lib/libgslcblas.a
#linux:LIBS         += $$PWD/3rdparty/zlib123/lib/libz.a

mac:LIBS         += ../../lib/mac/libmuparser.a
mac:LIBS         += ../../lib/mac/libqwtplot3d.a
mac:LIBS         += ../../lib/mac/libqwt.a
mac:LIBS         += ../../lib/mac/libgsl.a
mac:LIBS         += ../../lib/mac/libgslcblas.a
mac:LIBS         += ../../lib/mac/libz.a

# dynamically link against dependencies if they are installed system-wide
linux:LIBS         += -lmuparser
#linux:LIBS         += -lqwtplot3d
#linux:LIBS         += -lqwt
linux:LIBS         += -lgsl -lgslcblas

##################### Windows ###############################################

#win32:LIBS        += $$PWD/3rdparty/muparser/lib/libmuparser.a
#win32:LIBS        += $$PWD/3rdparty/qwtplot3d/lib/libqwtplot3d.a
#win32:LIBS        += $$PWD/3rdparty/qwtplot3d/lib/qwtplot3d.dll
#win32:LIBS        += $$PWD/3rdparty/qwt/lib/libqwt.a
#win32:LIBS        += $$PWD/3rdparty/gsl/lib/libgsl.dll
#win32:LIBS        += $$PWD/3rdparty/gsl/lib/libgslcblas.dll
#win32:LIBS        += $$PWD/3rdparty/zlib123/lib/zdll.lib
#win32:LIBS        += $$PWD/3rdparty/zlib123/lib/libzdll.a
#win32:LIBS        += $$PWD/3rdparty/lib-build-desktop/QtAssistantClient4.dll

#win32:LIBS        += ./lib/win32-x86-gcc/libmuparser.a
#win32:LIBS        += ./lib/win32-x86-gcc/libqwtplot3d.a
#win32:LIBS        += ./lib/win32-x86-gcc/libqwt.a
#win32:LIBS        += ./lib/win32-x86-gcc/libgsl.dll
#win32:LIBS        += ./lib/win32-x86-gcc/libgslcblas.dll
##win32:LIBS        += ./lib/win32-x86-gcc/zdll.lib
#win32:LIBS        += ./lib/win32-x86-gcc/zlib1.dll
##win32:LIBS        += ./lib/win32-x86-gcc/libzdll.a
#win32:LIBS        += ./lib/win32-x86-gcc/QtAssistantClient4.dll
##win32:LIBS        += ./lib/win32-x86-gcc/libQtAssistantClient4.a

win32:LIBS        += ../../lib/win32-x86-gcc/muparser.dll
win32:LIBS        += ../../lib/win32-x86-gcc/qwtplot3d.dll
#win32:LIBS        += ../../lib/win32-x86-gcc/qwtmathml5.dll
win32:LIBS        += ../../lib/win32-x86-gcc/qwt5.dll
win32:LIBS        += ../../lib/win32-x86-gcc/libgsl.dll
win32:LIBS        += ../../lib/win32-x86-gcc/libgslcblas.dll
win32:LIBS        += ../../lib/win32-x86-gcc/zlib1.dll
win32:LIBS        += ../../lib/win32-x86-gcc/msvcrt.dll
win32:LIBS        += ../../lib/win32-x86-gcc/QtAssistantClient4.dll

#win32:LIBS        += C:/STA/trunk/thirdparty/qtiplot/release/analysisParametersChoice.o
#win32:LIBS        += C:/STA/trunk/thirdparty/qtiplot/release/moc_analysisParametersChoice.o

#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################



DEFINES       += QT_PLUGIN
contains(CONFIG, CustomInstall){
    INSTALLS        -= target
    INSTALLS        -= translations
    INSTALLS        -= manual
    INSTALLS        -= documentation
    unix:INSTALLS   -= man

    #unix: INSTALLBASE = /Users/go/Dekstop/MAMA
    #win32: INSTALLBASE = C:/QtiPlot

        #unix: target.path = $$INSTALLBASE/bin
        #unix: translations.path = $$INSTALLBASE/share/qtiplot/translations
        #unix: manual.path = $$INSTALLBASE/share/doc/qtiplot/manual
        #unix: documentation.path = $$INSTALLBASE/share/doc/qtiplot
        #unix: man.path = $$INSTALLBASE/share/man/man1/

        #win32: target.path = $$INSTALLBASE
        #win32: translations.path = $$INSTALLBASE/translations
        #win32: manual.path = $$INSTALLBASE/manual
        #win32: documentation.path = $$INSTALLBASE/doc

    #DEFINES       += TRANSLATIONS_PATH="\\\"$$replace(translations.path," ","\ ")\\\"
    #DEFINES       += MANUAL_PATH="\\\"$$replace(manual.path," ","\ ")\\\"
	}

win32:DEFINES += QT_DLL QT_THREAD_SUPPORT


#MOC_DIR        = ../tmp/qtiplot
#OBJECTS_DIR    = ../tmp/qtiplot
#SIP_DIR        = ../tmp/qtiplot
#DESTDIR        = ./

#############################################################################
###################### PROJECT FILES SECTION ################################
#############################################################################

###################### ICONS ################################################

#win32:RC_FILE = qtiplot/icons/qtiplot.rc
#mac:RC_FILE   = qtiplot/icons/qtiplot.icns

###################### TRANSLATIONS #########################################

TRANSLATIONS    = translations/qtiplot_de.ts \
                  translations/qtiplot_es.ts \
                  translations/qtiplot_fr.ts \
                  translations/qtiplot_ru.ts \
                  translations/qtiplot_ja.ts \
                  translations/qtiplot_sv.ts

#system(lupdate -verbose qtiplot.pro)
#system(lrelease -verbose qtiplot.pro)

translations.files += translations/qtiplot_de.qm \
                  translations/qtiplot_es.qm \
                 translations/qtiplot_fr.qm \
                 translations/qtiplot_ru.qm \
                 translations/qtiplot_ja.qm \
                 translations/qtiplot_sv.qm

###################### DOCUMENTATION ########################################

manual.files += $$PWD/manual/html \
                $$PWD/manual/qtiplot-manual-en.pdf

documentation.files += $$PWD/README.html \
                       $$PWD/gpl_licence.txt

unix: man.files += $$PWD/qtiplot.1

###################### HEADERS ##############################################

HEADERS  += \
            ../../sta-src/Scenario/staschema.h \
            ../../sta-src/Scenario/scenario.h \
            ../../sta-src/Scenario/stascenarioutil.h \
            ../../sta-src/Astro-Core/cartesianTOorbital.h \
            ../../sta-src/Astro-Core/orbitalTOcartesian.h \
            ../../sta-src/Astro-Core/trueAnomalyTOmeanAnomaly.h \
            ../../sta-src/Analysis/analysisParametersChoice.h \
            qtiplot/src/ApplicationWindow.h \
            qtiplot/src/globals.h\
            qtiplot/src/Table.h \
            qtiplot/src/SetColValuesDialog.h \
            qtiplot/src/ExportDialog.h \
            qtiplot/src/TableDialog.h \
            qtiplot/src/pixmaps.h\
            qtiplot/src/SortDialog.h\
            qtiplot/src/ConfigDialog.h \
            qtiplot/src/DataSetDialog.h \
            qtiplot/src/MyParser.h \
            qtiplot/src/ColorBox.h \
            qtiplot/src/SymbolBox.h \
            qtiplot/src/PatternBox.h \
            qtiplot/src/importOPJ.h\
            qtiplot/src/SymbolDialog.h \
            qtiplot/src/ColorButton.h \
            qtiplot/src/RenameWindowDialog.h \
            qtiplot/src/MdiSubWindow.h \
            qtiplot/src/ImportASCIIDialog.h \
            qtiplot/src/Note.h\
            qtiplot/src/Folder.h\
            qtiplot/src/FindDialog.h\
            qtiplot/src/ScriptingEnv.h\
            qtiplot/src/Script.h\
            qtiplot/src/ScriptEdit.h\
            qtiplot/src/customevents.h\
            qtiplot/src/ScriptingLangDialog.h\
            qtiplot/src/ScriptWindow.h\
            qtiplot/src/TextFormatButtons.h\
            qtiplot/src/TableStatistics.h\
            qtiplot/src/ColorMapEditor.h\
            qtiplot/src/ColorMapDialog.h\
            qtiplot/src/ExtensibleFileDialog.h\
            qtiplot/src/OpenProjectDialog.h\
            qtiplot/src/CustomActionDialog.h \
            qtiplot/src/DoubleSpinBox.h \
            qtiplot/src/PlotWizard.h \
            qtiplot/src/LineNumberDisplay.h \
            qtiplot/src/PenStyleBox.h \


###################### SOURCES ##############################################

SOURCES  += \
            ../../sta-src/Scenario/staschema.cpp \
            ../../sta-src/Scenario/scenario.cpp \
            ../../sta-src/Scenario/stascenarioutil.cpp \
            ../../sta-src/Astro-Core/cartesianTOorbital.cpp \
            ../../sta-src/Astro-Core/orbitalTOcartesian.cpp \
            ../../sta-src/Astro-Core/trueAnomalyTOmeanAnomaly.cpp \
            ../../sta-src/Analysis/analysisParametersChoice.cpp \
            qtiplot/src/ApplicationWindow.cpp \
            qtiplot/src/Table.cpp \
            qtiplot/src/SetColValuesDialog.cpp \
            qtiplot/src/ExportDialog.cpp \
            qtiplot/src/TableDialog.cpp \
            qtiplot/src/SortDialog.cpp\
            qtiplot/src/ConfigDialog.cpp \
            qtiplot/src/DataSetDialog.cpp \
            qtiplot/src/MyParser.cpp \
            qtiplot/src/ColorBox.cpp \
            qtiplot/src/SymbolBox.cpp \
            qtiplot/src/PatternBox.cpp \
            qtiplot/src/importOPJ.cpp\
            qtiplot/src/SymbolDialog.cpp \
            qtiplot/src/ColorButton.cpp \
            qtiplot/src/RenameWindowDialog.cpp \
            qtiplot/src/MdiSubWindow.cpp \
            qtiplot/src/ImportASCIIDialog.cpp \
            qtiplot/src/Note.cpp\
            qtiplot/src/Folder.cpp\
            qtiplot/src/FindDialog.cpp\
            qtiplot/src/ScriptingEnv.cpp\
            qtiplot/src/Script.cpp\
            qtiplot/src/ScriptEdit.cpp\
            qtiplot/src/ScriptingLangDialog.cpp\
            qtiplot/src/ScriptWindow.cpp\
            qtiplot/src/TextFormatButtons.cpp\
            qtiplot/src/TableStatistics.cpp\
            qtiplot/src/ColorMapEditor.cpp\
            qtiplot/src/ColorMapDialog.cpp\
            qtiplot/src/ExtensibleFileDialog.cpp\
            qtiplot/src/OpenProjectDialog.cpp\
            qtiplot/src/CustomActionDialog.cpp \
            qtiplot/src/DoubleSpinBox.cpp \
            qtiplot/src/PlotWizard.cpp \
            qtiplot/src/LineNumberDisplay.cpp \
            qtiplot/src/PenStyleBox.cpp \


   FORMS += \
            ../../sta-src/Analysis/analysisParametersChoice.ui


###############################################################
##################### Compression (zlib123) ###################
###############################################################

    SOURCES += $$PWD/3rdparty/zlib123/minigzip.c

###############################################################
##################### QAssisstant          ###################
###############################################################

    HEADERS += $$PWD/3rdparty/qtassisstant/qassistantclient_global.h \
               $$PWD/3rdparty/qtassisstant/qassistantclient.h

    SOURCES += $$PWD/3rdparty/qtassisstant/qassistantclient.cpp


###############################################################
################# Origin Import (liborigin) ###################
###############################################################

    HEADERS += $$PWD/3rdparty/liborigin/OPJFile.h
    SOURCES += $$PWD/3rdparty/liborigin/OPJFile.cpp

###############################################################
################# Module: Matrix ##############################
###############################################################

    HEADERS += qtiplot/src/matrix/Matrix.h \
               qtiplot/src/matrix/MatrixCommand.h \
               qtiplot/src/matrix/MatrixDialog.h \
               qtiplot/src/matrix/MatrixModel.h \
               qtiplot/src/matrix/MatrixSizeDialog.h \
               qtiplot/src/matrix/MatrixValuesDialog.h \

    SOURCES += qtiplot/src/matrix/Matrix.cpp \
               qtiplot/src/matrix/MatrixCommand.cpp \
               qtiplot/src/matrix/MatrixDialog.cpp \
               qtiplot/src/matrix/MatrixModel.cpp \
               qtiplot/src/matrix/MatrixSizeDialog.cpp \
               qtiplot/src/matrix/MatrixValuesDialog.cpp \

###############################################################
################# Module: Plot 2D #############################
###############################################################

    HEADERS += qtiplot/src/plot2D/AddWidgetTool.h \
               qtiplot/src/plot2D/ArrowMarker.h \
               qtiplot/src/plot2D/BoxCurve.h \
               qtiplot/src/plot2D/CanvasPicker.h \
               qtiplot/src/plot2D/DataPickerTool.h \
               qtiplot/src/plot2D/FrameWidget.h \
               qtiplot/src/plot2D/FunctionCurve.h \
               qtiplot/src/plot2D/Graph.h \
               qtiplot/src/plot2D/Grid.h \
               qtiplot/src/plot2D/ImageWidget.h \
               qtiplot/src/plot2D/LegendWidget.h \
               qtiplot/src/plot2D/LineProfileTool.h \
               qtiplot/src/plot2D/MultiLayer.h \
               qtiplot/src/plot2D/MultiPeakFitTool.h \
               qtiplot/src/plot2D/PlotCurve.h \
               qtiplot/src/plot2D/PlotToolInterface.h \
               qtiplot/src/plot2D/QwtBarCurve.h \
               qtiplot/src/plot2D/QwtErrorPlotCurve.h \
               qtiplot/src/plot2D/QwtHistogram.h \
               qtiplot/src/plot2D/QwtPieCurve.h \
               qtiplot/src/plot2D/RangeSelectorTool.h \
               qtiplot/src/plot2D/RectangleWidget.h \
               qtiplot/src/plot2D/ScaleDraw.h \
               qtiplot/src/plot2D/ScaleEngine.h \
               qtiplot/src/plot2D/ScalePicker.h \
               qtiplot/src/plot2D/ScreenPickerTool.h \
               qtiplot/src/plot2D/SelectionMoveResizer.h \
               qtiplot/src/plot2D/Spectrogram.h \
               qtiplot/src/plot2D/TexWidget.h \
               qtiplot/src/plot2D/TitlePicker.h \
               qtiplot/src/plot2D/TranslateCurveTool.h \
               qtiplot/src/plot2D/VectorCurve.h \

    SOURCES += qtiplot/src/plot2D/AddWidgetTool.cpp \
               qtiplot/src/plot2D/ArrowMarker.cpp \
               qtiplot/src/plot2D/BoxCurve.cpp \
               qtiplot/src/plot2D/CanvasPicker.cpp \
               qtiplot/src/plot2D/DataPickerTool.cpp \
               qtiplot/src/plot2D/FrameWidget.cpp \
               qtiplot/src/plot2D/FunctionCurve.cpp \
               qtiplot/src/plot2D/Graph.cpp \
               qtiplot/src/plot2D/Grid.cpp \
               qtiplot/src/plot2D/ImageWidget.cpp \
               qtiplot/src/plot2D/LegendWidget.cpp \
               qtiplot/src/plot2D/LineProfileTool.cpp \
               qtiplot/src/plot2D/MultiLayer.cpp \
               qtiplot/src/plot2D/MultiPeakFitTool.cpp \
               qtiplot/src/plot2D/PlotCurve.cpp \
               qtiplot/src/plot2D/QwtBarCurve.cpp \
               qtiplot/src/plot2D/QwtErrorPlotCurve.cpp \
               qtiplot/src/plot2D/QwtHistogram.cpp \
               qtiplot/src/plot2D/QwtPieCurve.cpp \
               qtiplot/src/plot2D/RangeSelectorTool.cpp \
               qtiplot/src/plot2D/RectangleWidget.cpp \
               qtiplot/src/plot2D/ScaleDraw.cpp \
               qtiplot/src/plot2D/ScaleEngine.cpp \
               qtiplot/src/plot2D/ScalePicker.cpp \
               qtiplot/src/plot2D/ScreenPickerTool.cpp \
               qtiplot/src/plot2D/SelectionMoveResizer.cpp \
               qtiplot/src/plot2D/Spectrogram.cpp \
               qtiplot/src/plot2D/TexWidget.cpp \
               qtiplot/src/plot2D/TitlePicker.cpp \
               qtiplot/src/plot2D/TranslateCurveTool.cpp \
               qtiplot/src/plot2D/VectorCurve.cpp \

    HEADERS += qtiplot/src/plot2D/dialogs/AssociationsDialog.h \
               qtiplot/src/plot2D/dialogs/AxesDialog.h \
               qtiplot/src/plot2D/dialogs/CurvesDialog.h \
               qtiplot/src/plot2D/dialogs/CurveRangeDialog.h \
               qtiplot/src/plot2D/dialogs/EnrichmentDialog.h \
               qtiplot/src/plot2D/dialogs/ErrDialog.h \
               qtiplot/src/plot2D/dialogs/FunctionDialog.h \
               qtiplot/src/plot2D/dialogs/ImageExportDialog.h \
               qtiplot/src/plot2D/dialogs/LayerDialog.h \
               qtiplot/src/plot2D/dialogs/LineDialog.h \
               qtiplot/src/plot2D/dialogs/PlotDialog.h \
               qtiplot/src/plot2D/dialogs/TextDialog.h \
               qtiplot/src/plot2D/dialogs/TextEditor.h \

    SOURCES += qtiplot/src/plot2D/dialogs/AssociationsDialog.cpp \
               qtiplot/src/plot2D/dialogs/AxesDialog.cpp \
               qtiplot/src/plot2D/dialogs/CurvesDialog.cpp \
               qtiplot/src/plot2D/dialogs/CurveRangeDialog.cpp \
               qtiplot/src/plot2D/dialogs/EnrichmentDialog.cpp \
               qtiplot/src/plot2D/dialogs/ErrDialog.cpp \
               qtiplot/src/plot2D/dialogs/FunctionDialog.cpp \
               qtiplot/src/plot2D/dialogs/ImageExportDialog.cpp \
               qtiplot/src/plot2D/dialogs/LayerDialog.cpp \
               qtiplot/src/plot2D/dialogs/LineDialog.cpp \
               qtiplot/src/plot2D/dialogs/PlotDialog.cpp \
               qtiplot/src/plot2D/dialogs/TextDialog.cpp \
               qtiplot/src/plot2D/dialogs/TextEditor.cpp \

###############################################################
################# Module: Plot 3D #############################
###############################################################

    HEADERS += qtiplot/src/plot3D/Bar.h \
               qtiplot/src/plot3D/Cone3D.h \
               qtiplot/src/plot3D/Graph3D.h \
               qtiplot/src/plot3D/Plot3DDialog.h \
               qtiplot/src/plot3D/SurfaceDialog.h \

    SOURCES += qtiplot/src/plot3D/Bar.cpp \
               qtiplot/src/plot3D/Cone3D.cpp \
               qtiplot/src/plot3D/Graph3D.cpp \
               qtiplot/src/plot3D/Plot3DDialog.cpp \
               qtiplot/src/plot3D/SurfaceDialog.cpp \

###############################################################
################# Module: Analysis ##############################
###############################################################

    HEADERS += qtiplot/src/analysis/Convolution.h \
               qtiplot/src/analysis/Correlation.h \
               qtiplot/src/analysis/Differentiation.h \
               qtiplot/src/analysis/ExponentialFit.h \
               qtiplot/src/analysis/FFTFilter.h \
               qtiplot/src/analysis/FFT.h \
               qtiplot/src/analysis/Filter.h \
               qtiplot/src/analysis/Fit.h \
               qtiplot/src/analysis/FitModelHandler.h \
               qtiplot/src/analysis/Integration.h \
               qtiplot/src/analysis/Interpolation.h \
               qtiplot/src/analysis/LogisticFit.h \
               qtiplot/src/analysis/MultiPeakFit.h \
               qtiplot/src/analysis/NonLinearFit.h \
               qtiplot/src/analysis/PluginFit.h \
               qtiplot/src/analysis/PolynomialFit.h \
               qtiplot/src/analysis/SigmoidalFit.h \
               qtiplot/src/analysis/SmoothFilter.h \
               qtiplot/src/analysis/fft2D.h \
               qtiplot/src/analysis/fit_gsl.h \
               qtiplot/src/analysis/nrutil.h \

    SOURCES += qtiplot/src/analysis/Convolution.cpp \
               qtiplot/src/analysis/Correlation.cpp \
               qtiplot/src/analysis/Differentiation.cpp \
               qtiplot/src/analysis/ExponentialFit.cpp \
               qtiplot/src/analysis/FFTFilter.cpp \
               qtiplot/src/analysis/FFT.cpp \
               qtiplot/src/analysis/Filter.cpp \
               qtiplot/src/analysis/Fit.cpp \
               qtiplot/src/analysis/FitModelHandler.cpp \
               qtiplot/src/analysis/Integration.cpp \
               qtiplot/src/analysis/Interpolation.cpp \
               qtiplot/src/analysis/LogisticFit.cpp \
               qtiplot/src/analysis/MultiPeakFit.cpp \
               qtiplot/src/analysis/NonLinearFit.cpp \
               qtiplot/src/analysis/PluginFit.cpp \
               qtiplot/src/analysis/PolynomialFit.cpp \
               qtiplot/src/analysis/SigmoidalFit.cpp \
               qtiplot/src/analysis/SmoothFilter.cpp \
               qtiplot/src/analysis/fft2D.cpp \
               qtiplot/src/analysis/fit_gsl.cpp \
               qtiplot/src/analysis/nrutil.cpp \

    HEADERS += qtiplot/src/analysis/dialogs/ExpDecayDialog.h \
               qtiplot/src/analysis/dialogs/FFTDialog.h \
               qtiplot/src/analysis/dialogs/FitDialog.h \
               qtiplot/src/analysis/dialogs/FilterDialog.h \
               qtiplot/src/analysis/dialogs/FrequencyCountDialog.h \
               qtiplot/src/analysis/dialogs/IntDialog.h \
               qtiplot/src/analysis/dialogs/InterpolationDialog.h \
               qtiplot/src/analysis/dialogs/PolynomFitDialog.h \
               qtiplot/src/analysis/dialogs/SmoothCurveDialog.h \

    SOURCES += qtiplot/src/analysis/dialogs/ExpDecayDialog.cpp \
               qtiplot/src/analysis/dialogs/FFTDialog.cpp \
               qtiplot/src/analysis/dialogs/FitDialog.cpp \
               qtiplot/src/analysis/dialogs/FilterDialog.cpp \
               qtiplot/src/analysis/dialogs/FrequencyCountDialog.cpp \
               qtiplot/src/analysis/dialogs/IntDialog.cpp \
               qtiplot/src/analysis/dialogs/InterpolationDialog.cpp \
               qtiplot/src/analysis/dialogs/PolynomFitDialog.cpp \
               qtiplot/src/analysis/dialogs/SmoothCurveDialog.cpp \

###############################################################
################# Restricted Module: EmfEngine ################
###############################################################

contains(RESTRICTED_MODULES, EMF) {
	DEFINES += EMF_OUTPUT

    INCLUDEPATH += $$PWD/3rdparty/libEMF/include
    unix:LIBS += -L$$PWD/3rdparty/libEMF/lib
	win32:LIBS += -lgdi32

    INCLUDEPATH += $$PWD/3rdparty/EmfEngine
    HEADERS += $$PWD/3rdparty/EmfEngine/EmfEngine.h
    SOURCES += $$PWD/3rdparty/EmfEngine/EmfEngine.cpp
}

###############################################################
##################### SCRIPTING LANGUAGES SECTION #############
###############################################################

##################### Default: muParser v1.28 #################

contains(SCRIPTING_LANGS, muParser) {
  DEFINES += SCRIPTING_MUPARSER

  HEADERS += qtiplot/src/muParserScript.h \
             qtiplot/src/muParserScripting.h \
             3rdparty/muparser/include/muParserDLL.h \
             3rdparty/muparser/include/muParser.h \
             3rdparty/muparser/include/muParserBase.h \
             3rdparty/muparser/include/muParserBytecode.h \
             3rdparty/muparser/include/muParserCallback.h \
             3rdparty/muparser/include/muParserDef.h \
             3rdparty/muparser/include/muParserError.h \
             3rdparty/muparser/include/muParserFixes.h \
             3rdparty/muparser/include/muParserStack.h \
             3rdparty/muparser/include/muParserToken.h \
             3rdparty/muparser/include/muParserTokenReader.h

  SOURCES += qtiplot/src/muParserScript.cpp \
             qtiplot/src/muParserScripting.cpp \
             3rdparty/muparser/include/muParser.cpp \
             3rdparty/muparser/include/muParserBase.cpp \
             3rdparty/muparser/include/muParserBytecode.cpp \
             3rdparty/muparser/include/muParserCallback.cpp \
             3rdparty/muparser/include/muParserError.cpp \
             3rdparty/muparser/include/muParserTokenReader.cpp
}

