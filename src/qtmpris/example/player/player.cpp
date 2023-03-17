//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#include <Mpris>
#include <MprisPlayer>

#include <QtQuick>

#include <QGuiApplication>

static QObject * api_factory(QQmlEngine *, QJSEngine *)
{
    return new Mpris;
}

int main(int argc, char *argv[])
{
    QGuiApplication *app = new QGuiApplication(argc, argv);
    QQuickView *view = new QQuickView;

    qmlRegisterSingletonType<Mpris>("org.nemomobile.qtmpris", 1, 0, "Mpris", api_factory);
    qmlRegisterType<MprisPlayer>("org.nemomobile.qtmpris", 1, 0, "MprisPlayer");

    view->setSource(app->applicationDirPath().append("/../qml/player.qml"));
    view->show();

    int retVal = app->exec();

    return retVal;
}
