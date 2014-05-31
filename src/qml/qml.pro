TARGET = dashboard
PLUGIN_IMPORT_PATH = org/SfietKonstantin/dashboard

TEMPLATE = lib
CONFIG += qt plugin hide_symbols

QT = core gui qml quick

HEADERS += \
    widgetcontextinfo.h \
    widgetfactory.h \
    widgetlistmodel.h \
    installedwidgetlistmodel.h

SOURCES += \
    plugin.cpp \
    widgetcontextinfo.cpp \
    widgetfactory.cpp \
    widgetlistmodel.cpp \
    installedwidgetlistmodel.cpp

OTHER_FILES += \
    qmldir \
    WidgetContainer.qml

target.path = $$[QT_INSTALL_QML]/$$PLUGIN_IMPORT_PATH
INSTALLS += target

qml.files += $$OTHER_FILES
qml.path +=  $$target.path
INSTALLS += qml
