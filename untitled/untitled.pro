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

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../XTP/release/ -lXTPClientIf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../XTP/debug/ -lXTPClientIf
else:unix: LIBS += -L$$PWD/../XTP/ -lXTPClientIf

INCLUDEPATH += $$PWD/../XTP
DEPENDPATH += $$PWD/../XTP

INCLUDEPATH += $$PWD/..

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../XTP/release/libXTPClientIf.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../XTP/debug/libXTPClientIf.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../XTP/release/XTPClientIf.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../XTP/debug/XTPClientIf.lib
else:unix: PRE_TARGETDEPS += $$PWD/../XTP/libXTPClientIf.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../IDCBaseLib/release/ -lIDCBaseLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../IDCBaseLib/debug/ -lIDCBaseLib
else:unix: LIBS += -L$$PWD/../IDCBaseLib/ -lIDCBaseLib

INCLUDEPATH += $$PWD/../IDCBaseLib
DEPENDPATH += $$PWD/../IDCBaseLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/release/libIDCBaseLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/debug/libIDCBaseLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/release/IDCBaseLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/debug/IDCBaseLib.lib
else:unix: PRE_TARGETDEPS += $$PWD/../IDCBaseLib/libIDCBaseLib.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../IDCBaseLib/release/ -lIDCBaseLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../IDCBaseLib/debug/ -lIDCBaseLib
else:unix: LIBS += -L$$PWD/../IDCBaseLib/ -lIDCBaseLib

INCLUDEPATH += $$PWD/../IDCBaseLib
DEPENDPATH += $$PWD/../IDCBaseLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/release/libIDCBaseLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/debug/libIDCBaseLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/release/IDCBaseLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../IDCBaseLib/debug/IDCBaseLib.lib
else:unix: PRE_TARGETDEPS += $$PWD/../IDCBaseLib/libIDCBaseLib.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../IDCLog/release/ -lIDCLog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../IDCLog/debug/ -lIDCLog
else:unix: LIBS += -L$$PWD/../IDCLog/ -lIDCLog

INCLUDEPATH += $$PWD/../IDCLog
DEPENDPATH += $$PWD/../IDCLog

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../IDCLog/release/libIDCLog.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../IDCLog/debug/libIDCLog.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../IDCLog/release/IDCLog.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../IDCLog/debug/IDCLog.lib
else:unix: PRE_TARGETDEPS += $$PWD/../IDCLog/libIDCLog.a
