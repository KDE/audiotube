//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#ifndef MPRIS_PLUGIN_H
#define MPRIS_PLUGIN_H

#include <QQmlExtensionPlugin>
#include <QtGlobal>


class MprisPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nemomobile.mpris")

public:
    MprisPlugin(QObject *parent = 0);
    ~MprisPlugin();

    virtual void registerTypes(const char *uri) override;
};


#endif /* MPRIS_PLUGIN_H */
