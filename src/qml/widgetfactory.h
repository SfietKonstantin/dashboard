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

#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

#include <QtCore/QObject>

class QJsonObject;
class QUrl;
class QQmlComponent;
class QQmlEngine;
class WidgetContextInfo;
struct WidgetFactoryContainer;
class WidgetFactoryPrivate;
class WidgetFactory : public QObject
{
    Q_OBJECT
public:
    explicit WidgetFactory(QQmlEngine *engine, QObject *parent = 0);
    virtual ~WidgetFactory();
    const QJsonObject & widgetDescriptionJson() const;
    QString widgetName() const;
    QString widgetDescription() const;
    QUrl widgetSource() const;
    WidgetContextInfo * createWidgetContext(QObject *parent = 0) const;
    bool readSource(const QString &source);
    void createWidget(const QUrl &url, WidgetContextInfo *widgetContextInfo, QObject *parent = 0);
signals:
    void widgetCreated(WidgetContextInfo *widgetContextInfo, QObject *widget);
protected:
    QScopedPointer<WidgetFactoryPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(WidgetFactory)
};

#endif // WIDGETFACTORY_H
