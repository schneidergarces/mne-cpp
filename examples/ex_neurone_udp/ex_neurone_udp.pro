TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


QT += network

## tried to add line below to get include to work, did not help
# LIBS += -lQtNetwork
