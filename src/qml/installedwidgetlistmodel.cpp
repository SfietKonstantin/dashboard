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

#include "installedwidgetlistmodel.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include "widgetfactory.h"

static const char *DEFAULT_PATH = "/usr/share/dashboard/widgets";

struct InstalledWidgetListModelItem
{
    QString name;
    QString description;
    QString source;
};

class InstalledWidgetListModelPrivate
{
public:
    explicit InstalledWidgetListModelPrivate(InstalledWidgetListModel *q);
    virtual ~InstalledWidgetListModelPrivate();
    void init();
    void refresh();
    bool initialized;
    QStringList searchPaths;
    QList<InstalledWidgetListModelItem *> items;
    WidgetFactory *factory;
protected:
    InstalledWidgetListModel * const q_ptr;
private:
    Q_DECLARE_PUBLIC(InstalledWidgetListModel)
};

InstalledWidgetListModelPrivate::InstalledWidgetListModelPrivate(InstalledWidgetListModel *q)
    : initialized(false), factory(0), q_ptr(q)
{
}

InstalledWidgetListModelPrivate::~InstalledWidgetListModelPrivate()
{
    qDeleteAll(items);
}

void InstalledWidgetListModelPrivate::init()
{
    Q_Q(InstalledWidgetListModel);
    QQmlContext *context = QQmlEngine::contextForObject(q);
    if (context) {
        factory = new WidgetFactory(context->engine(), q);
    } else {
        qWarning() << "Failed to initialize widget factory. No widget will be available.";
    }
    initialized = true;
}

void InstalledWidgetListModelPrivate::refresh()
{
    Q_Q(InstalledWidgetListModel);

    if (!initialized) {
        return;
    }

    if (!factory) {
        return;
    }

    QStringList allSearchPaths;
    allSearchPaths.append(DEFAULT_PATH);
    allSearchPaths.append(searchPaths);

    QMap <QString, InstalledWidgetListModelItem *> sortedItems;
    foreach (const QString &path, allSearchPaths) {
        QDir dir (path);
        if (!dir.exists()) {
            continue;
        }

        QStringList subdirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        foreach (const QString &subdirPaths, subdirs) {
            QDir subdir (path);
            if (!subdir.cd(subdirPaths)) {
                continue;
            }

            if (!factory->readSource(subdir.absolutePath())) {
                continue;
            }

            InstalledWidgetListModelItem *item = new InstalledWidgetListModelItem;
            item->name = factory->widgetName();
            item->description = factory->widgetDescription();
            item->source = subdir.absolutePath();
            sortedItems.insert(item->name, item);
        }
    }

    if (!items.isEmpty()) {
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        qDeleteAll(items);
        q->endRemoveRows();
    }

    if (!sortedItems.isEmpty()) {
        q->beginInsertRows(QModelIndex(), 0, sortedItems.count() - 1);
        foreach (InstalledWidgetListModelItem *item, sortedItems) {
            items.append(item);
        }
        emit q->countChanged();
        q->endInsertRows();
    }
}

InstalledWidgetListModel::InstalledWidgetListModel(QObject *parent) :
    QAbstractListModel(parent), d_ptr(new InstalledWidgetListModelPrivate(this))
{
}

InstalledWidgetListModel::~InstalledWidgetListModel()
{
}

void InstalledWidgetListModel::classBegin()
{
}

void InstalledWidgetListModel::componentComplete()
{
    Q_D(InstalledWidgetListModel);
    d->init();
    d->refresh();
}

int InstalledWidgetListModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    Q_D(const InstalledWidgetListModel);
    return d->items.count();
}

QVariant InstalledWidgetListModel::data(const QModelIndex &index, int role) const
{
    Q_D(const InstalledWidgetListModel);
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }

    const InstalledWidgetListModelItem *item = d->items.at(row);
    switch (role) {
    case NameRole:
        return item->name;
        break;
    case DescriptionRole:
        return item->description;
        break;
    case SourceRole:
        return item->source;
        break;
    default:
        return QVariant();
        break;
    }
}

int InstalledWidgetListModel::count() const
{
    return rowCount();
}

QStringList InstalledWidgetListModel::searchPaths() const
{
    Q_D(const InstalledWidgetListModel);
    return d->searchPaths;
}

void InstalledWidgetListModel::setSearchPaths(const QStringList &searchPaths)
{
    Q_D(InstalledWidgetListModel);
    if (d->searchPaths != searchPaths) {
        d->searchPaths = searchPaths;
        d->refresh();
        emit searchPathsChanged();
    }
}

QHash<int, QByteArray> InstalledWidgetListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(DescriptionRole, "description");
    roles.insert(SourceRole, "source");
    return roles;
}
