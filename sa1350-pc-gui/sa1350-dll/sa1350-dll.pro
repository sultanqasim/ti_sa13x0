QT       -= core gui


TEMPLATE  = lib

CONFIG    += warn_on
CONFIG    += thread
CONFIG	  += dll

LIBS      += -lsetupapi

DEFINES += SA1350_EXPORTS
DEFINES -= UNICODE
DEFINES += "WINVER=0x0500"

SOURCES += \
    cUsbDetect.cpp \
    cThread.cpp \
    cRegAccess.cpp \
    cMutex.cpp \
    cEvent.cpp \
    cDriver.cpp \
    cDeviceDriver.cpp \
    dllmain.cpp \
    sa1350.cpp

HEADERS +=\
    cUsbDetect.h \
    cThread.h \
    cRegAccess.h \
    cMutex.h \
    cEvent.h \
    cDriver.h \
    cDeviceDriver.h \
    sa1350.h \
    sa1350TypeDef.h \
    sa1350_global.h \
    sa1350Cmd.h

CONFIG(debug, debug|release){
    TARGET    = sa1350dbg
   }else{
    TARGET    = sa1350
   }
