//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#include "mprisplugin.h"

#include "mpris.h"
#include "mprisplayer.h"
#include "mprismanager.h"

#include <qqml.h>

static QObject * api_factory(QQmlEngine *, QJSEngine *)
{
    return new Mpris;
}

MprisPlugin::MprisPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

MprisPlugin::~MprisPlugin()
{
}

void MprisPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<Mpris>(uri, 1, 0, "Mpris", api_factory);
    qmlRegisterType<MprisPlayer>(uri, 1, 0, "MprisPlayer");
    qmlRegisterType<MprisManager>(uri, 1, 0, "MprisManager");
}
