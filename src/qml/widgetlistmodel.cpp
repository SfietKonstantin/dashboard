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

#include "widgetlistmodel.h"
#include "widgetcontextinfo.h"
#include "widgetfactory.h"
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

struct WidgetListModelItem
{
    virtual ~WidgetListModelItem();
    QUrl source;
    WidgetContextInfo *contextInfo;
};

WidgetListModelItem::~WidgetListModelItem()
{
    contextInfo->deleteLater();
}

class WidgetListModelPrivate
{
public:
    explicit WidgetListModelPrivate(WidgetListModel *q);
    virtual ~WidgetListModelPrivate();
    void init();
    QList<WidgetListModelItem *> items;
    WidgetFactory *factory;
protected:
    WidgetListModel * const q_ptr;
private:
    Q_DECLARE_PUBLIC(WidgetListModel)
};

WidgetListModelPrivate::WidgetListModelPrivate(WidgetListModel *q)
    : factory(0), q_ptr(q)
{
}

WidgetListModelPrivate::~WidgetListModelPrivate()
{
    qDeleteAll(items);
}

void WidgetListModelPrivate::init()
{
    Q_Q(WidgetListModel);
    QQmlContext *context = QQmlEngine::contextForObject(q);
    if (context) {
        factory = new WidgetFactory(context->engine(), q);
    } else {
        qWarning() << "Failed to initialize widget factory. No widget will be available.";
    }
}

WidgetListModel::WidgetListModel(QObject *parent)
    : QAbstractListModel(parent), d_ptr(new WidgetListModelPrivate(this))
{
    Q_D(WidgetListModel);
    QQmlContext *context = QQmlEngine::contextForObject(this);
    if (context) {
        d->factory = new WidgetFactory(context->engine(), this);
    }
}


WidgetListModel::~WidgetListModel()
{
}

void WidgetListModel::classBegin()
{
}

void WidgetListModel::componentComplete()
{
    Q_D(WidgetListModel);
    d->init();
}

int WidgetListModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    Q_D(const WidgetListModel);
    return d->items.count();
}

QVariant WidgetListModel::data(const QModelIndex &index, int role) const
{
    Q_D(const WidgetListModel);
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }

    const WidgetListModelItem *item = d->items.at(row);
    switch (role) {
    case ContextInfoRole:
        return QVariant::fromValue(item->contextInfo);
        break;
    default:
        return QVariant();
        break;
    }
}

int WidgetListModel::count() const
{
    return rowCount();
}

void WidgetListModel::createWidget(int index, QObject *parent)
{
    Q_D(WidgetListModel);
    if (index < 0 || index >= rowCount()) {
        return;
    }

    if (!d->factory) {
        return;
    }

    const WidgetListModelItem *item = d->items.at(index);
    d->factory->createWidget(item->source, item->contextInfo, parent);
}

void WidgetListModel::add(const QString &source)
{
    Q_D(WidgetListModel);
    if (!d->factory->readSource(source)) {
        return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    WidgetListModelItem *item = new WidgetListModelItem;
    item->source = d->factory->widgetSource();
    item->contextInfo = d->factory->createWidgetContext(this);
    d->items.append(item);
    emit countChanged();
    endInsertRows();
}

void WidgetListModel::move(int sourceIndex, int destinationIndex)
{
    Q_D(WidgetListModel);
    if (!beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), destinationIndex)) {
        return;
    }
    int newDestinationIndex = destinationIndex;
    if (newDestinationIndex > sourceIndex) {
        newDestinationIndex --;
    }
    d->items.move(sourceIndex, newDestinationIndex);
    endMoveRows();
}

void WidgetListModel::remove(int index)
{
    Q_D(WidgetListModel);
    if (index < 0 || index >= rowCount()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);

    delete d->items.takeAt(index);
    emit countChanged();
    endRemoveRows();
}

void WidgetListModel::setSize(int index, int size)
{
    Q_D(WidgetListModel);
    if (index < 0 || index >= rowCount()) {
        return;
    }

    WidgetListModelItem *item = d->items[index];
    item->contextInfo->setSize((WidgetContextInfo::WidgetSize) size);
}

QHash<int, QByteArray> WidgetListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(ContextInfoRole, "contextInfo");
    return roles;
}
