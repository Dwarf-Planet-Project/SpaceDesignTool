message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])

CONFIG += debug_and_release
#CONFIG += release
CONFIG += warn_off
CONFIG   += ordered
CONFIG += x86

#TARGET = qtiplot
TEMPLATE = subdirs


SUBDIRS += \
          #manual \
          #fitPlugins/fitRational0 \
          #fitPlugins/fitRational1 \
          3rdparty/qwt \
          3rdparty/qwtplot3d \
          qtiplot


qtiplot.file = ./qtiplotapp.pro

QT += qt3support
QT += network
QT += xml xmlpatterns
QT += opengl
QT += svg

CONFIG += assistant
CONFIG += designer

macx {
     # Guillermo: we use LLVM in MAC OS X instead of gcc (2X fast compilation)
     QMAKE_CC = llvm-gcc -arch i386
     QMAKE_CXX = llvm-g++ -arch i386
}
