QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addjournaldialog.cpp \
    addwalletdialog.cpp \
    application.cpp \
    database.cpp \
    databasefile.cpp \
    datatypes.cpp \
    edittransactiondialog.cpp \
    main.cpp \
    mainwindow.cpp \
    transactiontablemodel.cpp \
    filterwidget.cpp \
    transactiontableview.cpp \
    viewwalletsdialog.cpp

HEADERS += \
    addjournaldialog.h \
    addwalletdialog.h \
    application.h \
    database.h \
    databasefile.h \
    datatypes.h \
    edittransactiondialog.h \
    mainwindow.h \
    transactiontablemodel.h \
    filterwidget.h \
    transactiontableview.h \
    viewwalletsdialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
