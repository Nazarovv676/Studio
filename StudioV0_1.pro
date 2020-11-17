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
    Data/dbconnection.cpp \
    Data/log.cpp \
    Data/orderwriter.cpp \
    Data/settings.cpp \
    DonutBreakDownChart/donutbreakdownchart.cpp \
    DonutBreakDownChart/mainslice.cpp \
    ItemDelegate/itemdelegate.cpp \
    ItemDelegate/itemdelegatecheckbox.cpp \
    ItemDelegate/itemdelegatecombobox.cpp \
    ItemDelegate/itemdelegatedate.cpp \
    ItemDelegate/itemdelegatedoublespinbox.cpp \
    ItemDelegate/itemdelegatespinbox.cpp \
    main.cpp \
    loginwindow.cpp \
    mainwindow.cpp \
    settingswindow.cpp \
    signupwindow.cpp \
    statisticwindow.cpp

HEADERS += \
    Data/dbconnection.h \
    Data/log.h \
    Data/orderwriter.h \
    Data/settings.h \
    DonutBreakDownChart/donutbreakdownchart.h \
    DonutBreakDownChart/mainslice.h \
    ItemDelegate/itemdelegate.h \
    ItemDelegate/itemdelegatecheckbox.h \
    ItemDelegate/itemdelegatecombobox.h \
    ItemDelegate/itemdelegatedate.h \
    ItemDelegate/itemdelegatedoublespinbox.h \
    ItemDelegate/itemdelegatespinbox.h \
    loginwindow.h \
    mainwindow.h \
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
