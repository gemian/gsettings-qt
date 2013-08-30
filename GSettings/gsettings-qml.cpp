/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Lars Uebernickel <lars.uebernickel@canonical.com>
 */

#include "gsettings-qml.h"
#include <QGSettings>

struct GSettingsSchemaQmlPrivate
{
    QByteArray id;
    QByteArray path;
};

struct GSettingsQmlPrivate
{
    GSettingsSchemaQml *schema;
    QGSettings *settings;
};

GSettingsSchemaQml::GSettingsSchemaQml(QObject *parent): QObject(parent)
{
    priv = new GSettingsSchemaQmlPrivate;
}

GSettingsSchemaQml::~GSettingsSchemaQml()
{
    delete priv;
}

QByteArray GSettingsSchemaQml::id() const
{
    return priv->id;
}

void GSettingsSchemaQml::setId(const QByteArray &id)
{
    if (!priv->id.isEmpty()) {
        qWarning("GSettings.schema.id may only be set on construction");
        return;
    }

    priv->id = id;
}

QByteArray GSettingsSchemaQml::path() const
{
    return priv->path;
}

void GSettingsSchemaQml::setPath(const QByteArray &path)
{
    if (!priv->path.isEmpty()) {
        qWarning("GSettings.schema.path may only be set on construction");
        return;
    }

    priv->path = path;
}

QVariantList GSettingsSchemaQml::choices(const QByteArray &key) const
{
    GSettingsQml *parent = (GSettingsQml *) this->parent();

    if (parent->priv->settings == NULL)
        return QVariantList();

    if (!parent->contains(key))
        return QVariantList();

    return parent->priv->settings->choices(key);
}

void GSettingsSchemaQml::reset(const QByteArray &key)
{
    GSettingsQml *parent = (GSettingsQml *) this->parent();

    if (parent->priv->settings != NULL)
        parent->priv->settings->reset(key);
}

GSettingsQml::GSettingsQml(QObject *parent): QQmlPropertyMap(this, parent)
{
    priv = new GSettingsQmlPrivate;
    priv->schema = new GSettingsSchemaQml(this);
    priv->settings = NULL;
}

GSettingsQml::~GSettingsQml()
{
    delete priv;
}

GSettingsSchemaQml * GSettingsQml::schema() const
{
    return priv->schema;
}

void GSettingsQml::classBegin()
{
}

void GSettingsQml::componentComplete()
{
    priv->settings = new QGSettings(priv->schema->id(), priv->schema->path(), this);

    connect(priv->settings, SIGNAL(changed(const QString &)), this, SLOT(settingChanged(const QString &)));

    Q_FOREACH(QString key, priv->settings->keys())
        this->insert(key, priv->settings->get(key));

    Q_EMIT(schemaChanged());
}

void GSettingsQml::settingChanged(const QString &key)
{
    QVariant value = priv->settings->get(key);
    this->insert(key, value);
    Q_EMIT(changed(key, value));
}

QVariant GSettingsQml::updateValue(const QString& key, const QVariant &value)
{
    if (priv->settings == NULL)
        return QVariant();

    if (priv->settings->trySet(key, value)) {
        return value;
    }
    else {
        qWarning("unable to set key '%s' to value '%s'", key.toUtf8().constData(), value.toString().toUtf8().constData());
        return priv->settings->get(key);
    }
}
