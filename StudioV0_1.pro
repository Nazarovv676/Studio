QT       += core gui sql printsupport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    dbconnection.cpp \
    itemdelegate.cpp \
    itemdelegatecheckbox.cpp \
    itemdelegatecombobox.cpp \
    itemdelegatedate.cpp \
    itemdelegatedoublespinbox.cpp \
    itemdelegatespinbox.cpp \
    log.cpp \
    main.cpp \
    loginwindow.cpp \
    mainwindow.cpp \
    orderwriter.cpp \
    settings.cpp \
    settingswindow.cpp \
    signupwindow.cpp \
    statisticwindow.cpp

HEADERS += \
    dbconnection.h \
    itemdelegate.h \
    itemdelegatecheckbox.h \
    itemdelegatecombobox.h \
    itemdelegatedate.h \
    itemdelegatedoublespinbox.h \
    itemdelegatespinbox.h \
    log.h \
    loginwindow.h \
    mainwindow.h \
    orderwriter.h \
    settings.h \
    settingswindow.h \
    signupwindow.h \
    statisticwindow.h

FORMS += \
    loginwindow.ui \
    mainwindow.ui \
    settingswindow.ui \
    signupwindow.ui \
    statisticwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

DISTFILES += \
    studioDB.sql \
    umldiagram.qmodel \
    umltest.jpeg
