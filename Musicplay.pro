QT += widgets multimedia network sql

TARGET              = MusicPlay
TEMPLATE            = app
MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj
DESTDIR             = bin

include             ($$PWD/CloudMusic/CloudMusic.pri)
include             ($$PWD/DataBase/DataBase.pri)

INCLUDEPATH         += $$PWD/CloudMusic
INCLUDEPATH         += $$PWD/DataBase

INCLUDEPATH         += $$PWD/taglib2.0.1/include
LIBS                += -L$$PWD/taglib2.0.1/lib -ltag -ltag_c


FORMS += \

HEADERS += \

SOURCES += \
    main.cpp \

RESOURCES += \
    music.qrc
