//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#include "dbusextendedpendingcallwatcher_p.h"


DBusExtendedPendingCallWatcher::DBusExtendedPendingCallWatcher(const QDBusPendingCall &call, const QString &asyncProperty, const QVariant &previousValue, QObject *parent)
    : QDBusPendingCallWatcher(call, parent)
    , m_asyncProperty(asyncProperty)
    , m_previousValue(previousValue)
{
}

DBusExtendedPendingCallWatcher::~DBusExtendedPendingCallWatcher()
{
}
