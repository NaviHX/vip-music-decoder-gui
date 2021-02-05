TEMPLATE = app
TARGET = vip-music-decoder
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/mainWindow.h \
           src-ncm/aes.h \
           src-ncm/base64.h \
           src-ncm/cJSON.h \
           src-ncm/decoderNCM.hpp \
           src-qmc/decoderQMC.hpp
SOURCES += src/main.cpp src/mainWindow.cpp src-ncm/aes.cpp src-ncm/cJSON.cpp

QT += core gui widgets
LIBS += -ltag
