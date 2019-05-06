#-------------------------------------------------
#
# Project created by QtCreator 2018-12-19T15:46:05
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chat
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "C:/Users/29069/Downloads/Compressed/include/include"


SOURCES += \
        main.cpp \
        loginwindow.cpp \
    titlebar.cpp \
    accountitem.cpp \
    groupitem.cpp \
    mainwindow.cpp \
    personalinfo.cpp \
    message.cpp \
    groupiteminfo.cpp \
    global.cpp \
    chatwindow.cpp \
    videowindow.cpp \
    udprecvicer.cpp \
    audioplayer.cpp \
    chatwindowmessageitem.cpp \
    messagewidget.cpp \
    usersgroupitem.cpp \
    usersgroupinfo.cpp \
    deleteusersgroupmemberwin.cpp \
    profileuploadwin.cpp \
    tcpfilethread.cpp \
    tcpfilerecvicer.cpp \
    filewidgetitem.cpp \
    systemmessagewin.cpp \
    systemmessageitem.cpp \
    registerwin.cpp \
    networkspacewin.cpp \
    groupchatwin.cpp

HEADERS += \
        loginwindow.h \
    titlebar.h \
    accountitem.h \
    mainwindow.h \
    groupitem.h \
    mainwindow.h \
    personalinfo.h \
    message.h \
    groupiteminfo.h \
    global.h \
    messagetype.h \
    chatwindow.h \
    videowindow.h \
    udprecvicer.h \
    concurrentqueue.h \
    audioplayer.h \
    chatwindowmessageitem.h \
    messagewidget.h \
    usersgroupitem.h \
    usersgroupinfo.h \
    deleteusersgroupmemberwin.h \
    profileuploadwin.h \
    tcpfilethread.h \
    tcpfilerecvicer.h \
    filewidgetitem.h \
    systemmessagewin.h \
    systemmessageitem.h \
    registerwin.h \
    networkspacewin.h \
    groupchatwin.h

FORMS += \
        loginwindow.ui \
    personalinfo.ui \
    chatwindow.ui \
    videowindow.ui \
    chatwindowmessageitem.ui \
    usersgroupinfo.ui \
    deleteusersgroupmemberwin.ui \
    profileuploadwin.ui \
    filewidgetitem.ui \
    systemmessagewin.ui \
    systemmessageitem.ui \
    registerwin.ui \
    networkspacewin.ui \
    groupchatwin.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../openCV/opencv/build/x64/vc15/lib/ -lopencv_world401
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../openCV/opencv/build/x64/vc15/lib/ -lopencv_world401d

INCLUDEPATH += $$PWD/../../openCV/opencv/build/include
DEPENDPATH += $$PWD/../../openCV/opencv/build/include

win32: LIBS += -L'C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64/' -lOpenAL32

INCLUDEPATH += 'C:/Program Files (x86)/OpenAL 1.1 SDK/include'
DEPENDPATH += 'C:/Program Files (x86)/OpenAL 1.1 SDK/include'
