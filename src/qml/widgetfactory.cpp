/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "widgetfactory.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include "widgetcontextinfo.h"

static const char *WIDGET_DESCRIPTION_FILE = "widget.json";
static const char *WIDGET_FILE_NAME = "widget.qml";
static const char *NAME_KEY = "name";
static const char *DESCRIPTION_KEY = "description";
static const char *DEFAULT_SETTINGS_KEY = "default_settings";
static const char *SIZE_KEY = "size";
static const char *SIZE_SMALL = "small";
// static const char *SIZE_MEDIUM = "medium";
static const char *SIZE_LARGE = "large";
// static const char *DEFAULT_PROPERTIES_KEY = "default_properties";

struct WidgetFactoryContainer
{
    WidgetContextInfo *widgetContextInfo;
    QObject *parent;
};

class WidgetFactoryPrivate: public QObject
{
    Q_OBJECT
public:
    explicit WidgetFactoryPrivate(WidgetFactory *q);
    void statusChanged(QQmlComponent::Status status);
    void addWidget(QQmlComponent *component, WidgetContextInfo *widgetContextInfo, QObject *parent);
    QMap<QQmlComponent *, WidgetFactoryContainer> infos;
    QQmlEngine *engine;
    QString source;
    QJsonObject widgetDescription;
protected:
    WidgetFactory * const q_ptr;
private:
    Q_DECLARE_PUBLIC(WidgetFactory)
};

WidgetFactoryPrivate::WidgetFactoryPrivate(WidgetFactory *q)
    : engine(0), q_ptr(q)
{
}

void WidgetFactoryPrivate::statusChanged(QQmlComponent::Status status)
{
    Q_UNUSED(status)
    QQmlComponent *component = qobject_cast<QQmlComponent *>(sender());
    if (!component) {
        return;
    }

    if (!infos.contains(component)) {
        component->deleteLater();
        return;
    }

    const WidgetFactoryContainer &container = infos[component];
    WidgetContextInfo *widgetContextInfo = container.widgetContextInfo;
    QObject *parent = container.parent;
    addWidget(component, widgetContextInfo, parent);
    infos.remove(component);
}

void WidgetFactoryPrivate::addWidget(QQmlComponent *component, WidgetContextInfo *widgetContextInfo,
                                     QObject *parent)
{
    Q_Q(WidgetFactory);
    if (component->status() == QQmlComponent::Error) {
        qWarning() << "Error creating a component" << component->errorString().trimmed().toLocal8Bit().data();
        infos.remove(component);
        component->deleteLater();
        return;
    }

    if (component->status() == QQmlComponent::Ready) {
        QQmlContext *context = new QQmlContext(engine->rootContext(), widgetContextInfo);
        context->setContextProperty("widget", widgetContextInfo);
        QObject *widget = component->beginCreate(context);
        QQuickItem *item = qobject_cast<QQuickItem *>(widget);
        QQuickItem *parentItem = qobject_cast<QQuickItem *>(parent);
        widget->setParent(parent);
        if (item && parentItem) {
            item->setParentItem(parentItem);
        }
        component->completeCreate();

        emit q->widgetCreated(widgetContextInfo, widget);
        component->deleteLater();
    }
}

WidgetFactory::WidgetFactory(QQmlEngine *engine, QObject *parent) :
    QObject(parent), d_ptr(new WidgetFactoryPrivate(this))
{
    Q_D(WidgetFactory);
    d->engine = engine;
}

WidgetFactory::~WidgetFactory()
{
}

const QJsonObject & WidgetFactory::widgetDescriptionJson() const
{
    Q_D(const WidgetFactory);
    return d->widgetDescription;
}

QString WidgetFactory::widgetName() const
{
    Q_D(const WidgetFactory);
    return d->widgetDescription.value(NAME_KEY).toString();
}

QString WidgetFactory::widgetDescription() const
{
    Q_D(const WidgetFactory);
    return d->widgetDescription.value(DESCRIPTION_KEY).toString();
}

QUrl WidgetFactory::widgetSource() const
{
    Q_D(const WidgetFactory);
    if (d->widgetDescription.isEmpty()) {
        return QUrl();
    }

    QDir dir (d->source);
    QString file = dir.absoluteFilePath(WIDGET_FILE_NAME);
    QUrl source;
    if (file.startsWith(":")) {
        file = file.mid(1);
        source = QUrl(QString("qrc:/%1").arg(file));
    } else if (file.startsWith("qrc:")) {
        file = file.mid(4);
        source = QUrl(QString("qrc:/%1").arg(file));
    } else {
        source = QUrl::fromLocalFile(file);
    }
    return source;
}

WidgetContextInfo * WidgetFactory::createWidgetContext(QObject *parent) const
{
    Q_D(const WidgetFactory);
    if (d->widgetDescription.isEmpty()) {
        return 0;
    }

    // Get default size
    QJsonObject defaultSettingsValue = d->widgetDescription.value(DEFAULT_SETTINGS_KEY).toObject();
    QString sizeString = defaultSettingsValue.value(SIZE_KEY).toString();
    WidgetContextInfo::WidgetSize size = WidgetContextInfo::Medium;
    if (sizeString == SIZE_SMALL) {
        size = WidgetContextInfo::Small;
    } else if (sizeString == SIZE_LARGE) {
        size = WidgetContextInfo::Large;
    }

    WidgetContextInfo *context = WidgetContextInfo::create(size, parent);
    return context;
}

bool WidgetFactory::readSource(const QString &source)
{
    Q_D(WidgetFactory);
    d->widgetDescription = QJsonObject();
    QDir subdir (source);

    // Check widget description file inside dir
    QString fileName = subdir.absoluteFilePath(WIDGET_DESCRIPTION_FILE);
    if (!QFile::exists(fileName)) {
        return false;
    }

    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QJsonParseError error;
    QJsonDocument widgetDescriptionDocument = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing widget description file:"
                   << error.errorString().toLocal8Bit().data();
        return false;
    }

    d->source = source;
    d->widgetDescription = widgetDescriptionDocument.object();
    return true;
}

void WidgetFactory::createWidget(const QUrl &url, WidgetContextInfo *widgetContextInfo, QObject *parent)
{
    Q_D(WidgetFactory);
    QQmlComponent *component = new QQmlComponent(d->engine, url, QQmlComponent::Asynchronous, this);
    if (component->status() == QQmlComponent::Ready || component->status() == QQmlComponent::Error) {
        d->addWidget(component, widgetContextInfo, parent);
    } else {
        connect(component, &QQmlComponent::statusChanged, d, &WidgetFactoryPrivate::statusChanged);
        WidgetFactoryContainer container;
        container.widgetContextInfo = widgetContextInfo;
        container.parent = parent;
        d->infos.insert(component, container);
    }
}


#include "widgetfactory.moc"
