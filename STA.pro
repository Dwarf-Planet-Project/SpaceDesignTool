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
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])

CONFIG += debug_and_release
#CONFIG += Release
#CONFIG += warn_off

#target.path =
#INSTALLS += target

TARGET = STA

TEMPLATE = subdirs

SUBDIRS += \
    vesta \
    qtiplot \
    STAapp
#    schema2cpp

STAapp.file = ./STAapp.pro
STAapp.depends = vesta

vesta.subdir = thirdparty/vesta
qtiplot.subdir = thirdparty/qtiplot
#schema2cpp.subdir = tools/schema2cpp

