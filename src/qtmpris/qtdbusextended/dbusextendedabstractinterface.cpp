//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#include "dbusextendedpendingcallwatcher_p.h"

#include <dbusextendedabstractinterface.h>

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>

#include <QtCore/QDebug>
#include <QtCore/QMetaProperty>


Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, dBusPropertiesInterface, ("org.freedesktop.DBus.Properties"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, dBusPropertiesChangedSignal, ("PropertiesChanged"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, propertyChangedSignature, ("propertyChanged(QString,QVariant)"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, propertyInvalidatedSignature, ("propertyInvalidated(QString)"))


DBusExtendedAbstractInterface::DBusExtendedAbstractInterface(const QString &service, const QString &path, const char *interface, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, interface, connection, parent)
    , m_sync(false)
    , m_useCache(false)
    , m_getAllPendingCallWatcher(nullptr)
    , m_propertiesChangedConnected(false)
{
}

DBusExtendedAbstractInterface::~DBusExtendedAbstractInterface()
{
}

void DBusExtendedAbstractInterface::getAllProperties()
{
    m_lastExtendedError = QDBusError();

    if (!isValid()) {
        QString errorMessage = QStringLiteral("This Extended DBus interface is not valid yet.");
        m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
        qDebug() << Q_FUNC_INFO << errorMessage;
        return;
    }

    if (!m_sync && m_getAllPendingCallWatcher) {
        // Call already in place, not repeating ...
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), QString::fromLocal8Bit(*dBusPropertiesInterface()), QStringLiteral("GetAll"));
    msg << interface();

    if (m_sync) {
        QDBusMessage reply = connection().call(msg);

        if (reply.type() != QDBusMessage::ReplyMessage) {
            m_lastExtendedError = QDBusError(reply);
            qWarning() << Q_FUNC_INFO << m_lastExtendedError.message();
            return;
        }

        if (reply.signature() != QLatin1String("a{sv}")) {
            QString errorMessage = QStringLiteral("Invalid signature \"%1\" in return from call to %2")
                .arg(reply.signature(),
                     QString::fromLocal8Bit(*dBusPropertiesInterface()));
            qWarning() << Q_FUNC_INFO << errorMessage;
            m_lastExtendedError = QDBusError(QDBusError::InvalidSignature, errorMessage);
            return;
        }

        QVariantMap value = reply.arguments().at(0).toMap();
        onPropertiesChanged(interface(), value, QStringList());
    } else {
        QDBusPendingReply<QVariantMap> async = connection().asyncCall(msg);
        m_getAllPendingCallWatcher = new QDBusPendingCallWatcher(async, this);

        connect(m_getAllPendingCallWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(onAsyncGetAllPropertiesFinished(QDBusPendingCallWatcher*)));
        return;
    }
}

void DBusExtendedAbstractInterface::connectNotify(const QMetaMethod &signal)
{
    if (signal.methodType() == QMetaMethod::Signal
        && (signal.methodSignature() == *propertyChangedSignature()
            || signal.methodSignature() == *propertyInvalidatedSignature())) {
        if (!m_propertiesChangedConnected) {
            QStringList argumentMatch;
            argumentMatch << interface();
            connection().connect(service(), path(), QString::fromLocal8Bit(*dBusPropertiesInterface()), QString::fromLocal8Bit(*dBusPropertiesChangedSignal()),
                                 argumentMatch, QString(),
                                 this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

            m_propertiesChangedConnected = true;
            return;
        }
    } else {
        QDBusAbstractInterface::connectNotify(signal);
    }
}

void DBusExtendedAbstractInterface::disconnectNotify(const QMetaMethod &signal)
{
    if (signal.methodType() == QMetaMethod::Signal
        && (signal.methodSignature() == *propertyChangedSignature()
            || signal.methodSignature() == *propertyInvalidatedSignature())) {
        if (m_propertiesChangedConnected
            && 0 == receivers(propertyChangedSignature()->constData())
            && 0 == receivers(propertyInvalidatedSignature()->constData())) {
            QStringList argumentMatch;
            argumentMatch << interface();
            connection().disconnect(service(), path(), QString::fromLocal8Bit(*dBusPropertiesInterface()),
                                    QString::fromLocal8Bit(*dBusPropertiesChangedSignal()),
                                    argumentMatch, QString(),
                                    this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

            m_propertiesChangedConnected = false;
            return;
        }
    } else {
        QDBusAbstractInterface::disconnectNotify(signal);
    }
}

QVariant DBusExtendedAbstractInterface::internalPropGet(const char *propname, void *propertyPtr)
{
    m_lastExtendedError = QDBusError();

    if (m_useCache) {
        int propertyIndex = metaObject()->indexOfProperty(propname);
        QMetaProperty metaProperty = metaObject()->property(propertyIndex);
        return QVariant(metaProperty.metaType(), propertyPtr);
    }

    if (m_sync) {
        return property(propname);
    } else {
        if (!isValid()) {
            QString errorMessage = QStringLiteral("This Extended DBus interface is not valid yet.");
            m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
            qDebug() << Q_FUNC_INFO << errorMessage;
            return QVariant();
        }

        int propertyIndex = metaObject()->indexOfProperty(propname);

        if (-1 == propertyIndex) {
            QString errorMessage = QStringLiteral("Got unknown property \"%1\" to read")
                    .arg(QString::fromLatin1(propname));
            m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
            qWarning() << Q_FUNC_INFO << errorMessage;
            return QVariant();
        }

        QMetaProperty metaProperty = metaObject()->property(propertyIndex);

        if (!metaProperty.isReadable()) {
            QString errorMessage = QStringLiteral("Property \"%1\" is NOT readable")
                    .arg(QString::fromLatin1(propname));
            m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
            qWarning() << Q_FUNC_INFO << errorMessage;
            return QVariant();
        }

        // is this metatype registered?
        const char *expectedSignature = "";
        if (metaProperty.metaType() != QMetaType(QMetaType::QVariant)) {
            expectedSignature = QDBusMetaType::typeToSignature(metaProperty.metaType());
            if (nullptr == expectedSignature) {
                QString errorMessage =
                        QStringLiteral("Type %1 must be registered with Qt D-Bus "
                                       "before it can be used to read property "
                                       "%2.%3")
                        .arg(metaProperty.typeName(),
                             interface(),
                             propname);
                m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
                qWarning() << Q_FUNC_INFO << errorMessage;
                return QVariant();
            }
        }

        asyncProperty(QString::fromLocal8Bit(propname));
        return QVariant(metaProperty.metaType(), propertyPtr);
    }
}

void DBusExtendedAbstractInterface::internalPropSet(const char *propname, const QVariant &value, void *propertyPtr)
{
    m_lastExtendedError = QDBusError();

    if (m_sync) {
        setProperty(propname, value);
    } else {
        if (!isValid()) {
            QString errorMessage = QStringLiteral("This interface is not yet valid");
            m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
            qDebug() << Q_FUNC_INFO << errorMessage;
            return;
        }

        int propertyIndex = metaObject()->indexOfProperty(propname);

        if (-1 == propertyIndex) {
            QString errorMessage = QStringLiteral("Got unknown property \"%1\" to write")
                    .arg(QString::fromLatin1(propname));
            m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
            qWarning() << Q_FUNC_INFO << errorMessage;
            return;
        }

        QMetaProperty metaProperty = metaObject()->property(propertyIndex);

        if (!metaProperty.isWritable()) {
            QString errorMessage = QStringLiteral("Property \"%1\" is NOT writable")
                    .arg(QString::fromLatin1(propname));
            m_lastExtendedError = QDBusMessage::createError(QDBusError::Failed, errorMessage);
            qWarning() << Q_FUNC_INFO << errorMessage;
            return;
        }

        asyncSetProperty(QString::fromLatin1(propname), QVariant(metaProperty.metaType(), propertyPtr));
    }
}

QVariant DBusExtendedAbstractInterface::asyncProperty(const QString &propertyName)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), QString::fromLocal8Bit(*dBusPropertiesInterface()), QStringLiteral("Get"));
    msg << interface() << propertyName;
    QDBusPendingReply<QVariant> async = connection().asyncCall(msg);
    DBusExtendedPendingCallWatcher *watcher = new DBusExtendedPendingCallWatcher(async, propertyName, QVariant(), this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(onAsyncPropertyFinished(QDBusPendingCallWatcher*)));

    return QVariant();
}

void DBusExtendedAbstractInterface::asyncSetProperty(const QString &propertyName, const QVariant &value)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), QString::fromLocal8Bit(*dBusPropertiesInterface()), QStringLiteral("Set"));
    msg << interface() << propertyName << value;
    QDBusPendingReply<QVariant> async = connection().asyncCall(msg);
    DBusExtendedPendingCallWatcher *watcher = new DBusExtendedPendingCallWatcher(async, propertyName, value, this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(onAsyncSetPropertyFinished(QDBusPendingCallWatcher*)));
}


void DBusExtendedAbstractInterface::onAsyncPropertyFinished(DBusExtendedPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariant> reply = *watcher;

    if (reply.isError()) {
        m_lastExtendedError = reply.error();
    } else {
        int propertyIndex = metaObject()->indexOfProperty(watcher->asyncProperty().toLatin1().constData());
        QVariant value = demarshall(interface(),
                                    metaObject()->property(propertyIndex),
                                    reply.value(),
                                    &m_lastExtendedError);

        if (m_lastExtendedError.isValid()) {
            Q_EMIT propertyInvalidated(watcher->asyncProperty());
        } else {
            Q_EMIT propertyChanged(watcher->asyncProperty(), value);
        }
    }

    Q_EMIT asyncPropertyFinished(watcher->asyncProperty());
    watcher->deleteLater();
}

void DBusExtendedAbstractInterface::onAsyncSetPropertyFinished(DBusExtendedPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariant> reply = *watcher;

    if (reply.isError()) {
        m_lastExtendedError = reply.error();
    } else {
        m_lastExtendedError = QDBusError();
    }

    Q_EMIT asyncSetPropertyFinished(watcher->asyncProperty());

    // Resetting the property to its previous value after sending the
    // finished signal
    if (reply.isError()) {
        m_lastExtendedError = QDBusError();
        Q_EMIT propertyChanged(watcher->asyncProperty(), watcher->previousValue());
    }

    watcher->deleteLater();
}

void DBusExtendedAbstractInterface::onAsyncGetAllPropertiesFinished(QDBusPendingCallWatcher *watcher)
{
    m_getAllPendingCallWatcher = nullptr;

    QDBusPendingReply<QVariantMap> reply = *watcher;

    if (reply.isError()) {
        m_lastExtendedError = reply.error();
    } else {
        m_lastExtendedError = QDBusError();
    }

    Q_EMIT asyncGetAllPropertiesFinished();

    if (!reply.isError()) {
        onPropertiesChanged(interface(), reply.value(), QStringList());
    }

    watcher->deleteLater();
}

void DBusExtendedAbstractInterface::onPropertiesChanged(const QString& interfaceName,
                                                        const QVariantMap& changedProperties,
                                                        const QStringList& invalidatedProperties)
{
    if (interfaceName == interface()) {
        QVariantMap::const_iterator i = changedProperties.constBegin();
        while (i != changedProperties.constEnd()) {
            int propertyIndex = metaObject()->indexOfProperty(i.key().toLatin1().constData());

            if (-1 == propertyIndex) {
                qDebug() << Q_FUNC_INFO << "Got unknown changed property" <<  i.key();
            } else {
                QVariant value = demarshall(interface(), metaObject()->property(propertyIndex), i.value(), &m_lastExtendedError);

                if (m_lastExtendedError.isValid()) {
                    Q_EMIT propertyInvalidated(i.key());
                } else {
                    Q_EMIT propertyChanged(i.key(), value);
                }
            }

            ++i;
        }

        QStringList::const_iterator j = invalidatedProperties.constBegin();
        while (j != invalidatedProperties.constEnd()) {
            if (-1 == metaObject()->indexOfProperty(j->toLatin1().constData())) {
                qDebug() << Q_FUNC_INFO << "Got unknown invalidated property" <<  *j;
            } else {
                m_lastExtendedError = QDBusError();
                Q_EMIT propertyInvalidated(*j);
            }

            ++j;
        }
    }
}

QVariant DBusExtendedAbstractInterface::demarshall(const QString &interface, const QMetaProperty &metaProperty, const QVariant &value, QDBusError *error)
{
    Q_ASSERT(metaProperty.isValid());
    Q_ASSERT(error != 0);

    if (value.userType() == metaProperty.userType()) {
        // No need demarshalling. Passing back straight away ...
        *error = QDBusError();
        return value;
    }

    QVariant result = QVariant(metaProperty.metaType(), (void *)nullptr);
    QString errorMessage;
    const char *expectedSignature = QDBusMetaType::typeToSignature(metaProperty.metaType());

    if (value.userType() == qMetaTypeId<QDBusArgument>()) {
        // demarshalling a DBus argument ...
        QDBusArgument dbusArg = value.value<QDBusArgument>();

        if (expectedSignature == dbusArg.currentSignature().toLatin1()) {
            QDBusMetaType::demarshall(dbusArg, metaProperty.metaType(), result.data());
            if (!result.isValid()) {
                errorMessage = QStringLiteral("Unexpected failure demarshalling "
                                              "upon PropertiesChanged signal arrival "
                                              "for property `%3.%4' (expected type `%5' (%6))")
                                   .arg(interface,
                                        QString::fromLatin1(metaProperty.name()),
                                        QString::fromLatin1(metaProperty.typeName()),
                                        expectedSignature);
            }
        } else {
            errorMessage = QStringLiteral("Unexpected `user type' (%2) "
                                          "upon PropertiesChanged signal arrival "
                                          "for property `%3.%4' (expected type `%5' (%6))")
                               .arg(dbusArg.currentSignature(),
                                    interface,
                                    QString::fromLatin1(metaProperty.name()),
                                    QString::fromLatin1(metaProperty.typeName()),
                                    QString::fromLatin1(expectedSignature));
        }
    } else {
        const char *actualSignature = QDBusMetaType::typeToSignature(value.metaType());

        errorMessage = QStringLiteral("Unexpected `%1' (%2) "
                                      "upon PropertiesChanged signal arrival "
                                      "for property `%3.%4' (expected type `%5' (%6))")
                           .arg(QString::fromLatin1(value.typeName()),
                                QString::fromLatin1(actualSignature),
                                interface,
                                QString::fromLatin1(metaProperty.name()),
                                QString::fromLatin1(metaProperty.typeName()),
                                QString::fromLatin1(expectedSignature));
    }

    if (errorMessage.isEmpty()) {
        *error = QDBusError();
    } else {
        *error = QDBusMessage::createError(QDBusError::InvalidSignature, errorMessage);
        qDebug() << Q_FUNC_INFO << errorMessage;
    }

    return result;
}
