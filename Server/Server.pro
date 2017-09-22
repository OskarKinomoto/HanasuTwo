TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp Common/Protocol/Header.pb.cc \
    users.cpp \
    sql.cpp \
    config.cpp
HEADERS += Common/Protocol/*.pb.h \
    users.h \
    sql.h \
    config.h \
    types.hpp

LIBS += -L/usr/include/boost -lboost_system -lboost_log -pthread -lprotobuf -lboost_locale -lsqlite3 -lboost_program_options -lstdc++fs
