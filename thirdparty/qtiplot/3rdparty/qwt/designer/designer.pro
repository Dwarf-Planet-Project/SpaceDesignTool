# -*- mode: sh -*- ###########################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
##############################################

QWT_ROOT = ..

#include ( $${QWT_ROOT}/qwtconfig.pri )

contains(CONFIG, QwtDesigner) {

	CONFIG    += warn_on

	SUFFIX_STR =

    VVERSION = $$[QT_VERSION]
    isEmpty(VVERSION) {

        # Qt 3
        debug {
            SUFFIX_STR = $${DEBUG_SUFFIX}
        }
        else {
            SUFFIX_STR = $${RELEASE_SUFFIX}
        }
    }
    else {

        CONFIG(debug, debug|release) {
            SUFFIX_STR = $${DEBUG_SUFFIX}
        }
        else {
            SUFFIX_STR = $${RELEASE_SUFFIX}
        }
    }

	TEMPLATE        = lib
	MOC_DIR         = moc
	OBJECTS_DIR     = obj$${SUFFIX_STR}
	DESTDIR         = plugins/designer
	INCLUDEPATH    += $${QWT_ROOT}/src 
	DEPENDPATH     += $${QWT_ROOT}/src 

    LIBNAME         = qwt$${SUFFIX_STR}
	contains(CONFIG, QwtDll) {
		win32 {
			DEFINES += QT_DLL QWT_DLL
			LIBNAME = $${LIBNAME}$${VER_MAJ}
		}
	}

	!contains(CONFIG, QwtPlot) {
		DEFINES += NO_QWT_PLOT
	}

	!contains(CONFIG, QwtWidgets) {
		DEFINES += NO_QWT_WIDGETS
	}

	unix:LIBS      += -L$${QWT_ROOT}/lib -l$${LIBNAME}
	win32-msvc:LIBS  += $${QWT_ROOT}/lib/$${LIBNAME}.lib
	win32-msvc.net:LIBS  += $${QWT_ROOT}/lib/$${LIBNAME}.lib
	win32-msvc2002:LIBS += $${QWT_ROOT}/lib/$${LIBNAME}.lib
	win32-msvc2003:LIBS += $${QWT_ROOT}/lib/$${LIBNAME}.lib
	win32-msvc2005:LIBS += $${QWT_ROOT}/lib/$${LIBNAME}.lib
	win32-msvc2008:LIBS += $${QWT_ROOT}/lib/$${LIBNAME}.lib
	win32-g++:LIBS   += -L$${QWT_ROOT}/lib -l$${LIBNAME}

	# isEmpty(QT_VERSION) does not work with Qt-4.1.0/MinGW

	VVERSION = $$[QT_VERSION]
	isEmpty(VVERSION) {
		# Qt 3 
		TARGET    = qwtplugin$${SUFFIX_STR}
		CONFIG   += qt plugin

		UI_DIR    = ui

		HEADERS  += qwtplugin.h
		SOURCES  += qwtplugin.cpp

		target.path = $(QTDIR)/plugins/designer
		INSTALLS += target

		IMAGES  += \
			pixmaps/qwtplot.png \
			pixmaps/qwtanalogclock.png \
			pixmaps/qwtcounter.png \
			pixmaps/qwtcompass.png \
			pixmaps/qwtdial.png \
			pixmaps/qwtknob.png \
			pixmaps/qwtscale.png \
			pixmaps/qwtslider.png \
			pixmaps/qwtthermo.png \
			pixmaps/qwtwheel.png \
			pixmaps/qwtwidget.png 

	} else {

		# Qt 4

		TARGET    = qwt_designer_plugin$${SUFFIX_STR}
		CONFIG    += qt designer plugin 

		RCC_DIR   = resources

		HEADERS += \
			qwt_designer_plugin.h

		SOURCES += \
			qwt_designer_plugin.cpp

	    contains(CONFIG, QwtPlot) {

			HEADERS += \
				qwt_designer_plotdialog.h

			SOURCES += \
				qwt_designer_plotdialog.cpp
		}

		RESOURCES += \
			qwt_designer_plugin.qrc

		target.path = $$[QT_INSTALL_PLUGINS]/designer
		INSTALLS += target
	}
}
else {
	TEMPLATE        = subdirs # do nothing
}


######################################################################
# Install paths
######################################################################

VER_MAJ      = 5
VER_MIN      = 2
VER_PAT      = 2
VERSION      = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}

unix {
    INSTALLBASE    = /usr/local/qwt-$$VERSION-svn
}

win32 {
    INSTALLBASE    = C:/Qwt-$$VERSION-svn
}

target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include
doc.path       = $$INSTALLBASE/doc

######################################################################
# qmake internal options
######################################################################

CONFIG           += qt     # Also for Qtopia Core!
CONFIG           += warn_on
CONFIG           += thread
CONFIG           += silent

######################################################################
# release/debug mode
# If you want to build both DEBUG_SUFFIX and RELEASE_SUFFIX
# have to differ to avoid, that they overwrite each other.
######################################################################

VVERSION = $$[QT_VERSION]
isEmpty(VVERSION) {

    # Qt 3
    CONFIG           += release     # release/debug
}
else {
    # Qt 4
    win32 {
        # On Windows you can't mix release and debug libraries.
        # The designer is built in release mode. If you like to use it
        # you need a release version. For your own application development you
        # might need a debug version.
        # Enable debug_and_release + build_all if you want to build both.

        CONFIG           += release     # release/debug/debug_and_release
        #CONFIG           += debug_and_release
        #CONFIG           += build_all
    }
    else {
        CONFIG           += release     # release/debug
    }
}

######################################################################
# If you want to have different names for the debug and release
# versions you can add a suffix rule below.
######################################################################

DEBUG_SUFFIX        =
RELEASE_SUFFIX      =

win32 {
    DEBUG_SUFFIX      = d
}

######################################################################
# Build the static/shared libraries.
# If QwtDll is enabled, a shared library is built, otherwise
# it will be a static library.
######################################################################

CONFIG           += QwtDll

######################################################################
# QwtPlot enables all classes, that are needed to use the QwtPlot
# widget.
######################################################################

CONFIG       += QwtPlot

######################################################################
# QwtWidgets enables all classes, that are needed to use the all other
# widgets (sliders, dials, ...), beside QwtPlot.
######################################################################

CONFIG     += QwtWidgets

######################################################################
# If you want to display svg imageson the plot canvas, enable the
# line below. Note that Qwt needs the svg+xml, when enabling
# QwtSVGItem.
######################################################################

CONFIG     += QwtSVGItem

######################################################################
# You can use the MathML renderer of the Qt solutions package to
# enable MathML support in Qwt.  # If you want this, copy
# qtmmlwidget.h + qtmmlwidget.cpp to # textengines/mathml and enable
# the line below.
######################################################################

CONFIG     += QwtMathML

######################################################################
# If you want to build the Qwt designer plugin,
# enable the line below.
# Otherwise you have to build it from the designer directory.
######################################################################

CONFIG     += QwtDesigner

######################################################################
# If you want to auto build the examples, enable the line below
# Otherwise you have to build them from the examples directory.
######################################################################

#CONFIG     += QwtExamples


macx {
     # Guillermo: we use LLVM in MAC OS X instead of gcc (2X fast compilation)
     QMAKE_CC = llvm-gcc -arch i386
     QMAKE_CXX = llvm-g++ -arch i386
     DESTDIR = ../../lib/mac
}
win32 {
     DESTDIR = ../../lib/win32-x86-gcc
}
linux {
     DESTDIR = ../../lib/linux-x86
}
