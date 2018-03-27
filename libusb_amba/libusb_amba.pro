TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    amba_usb.cpp

INCLUDEPATH += $$PWD/../lib/include/libusb-1.0

LIBS += -L$$PWD/../lib/MinGW32/dll/ \
        -llibusb-1.0


message("include path is $$INCLUDEPATH")

HEADERS += \
    amba_usb.h

