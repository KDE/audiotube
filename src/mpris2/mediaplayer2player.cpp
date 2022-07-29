// SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
// SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
// SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
// SPDX-FileCopyrightText: 2022 (c) Jonah Brüchert <jbb@kaidan.im>

// SPDX-License-Identifier: GPL-3.0-or-later

#include "mediaplayer2player.h"

#include <QStringList>
#include <QStringBuilder>
#include <QDBusConnection>
#include <QMetaClassInfo>

#include <QImage>
#include <QBuffer>


constexpr double MAX_RATE = 1.0;
constexpr const double MIN_RATE = 1.0;

MediaPlayer2Player::MediaPlayer2Player(QObject* parent)
    : QDBusAbstractAdaptor(parent)
{
    m_volume = 0; // TODO
    m_canPlay = true; // TODO
    signalPropertiesChange(QStringLiteral("Volume"), Volume());

    m_mediaPlayerPresent = 1;
}

MediaPlayer2Player::~MediaPlayer2Player() = default;

QString MediaPlayer2Player::PlaybackStatus() const
{
    switch (m_playbackState) {
    case PlaybackStopped:
        return QStringLiteral("Stopped");
    case PlaybackRunning:
        return QStringLiteral("Playing");
    case PlaybackPaused:
        return QStringLiteral("Paused");
    }

    Q_UNREACHABLE();
}

bool MediaPlayer2Player::CanGoNext() const
{
    return m_canGoNext;
}

void MediaPlayer2Player::Next()
{
    Q_EMIT next();
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    return m_canGoPrevious;
}

void MediaPlayer2Player::Previous()
{
    Q_EMIT previous();
}

bool MediaPlayer2Player::CanPause() const
{
    return m_canPlay;
}

void MediaPlayer2Player::Pause()
{
    Q_EMIT pause();
}

void MediaPlayer2Player::PlayPause()
{
    Q_EMIT playPause();
}

void MediaPlayer2Player::Stop()
{
    Q_EMIT stop();
}

bool MediaPlayer2Player::CanPlay() const
{
    return m_canPlay;
}

void MediaPlayer2Player::Play()
{
    Q_EMIT play();
}

double MediaPlayer2Player::Volume() const
{
    return m_volume;
}

void MediaPlayer2Player::setVolume(double volume)
{
    m_volume = qBound(0.0, volume, 1.0);
    Q_EMIT volumeChanged(m_volume);

    signalPropertiesChange(QStringLiteral("Volume"), Volume());
}

void MediaPlayer2Player::setPropertyPosition(int newPositionInMs)
{
    // TODO
}

QVariantMap MediaPlayer2Player::Metadata() const
{
    return m_metadata;
}

qlonglong MediaPlayer2Player::Position() const
{
    return m_position;
}

double MediaPlayer2Player::Rate() const
{
    return m_rate;
}

void MediaPlayer2Player::setRate(double newRate)
{
    if (newRate <= 0.0001 && newRate >= -0.0001) {
        Pause();
    } else {
        m_rate = qBound(MinimumRate(), newRate, MaximumRate());
        Q_EMIT rateChanged(m_rate);

        signalPropertiesChange(QStringLiteral("Rate"), Rate());
    }
}

double MediaPlayer2Player::MinimumRate() const
{
    return MIN_RATE;
}

double MediaPlayer2Player::MaximumRate() const
{
    return MAX_RATE;
}

bool MediaPlayer2Player::CanSeek() const
{
    return m_playerIsSeekableChanged;
}

bool MediaPlayer2Player::CanControl() const
{
    return true;
}

void MediaPlayer2Player::Seek(qlonglong Offset)
{
    Q_EMIT seek(Offset);
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &trackId, qlonglong pos)
{
    Q_EMIT positionRequested(pos);
}

void MediaPlayer2Player::OpenUri(const QString &uri)
{
    Q_UNUSED(uri);
}

//void MediaPlayer2Player::playerSourceChanged()
//{
//    setCurrentTrack(m_manageAudioPlayer->playListPosition());
//}

void MediaPlayer2Player::playControlEnabledChanged()
{
    signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
    signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());

    Q_EMIT canPauseChanged();
    Q_EMIT canPlayChanged();
}

void MediaPlayer2Player::skipBackwardControlEnabledChanged()
{
    signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
    Q_EMIT canGoPreviousChanged();
}

void MediaPlayer2Player::skipForwardControlEnabledChanged()
{
    signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
    Q_EMIT canGoNextChanged();
}

void MediaPlayer2Player::playerPlaybackStateChanged()
{
    signalPropertiesChange(QStringLiteral("PlaybackStatus"), PlaybackStatus());
    Q_EMIT playbackStatusChanged();

    playerIsSeekableChanged();
}

void MediaPlayer2Player::playerIsSeekableChanged()
{
    signalPropertiesChange(QStringLiteral("CanSeek"), CanSeek());
    Q_EMIT canSeekChanged();
}

void MediaPlayer2Player::audioPositionChanged()
{
    setPropertyPosition(m_position);
}

void MediaPlayer2Player::playerSeeked(qint64 position)
{
    Q_EMIT Seeked(position * 1000);
}

void MediaPlayer2Player::audioDurationChanged()
{
    m_metadata = getMetadataOfCurrentTrack();
    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());

    skipBackwardControlEnabledChanged();
    skipForwardControlEnabledChanged();
    playerPlaybackStateChanged();
    playerIsSeekableChanged();
    setPropertyPosition(m_position);
}

QVariantMap MediaPlayer2Player::getMetadataOfCurrentTrack()
{
    auto result = QVariantMap();

    if (m_currentTrackId.isEmpty()) {
        return {};
    }

    result[QStringLiteral("mpris:trackid")] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(m_currentTrackId));
    result[QStringLiteral("mpris:length")] = m_duration * 1000;
    //convert milli-seconds into micro-seconds
    result[QStringLiteral("xesam:title")] = m_currentTrackTitle;
    result[QStringLiteral("xesam:url")] = m_currentTrackUrl;
    result[QStringLiteral("xesam:album")] = m_currentTrackAlbum;
    result[QStringLiteral("xesam:artist")] = m_currentTrackArtist;
    result[QStringLiteral("mpris:artUrl")] = m_coverArtUrl;

    return result;
}

int MediaPlayer2Player::mediaPlayerPresent() const
{
    return m_mediaPlayerPresent;
}

void MediaPlayer2Player::setDuration(qlonglong duration) {
    m_duration = duration;
}

void MediaPlayer2Player::setPlaybackState(PlaybackState newState)
{
    m_playbackState = newState;
}

void MediaPlayer2Player::setCurrentTrackTitle(const QString &title)
{
    m_currentTrackTitle = title;
}

QString MediaPlayer2Player::currentTrackTitle() const
{
    return m_currentTrackTitle;
}

void MediaPlayer2Player::setCurrentTrackUrl(const QUrl &url)
{
    m_currentTrackUrl = url;
}

void MediaPlayer2Player::setCurrentTrackAlbum(const QString &album)
{
    m_currentTrackAlbum = album;
}

QString MediaPlayer2Player::currentTrackAlbum() const
{
    return m_currentTrackAlbum;
}

void MediaPlayer2Player::setCurrentTrackArtist(const QString &artist)
{
    m_currentTrackArtist = artist;
}

QString MediaPlayer2Player::currentTrackArtist() const
{
    return m_currentTrackArtist;
}

void MediaPlayer2Player::setCurrentTrackCoverArt(const QUrl &url)
{
    m_coverArtUrl = url;
}

QUrl MediaPlayer2Player::currentTrackCoverArt() const
{
    return m_coverArtUrl;
}

void MediaPlayer2Player::setCurrentTrackCoverArt(const QImage &image)
{
    QByteArray data;
    QBuffer buffer(&data);
    image.save(&buffer);
    m_coverArtUrl = QUrl(QString("data:image/png;base64," % data));
}

void MediaPlayer2Player::setCanGoPrevious(bool value)
{
    m_canGoPrevious = value;
}

void MediaPlayer2Player::canGoForward(bool value)
{
    m_canGoNext = value;
}

void MediaPlayer2Player::canPause(bool value)
{
    m_canPause = value;
}

void MediaPlayer2Player::canPlay(bool value)
{
    m_canPlay = value;
}

void MediaPlayer2Player::setPosition(qlonglong position) {
    m_position = position;
}

void MediaPlayer2Player::setMediaPlayerPresent(int status)
{
    if (m_mediaPlayerPresent != status) {
        m_mediaPlayerPresent = status;
        Q_EMIT mediaPlayerPresentChanged();

        signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
        signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
        signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
        signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
        Q_EMIT canGoNextChanged();
        Q_EMIT canGoPreviousChanged();
        Q_EMIT canPauseChanged();
        Q_EMIT canPlayChanged();
    }
}

void MediaPlayer2Player::signalPropertiesChange(const QString &property, const QVariant &value)
{
    QVariantMap properties;
    properties[property] = value;
    const int ifaceIndex = metaObject()->indexOfClassInfo("D-Bus Interface");
    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("PropertiesChanged"));

    msg << QLatin1String(metaObject()->classInfo(ifaceIndex).value());
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

#include "moc_mediaplayer2player.cpp"
