QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/libftdi/
INCLUDEPATH += $$PWD/LibUSB/

SOURCES += \
    libftdi/ftdi.c \
    main.cpp \
    Dump/chunks.cpp \
    Dump/commands.cpp \
    Dump/qhexedit.cpp \
    dialog.cpp

HEADERS += \
    dialog.h \
    Dump/chunks.h \
    Dump/commands.h \
    Dump/qhexedit.h \
    libftdi/ftdi.h \
    libftdi/ftdi_i.h \
    libftdi/ftdi_version_i.h \
    libftdi/libusb.h

FORMS += \
    dialog.ui

win32:{
LIBS +=  $$PWD/LibUSB/MinGW64/static/libusb-1.0.a
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
