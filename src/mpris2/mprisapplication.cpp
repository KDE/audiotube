#include "mprisapplication.h"

#include <QDBusConnection>

#include "mediaplayer2adaptor.h"
#include "playeradaptor.h"

struct MprisApplicationPrivate {
    std::unique_ptr<MediaPlayer2Adaptor> adaptor;
    bool canQuit = false;
    bool canRaise = false;
    bool canSetFullscreen = false;
    QString desktopEntry;
    bool fullscreen = false;
    bool hasTrackList = false;
    QString identity;
    QStringList supportedMimeTypes;
    QStringList supportedUriSchemes;
};

MprisApplication::MprisApplication(const QString &appId, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<MprisApplicationPrivate>())
{
    d->adaptor = std::make_unique<MediaPlayer2Adaptor>(this);
    QDBusConnection::sessionBus().registerService(QStringLiteral("org.mpris.MediaPlayer2.") + appId);
    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this)) {
        qWarning() << "Failed to register MPRIS interface";
    }
}

MprisApplication::~MprisApplication() = default;

bool MprisApplication::canQuit() const
{
    return d->canQuit;
}

void MprisApplication::setCanQuit(bool canQuit)
{
    if (d->canQuit == canQuit) {
        return;
    }

    d->canQuit = canQuit;
    Q_EMIT canQuitChanged();
}

bool MprisApplication::canRaise() const
{
    return d->canRaise;
}

void MprisApplication::setCanRaise(bool canRaise)
{
    if (d->canRaise == canRaise) {
        return;
    }

    d->canRaise = canRaise;
    Q_EMIT canRaiseChanged();
}

bool MprisApplication::canSetFullscreen() const
{
    return d->canSetFullscreen;
}

void MprisApplication::setCanSetFullscreen(bool can)
{
    if (d->canSetFullscreen == can) {
        return;
    }

    d->canSetFullscreen = can;
    Q_EMIT canSetFullscreenChanged();
}

QString MprisApplication::desktopEntry() const
{
    return d->desktopEntry;
}

void MprisApplication::setDesktopEntry(const QString &entry)
{
    if (d->desktopEntry == entry) {
        return;
    }

    d->desktopEntry = entry;
    Q_EMIT desktopEntryChanged();
}

bool MprisApplication::fullscreen() const
{
    return d->fullscreen;
}

void MprisApplication::setFullscreen(bool value)
{
    if (d->fullscreen == value) {
        return;
    }

    d->fullscreen = value;
    Q_EMIT fullscreenChanged();
}

bool MprisApplication::hasTrackList() const
{
    return d->hasTrackList;
}

void MprisApplication::setHasTrackList(bool has)
{
    if (d->hasTrackList == has) {
        return;
    }

    d->hasTrackList = has;
    Q_EMIT hasTrackListChanged();
}

QString MprisApplication::identity() const
{
    return d->identity;
}

void MprisApplication::setIdentity(const QString &appName)
{
    if (d->identity == appName) {
        return;
    }

    d->identity = appName;
    Q_EMIT identityChanged();
}

QStringList MprisApplication::supportedMimeTypes() const
{
    return d->supportedMimeTypes;
}

void MprisApplication::setSupportedMimeTypes(const QStringList &types) {
    if (d->supportedMimeTypes == types) {
        return;
    }

    d->supportedMimeTypes = types;
    Q_EMIT supportedMimeTypesChanged();
}

QStringList MprisApplication::supportedUriSchemes() const
{
    return d->supportedUriSchemes;
}

void MprisApplication::setSupportedUriSchemes(const QStringList &schemes)
{
    if (d->supportedUriSchemes == schemes) {
        return;
    }

    d->supportedMimeTypes = schemes;
    Q_EMIT supportedUriSchemesChanged();
}

void MprisApplication::Quit()
{
    Q_EMIT quitRequested();
}

void MprisApplication::Raise()
{
    Q_EMIT raiseRequested();
}
