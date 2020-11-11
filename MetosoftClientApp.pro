QT  += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += "C:/Users/cehsy/Documents/metosoft/source-code/MetosoftClientApp/paho-mqtt3c.dll"

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    MQTTAsync.h \
    MQTTClient.h \
    MQTTClientPersistence.h \
    MQTTProperties.h \
    MQTTReasonCodes.h \
    MQTTSubscribeOpts.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
