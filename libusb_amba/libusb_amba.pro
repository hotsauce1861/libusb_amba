TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = run

DESTDIR = $$PWD/build/bin
MOC_DIR = $$PWD/build/moc

SOURCES += main.cpp \
    amba_usb.cpp \
    amba_param.cpp \
    amba_pktfmt.cpp

INCLUDEPATH += $$PWD/../lib/include/libusb-1.0

LIBS += -L$$PWD/../lib/MinGW32/dll/ \
        -lusb-1.0


message("include path is $$INCLUDEPATH")

HEADERS += \
    amba_usb.h \
    amba_param.h \
    amba_pktfmt.h

