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

#include "widgetcontextinfo.h"

WidgetContextInfo::WidgetContextInfo(QObject *parent) :
    QObject(parent), m_size(Medium)
{
}

WidgetContextInfo * WidgetContextInfo::create(WidgetSize size, QObject *parent)
{
    WidgetContextInfo *widgetContextInfo = new WidgetContextInfo(parent);
    widgetContextInfo->m_size = size;
    return widgetContextInfo;
}

WidgetContextInfo::WidgetSize WidgetContextInfo::size() const
{
    return m_size;
}

void WidgetContextInfo::setSize(WidgetSize size)
{
    if (m_size != size) {
        m_size = size;
        emit sizeChanged();
    }
}

QVariantMap WidgetContextInfo::settings() const
{
    return m_settings;
}

void WidgetContextInfo::setSettings(const QVariantMap &settings)
{
    if (m_settings != settings) {
        m_settings = settings;
        emit settingsChanged();
    }
}

QVariantMap WidgetContextInfo::properties() const
{
    return m_properties;
}

void WidgetContextInfo::setProperties(const QVariantMap &properties)
{
    if (m_properties != properties) {
        m_properties = properties;
        emit propertiesChanged();
    }
}
