TEMPLATE = subdirs

SUBDIRS += \
    vesta \
    STAapp

STAapp.file = ./STAapp.pro
STAapp.depends = vesta
vesta.subdir = thirdparty/vesta
