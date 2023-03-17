//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#include "mprisplayer_p.h"

#include "mprisplayer.h"

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class MprisRootAdaptor
 */

static const QString mprisRootInterface = QStringLiteral("org.mpris.MediaPlayer2");


MprisRootAdaptor::MprisRootAdaptor(MprisPlayer *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(false);

    connect(parent, SIGNAL(canQuitChanged()), this, SLOT(onCanQuitChanged()));
    connect(parent, SIGNAL(canRaiseChanged()), this, SLOT(onCanRaiseChanged()));
    connect(parent, SIGNAL(canSetFullscreenChanged()), this, SLOT(onCanSetFullscreenChanged()));
    connect(parent, SIGNAL(desktopEntryChanged()), this, SLOT(onDesktopEntryChanged()));
    connect(parent, SIGNAL(fullscreenChanged()), this, SLOT(onFullscreenChanged()));
    connect(parent, SIGNAL(hasTrackListChanged()), this, SLOT(onHasTrackListChanged()));
    connect(parent, SIGNAL(identityChanged()), this, SLOT(onIdentityChanged()));
    connect(parent, SIGNAL(supportedUriSchemesChanged()), this, SLOT(onSupportedUriSchemesChanged()));
    connect(parent, SIGNAL(supportedMimeTypesChanged()), this, SLOT(onSupportedMimeTypesChanged()));
}

MprisRootAdaptor::~MprisRootAdaptor()
{
    // destructor
}

bool MprisRootAdaptor::canQuit() const
{
    return static_cast<MprisPlayer *>(parent())->canQuit();
}

bool MprisRootAdaptor::canRaise() const
{
    return static_cast<MprisPlayer *>(parent())->canRaise();
}

bool MprisRootAdaptor::canSetFullscreen() const
{
    return static_cast<MprisPlayer *>(parent())->canSetFullscreen();
}

QString MprisRootAdaptor::desktopEntry() const
{
    return static_cast<MprisPlayer *>(parent())->desktopEntry();
}

bool MprisRootAdaptor::fullscreen() const
{
    return static_cast<MprisPlayer *>(parent())->fullscreen();
}

void MprisRootAdaptor::setFullscreen(bool value)
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());
    if (player->canSetFullscreen()) {
        Q_EMIT player->fullscreenRequested(value);
        return;
    }

    // We cannot send an error reply in a property setter so we just
    // complain here
    if (value) {
        qDebug() << Q_FUNC_INFO << "Requested to fullscreen, but not supported";
    } else {
        qDebug() << Q_FUNC_INFO << "Requested to unfullscreen, but not supported";
    }
}

bool MprisRootAdaptor::hasTrackList() const
{
    return static_cast<MprisPlayer *>(parent())->hasTrackList();
}

QString MprisRootAdaptor::identity() const
{
    return static_cast<MprisPlayer *>(parent())->identity();
}

QStringList MprisRootAdaptor::supportedMimeTypes() const
{
    return static_cast<MprisPlayer *>(parent())->supportedMimeTypes();
}

QStringList MprisRootAdaptor::supportedUriSchemes() const
{
    return static_cast<MprisPlayer *>(parent())->supportedUriSchemes();
}

void MprisRootAdaptor::Quit()
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());
    if (player->canQuit()) {
        Q_EMIT player->quitRequested();
        return;
    }

    player->sendErrorReply(QDBusError::NotSupported, QStringLiteral("Quit requested but not supported."));
}

void MprisRootAdaptor::Raise()
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());
    if (player->canRaise()) {
        Q_EMIT player->raiseRequested();
        return;
    }

    player->sendErrorReply(QDBusError::NotSupported, QStringLiteral("Raise requested but not supported."));
}


// Private
void MprisRootAdaptor::onCanQuitChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("CanQuit")] = QVariant(player->canQuit());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onCanRaiseChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("CanRaise")] = QVariant(player->canRaise());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onCanSetFullscreenChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("CanSetFullscreen")] = QVariant(player->canSetFullscreen());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onDesktopEntryChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("DesktopEntry")] = QVariant(player->desktopEntry());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onFullscreenChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("Fullscreen")] = QVariant(player->fullscreen());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onHasTrackListChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("HasTrackList")] = QVariant(player->hasTrackList());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onIdentityChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("Identity")] = QVariant(player->identity());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onSupportedUriSchemesChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("SupportedUriSchemes")] = QVariant(player->supportedUriSchemes());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}

void MprisRootAdaptor::onSupportedMimeTypesChanged() const
{
    MprisPlayer * const player = static_cast<MprisPlayer *>(parent());

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("SupportedMimeTypes")] = QVariant(player->supportedMimeTypes());

    player->notifyPropertiesChanged(mprisRootInterface, changedProperties, QStringList());
}
