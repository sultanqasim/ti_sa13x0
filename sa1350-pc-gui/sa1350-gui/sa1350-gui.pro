QT       += core gui xml widgets printsupport


TEMPLATE = app

# ---- Qwt Lib and includes setup -----------
INCLUDEPATH += C:/Qt/qwt-6.1.3/src

SOURCES += main.cpp\
    mainwindow.cpp \
    appGrid.cpp \
    appPlot.cpp \
    appStatusbar.cpp \
    appMarker.cpp \
    appSettings.cpp \
    cEvent.cpp \
    appCurve.cpp \
    appConstData.cpp \
    appReportCsv.cpp \
    appFwUpdater.cpp \
    drvSA1350.cpp

HEADERS += mainwindow.h \
    appPlot.h \
    appStatusbar.h \
    appMarker.h \
    appSettings.h \
    appTypedef.h \
    cEvent.h \
    appCurve.h \
    appReportCsv.h \
    appFwUpdater.h \
    drvSA1350.h \
    appGrid.h

FORMS   += mainwindow.ui \
    appFwUpdater.ui

CONFIG(debug, debug|release){
    LIBS += ../sa1350-dll/debug/libsa1350dbg.a
    LIBS += C:/Qt/qwt-6.1.3/lib/libqwtd.a
    TARGET = SA13x0Dbg
   }else{
    LIBS += ../sa1350-dll/release/libsa1350.a
    LIBS += C:/Qt/qwt-6.1.3/lib/libqwt.a
    TARGET = SA13x0
   }

RESOURCES += \
    Resource.qrc

RC_FILE += \
    Manifest.rc
