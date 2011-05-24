# -*- mode: sh -*- ###########################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
##############################################

# qmake project file for building the qwt libraries

QWT_ROOT = ..

#include( $${QWT_ROOT}/qwtconfig.pri )

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

TARGET            = qwt$${SUFFIX_STR}
TEMPLATE          = lib

MOC_DIR           = moc
OBJECTS_DIR       = obj$${SUFFIX_STR}
DESTDIR           = $${QWT_ROOT}/lib

contains(CONFIG, QwtDll ) {
    CONFIG += dll
} 
else {
    CONFIG += staticlib
}

win32:QwtDll {
    DEFINES    += QT_DLL QWT_DLL QWT_MAKEDLL
}

symbian:QwtDll {
    DEFINES    += QT_DLL QWT_DLL QWT_MAKEDLL
}

HEADERS += \
    qwt.h \
    qwt_abstract_scale_draw.h \
    qwt_array.h \
    qwt_color_map.h \
    qwt_clipper.h \
    qwt_double_interval.h \
    qwt_double_rect.h \
    qwt_dyngrid_layout.h \
    qwt_global.h \
    qwt_layout_metrics.h \
    qwt_math.h \
    qwt_magnifier.h \
    qwt_paint_buffer.h \
    qwt_painter.h \
    qwt_panner.h \
    qwt_picker.h \
    qwt_picker_machine.h \
    qwt_polygon.h \
    qwt_round_scale_draw.h \
    qwt_scale_div.h \
    qwt_scale_draw.h \
    qwt_scale_engine.h \
    qwt_scale_map.h \
    qwt_spline.h \
    qwt_symbol.h \
    qwt_text_engine.h \
    qwt_text_label.h \
    qwt_text.h \
    qwt_valuelist.h

SOURCES += \
    qwt_abstract_scale_draw.cpp \
    qwt_color_map.cpp \
    qwt_clipper.cpp \
    qwt_double_interval.cpp \
    qwt_double_rect.cpp \
    qwt_dyngrid_layout.cpp \
    qwt_layout_metrics.cpp \
    qwt_math.cpp \
    qwt_magnifier.cpp \
    qwt_paint_buffer.cpp \
    qwt_panner.cpp \
    qwt_painter.cpp \
    qwt_picker.cpp \
    qwt_round_scale_draw.cpp \
    qwt_scale_div.cpp \
    qwt_scale_draw.cpp \
    qwt_scale_map.cpp \
    qwt_spline.cpp \
    qwt_text_engine.cpp \
    qwt_text_label.cpp \
    qwt_text.cpp \
    qwt_event_pattern.cpp \
    qwt_picker_machine.cpp \
    qwt_scale_engine.cpp \
    qwt_symbol.cpp

 
contains(CONFIG, QwtPlot) {

    HEADERS += \
        qwt_curve_fitter.h \
        qwt_data.h \
        qwt_event_pattern.h \
        qwt_interval_data.h \
        qwt_legend.h \
        qwt_legend_item.h \
        qwt_legend_itemmanager.h \
        qwt_plot.h \
        qwt_plot_curve.h \
        qwt_plot_dict.h \
        qwt_plot_grid.h \
        qwt_plot_item.h \
        qwt_plot_layout.h \
        qwt_plot_marker.h \
        qwt_plot_printfilter.h \
        qwt_plot_rasteritem.h \
        qwt_plot_spectrogram.h \
        qwt_plot_scaleitem.h \
        qwt_plot_canvas.h \
        qwt_plot_rescaler.h \
        qwt_plot_panner.h \
        qwt_plot_picker.h \
        qwt_plot_zoomer.h \
        qwt_plot_magnifier.h \
        qwt_raster_data.h \
        qwt_scale_widget.h 

    SOURCES += \
        qwt_curve_fitter.cpp \
        qwt_data.cpp \
        qwt_interval_data.cpp \
        qwt_legend.cpp \
        qwt_legend_item.cpp \
        qwt_plot.cpp \
        qwt_plot_print.cpp \
        qwt_plot_xml.cpp \
        qwt_plot_axis.cpp \
        qwt_plot_curve.cpp \
        qwt_plot_dict.cpp \
        qwt_plot_grid.cpp \
        qwt_plot_item.cpp \
        qwt_plot_spectrogram.cpp \
        qwt_plot_scaleitem.cpp \
        qwt_plot_marker.cpp \
        qwt_plot_layout.cpp \
        qwt_plot_printfilter.cpp \
        qwt_plot_rasteritem.cpp \
        qwt_plot_canvas.cpp \
        qwt_plot_rescaler.cpp \
        qwt_plot_panner.cpp \
        qwt_plot_picker.cpp \
        qwt_plot_zoomer.cpp \
        qwt_plot_magnifier.cpp \
        qwt_raster_data.cpp \
        qwt_scale_widget.cpp 
}

contains(CONFIG, QwtSVGItem) {

    QT += svg
    HEADERS += qwt_plot_svgitem.h
    SOURCES += qwt_plot_svgitem.cpp 
}

contains(CONFIG, QwtWidgets) {

    HEADERS += \
        qwt_abstract_slider.h \
        qwt_abstract_scale.h \
        qwt_arrow_button.h \
        qwt_analog_clock.h \
        qwt_compass.h \
        qwt_compass_rose.h \
        qwt_counter.h \
        qwt_dial.h \
        qwt_dial_needle.h \
        qwt_double_range.h \
        qwt_knob.h \
        qwt_slider.h \
        qwt_thermo.h \
        qwt_wheel.h
    
    SOURCES += \
        qwt_abstract_slider.cpp \
        qwt_abstract_scale.cpp \
        qwt_arrow_button.cpp \
        qwt_analog_clock.cpp \
        qwt_compass.cpp \
        qwt_compass_rose.cpp \
        qwt_counter.cpp \
        qwt_dial.cpp \
        qwt_dial_needle.cpp \
        qwt_double_range.cpp \
        qwt_knob.cpp \
        qwt_slider.cpp \
        qwt_thermo.cpp \
        qwt_wheel.cpp
}

# Install directives

headers.files  = $$HEADERS
doc.files      = $${QWT_ROOT}/doc/html $${QWT_ROOT}/doc/qwt-5.2.0.qch
unix {
    doc.files      += $${QWT_ROOT}/doc/man
}

INSTALLS       = target headers doc


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
