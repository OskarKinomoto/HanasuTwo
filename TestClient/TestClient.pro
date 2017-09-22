TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp Common/Protocol/Header.pb.cc
HEADERS += Common/Protocol/*.pb.h


LIBS += -L/usr/include/boost -lboost_system -lboost_log -pthread -lprotobuf
