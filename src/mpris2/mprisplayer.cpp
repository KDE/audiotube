#include "mprisplayer.h"

#include <playeradaptor.h>

MprisPlayer::MprisPlayer(QObject *parent)
    : QObject(parent)
    , m_adaptor(std::make_unique<PlayerAdaptor>(this))
{
    QDBusConnection::sessionBus().registerService(QStringLiteral("org.mpris.MediaPlayer2.") + "audiotube");
    if (!QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this)) {
        qWarning() << "Failed to register MPRIS interface";
    }
}

void MprisPlayer::Next()
{
    Q_EMIT nextRequested();
}

void MprisPlayer::OpenUri(const QString &uri)
{
    Q_EMIT openUriRequested(uri);
}

void MprisPlayer::Pause()
{
    Q_EMIT pauseRequested();
}

void MprisPlayer::Play()
{
    Q_EMIT playRequested();
}

void MprisPlayer::PlayPause()
{
    Q_EMIT playPauseRequested();
}

void MprisPlayer::Previous()
{
    Q_EMIT previousRequested();
}

void MprisPlayer::Seek(qlonglong offset)
{
    Q_EMIT seekRequested(offset);
}

void MprisPlayer::SetPosition(const QDBusObjectPath &trackId, qlonglong position)
{
    Q_EMIT setPositionRequested(MprisTrack {}, position); // TODO write MprisTrack class
}

void MprisPlayer::Stop()
{
    Q_EMIT stopRequested();
}

MprisPlayer::~MprisPlayer() = default;
