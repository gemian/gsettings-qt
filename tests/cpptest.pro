TEMPLATE = app
QT += testlib
QT -= gui
CONFIG += testcase link_pkgconfig
TARGET = cpptest
IMPORTPATH = $$PWD/..
SOURCES = cpptest.cpp
PKGCONFIG += gsettings-qt

schema.target = gschemas.compiled
schema.commands = glib-compile-schemas $$PWD
schema.depends = com.canonical.gsettings.test.gschema.xml
QMAKE_EXTRA_TARGETS += schema
PRE_TARGETDEPS = gschemas.compiled

# qmake prepends this to the command line executed by `make check`
check.commands = QT_QPA_PLATFORM=minimal LD_LIBRARY_PATH=$$PWD/../src GSETTINGS_BACKEND=memory GSETTINGS_SCHEMA_DIR=$$PWD
