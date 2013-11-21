TARGET = double_color_lottery

QT += sql widgets testlib

QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           mymodel.cpp \
    newresultwindow.cpp \
    deleteresultwindow.cpp \
    lotterydockwidget.cpp \
    dataimporter.cpp \
    dataimporterproxy.cpp \
    dcltest.cpp \
    databasewizard.cpp

HEADERS += mainwindow.h \
           mymodel.h \
    newresultwindow.h \
    deleteresultwindow.h \
    lotterydockwidget.h \
    dataimporter.h \
    dataimporterproxy.h \
    dcltest.h \
    databasewizard.h

# install
target.path = .
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS dcl.pro
sources.path = .
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
