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

#ifndef WIDGETCONTEXTINFO_H
#define WIDGETCONTEXTINFO_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>

class WidgetContextInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WidgetSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QVariantMap settings READ settings NOTIFY propertiesChanged)
    Q_PROPERTY(QVariantMap properties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_ENUMS(WidgetSize)
public:
    enum WidgetSize
    {
        Small,
        Medium,
        Large
    };
    explicit WidgetContextInfo(QObject *parent = 0);
    static WidgetContextInfo * create(WidgetSize size, QObject *parent = 0);
    WidgetSize size() const;
    void setSize(WidgetSize size);
    QVariantMap settings() const;
    void setSettings(const QVariantMap &settings);
    QVariantMap properties() const;
    void setProperties(const QVariantMap &properties);
Q_SIGNALS:
    void sizeChanged();
    void settingsChanged();
    void propertiesChanged();
private:
    WidgetSize m_size;
    QVariantMap m_settings;
    QVariantMap m_properties;
};

#endif // WIDGETCONTEXTINFO_H
