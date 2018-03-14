#-------------------------------------------------
#
# Project created by QtCreator 2017-09-10T10:40:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    HmiXtp.cpp \
    eptables.cpp

HEADERS  += mainwindow.h \
    HmiXtp.h

FORMS    += mainwindow.ui \
    ui/classicStyle.ui \
    ui/classicStyle_rot.ui \
    ui/confirmation.ui \
    ui/confirmYesNo.ui \
    ui/examination.ui \
    ui/keyboard.ui \
    ui/password.ui \
    ui/s3pMain.ui \
    ui/S3pExpertMode.ui \
    ui/S3pSimpleMode.ui

RESOURCES += \
    icon.qrc

INCLUDEPATH += $$PWD/../inc


INCLUDEPATH += $$PWD/../IDCLibs
DEPENDPATH += $$PWD/../IDCLibs


################### IDCXTP Lib #####################
unix:!macx: LIBS += -L$$PWD/../IDCLibs/IDCXTP/build/ -lXTPClientIf


unix:!macx: PRE_TARGETDEPS += $$PWD/../IDCLibs/IDCXTP/build/libXTPClientIf.a

################### IDCBase Lib #####################

unix:!macx: LIBS += -L$$PWD/../IDCLibs/IDCBase/build/ -lIDCBase

unix:!macx: PRE_TARGETDEPS += $$PWD/../IDCLibs/IDCBase/build/libIDCBase.a

################### IDCLog Lib #####################
unix:!macx: LIBS += -L$$PWD/../IDCLibs/IDCLog/build/ -lIDCLog

unix:!macx: PRE_TARGETDEPS += $$PWD/../IDCLibs/IDCLog/build/libIDCLog.a


unix:!macx: LIBS += -lrt

unix:!macx: LIBS += -lpthread
