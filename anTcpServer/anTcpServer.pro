QT -= gui
QT += network

CONFIG += c++11
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    anthreadpool.cpp \
    antcpserver.cpp \
    antcpsocket.cpp

INCLUDEPATH += $$PWD/deps/include
INCLUDEPATH += $$PWD/../antlv

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    anthreadpool.h \
    antcpserver.h \
    antcpsocket.h

win32:VERSION = 1.0.0.1038  # major.minor.patch.build
else:VERSION = 1.0.0        # major.minor.patch

