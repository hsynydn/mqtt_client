QT  += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += "C:/Users/cehsy/Documents/metosoft/source-code/MetosoftClientApp/paho-mqtt3c.dll"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    msoft/dbManagement.cpp \
    msoft/json.cpp

HEADERS += \
    mainwindow.h \
    msoft/dbManagement.h \
    msoft/customTypes.h \
    msoft/dbManagement.h \
    msoft/json.h \
    paho/MQTTAsync.h \
    paho/MQTTClient.h \
    paho/MQTTClientPersistence.h \
    paho/MQTTProperties.h \
    paho/MQTTReasonCodes.h \
    paho/MQTTSubscribeOpts.h

FORMS += \
    mainwindow.ui
