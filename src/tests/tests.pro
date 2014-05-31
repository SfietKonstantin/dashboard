TEMPLATE = app

TARGET = dashboard-test

QT = core gui qml quick

HEADERS += \
    ../qml/widgetcontextinfo.h \
    ../qml/widgetfactory.h \
    ../qml/widgetlistmodel.h \
    ../qml/installedwidgetlistmodel.h

SOURCES += \
    main.cpp \
    ../qml/widgetcontextinfo.cpp \
    ../qml/widgetfactory.cpp \
    ../qml/widgetlistmodel.cpp \
    ../qml/installedwidgetlistmodel.cpp

RESOURCES += \
    res.qrc

OTHER_FILES += \
    main.qml \
    WidgetContainer.qml \
    widget.qml \
    widget.json


system($$PWD/copy.sh)
