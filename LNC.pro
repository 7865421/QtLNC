QT       += core gui sql axcontainer network serialport network ftp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
RC_ICONS=LNC.ICO
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dboperate.cpp \
    lib/libmodbus/modbus-data.c \
    lib/libmodbus/modbus-rtu.c \
    lib/libmodbus/modbus-tcp.c \
    lib/libmodbus/modbus.c \
    main.cpp \
    mainwindow.cpp \
    polling.cpp \
    src/excel/ExcelBase.cpp \
    sysinit.cpp \
    windowevent.cpp \
    analysethread.cpp \
    analyse.cpp \
    saveexcelthead.cpp \
    singleapp.cpp \
    keycapturer.cpp \
    keymonitor.cpp \
    paramsave.cpp \
    scanserial.cpp \
    postreq.cpp \
    saveascsv.cpp \
    ftpclient.cpp \
    autosavecsv.cpp

HEADERS += \
    lib/libmodbus/config.h \
    lib/libmodbus/modbus-private.h \
    lib/libmodbus/modbus-rtu-private.h \
    lib/libmodbus/modbus-rtu.h \
    lib/libmodbus/modbus-tcp-private.h \
    lib/libmodbus/modbus-tcp.h \
    lib/libmodbus/modbus-version.h \
    lib/libmodbus/modbus.h \
    mainwindow.h \
    src/excel/ExcelBase.h \
    lib/lnc2/scif2.h \
    lib/lnc2/scif2_define.h \
    analysethread.h \
    saveexcelthead.h \
    singleapp.h \
    keycapturer.h \
    keymonitor.h \
    saveascsv.h \
    qreplytimeout.h

FORMS += \
    mainwindow.ui

DEFINES += __QT
INCLUDEPATH += $$PWD/lib/lnc2
INCLUDEPATH += $$PWD/lib
INCLUDEPATH += $$PWD/src

win32:LIBS += -lws2_32
win32:LIBS += -L$$PWD/lib/lnc2 -lsc2_qt


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
