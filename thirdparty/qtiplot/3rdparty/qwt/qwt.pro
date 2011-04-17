# -*- mode: sh -*- ###########################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
##############################################

include( qwtconfig.pri )

TEMPLATE = subdirs
CONFIG   += ordered

SUBDIRS = \
    src \
    textengines

contains(CONFIG, QwtDesigner ) {
    SUBDIRS += designer 
}

contains(CONFIG, QwtExamples ) {
    SUBDIRS += examples 
}

macx{
     # Guillermo: we use LLVM in MAC OS X instead of gcc (2X fast compilation)
     QMAKE_CC = llvm-gcc -arch i386
     QMAKE_CXX = llvm-g++ -arch i386
}
