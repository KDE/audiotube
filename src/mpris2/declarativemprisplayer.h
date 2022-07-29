#pragma once

#include <QObject>

#include "mediaplayer2player.h"
#include "mpris2.h"


class DeclarativeMprisPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName NOTIFY playerNameChanged)
    Q_PROPERTY(PlaybackState playbackState READ playbackState WRITE setPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QString album READ album WRITE setAlbum NOTIFY albumChanged)
    Q_PROPERTY(QUrl albumArt READ albumArt WRITE setAlbumArt NOTIFY albumArtChanged)
    Q_PROPERTY(QString trackTitle READ trackTitle WRITE setTrackTitle NOTIFY trackTitleChanged);
    Q_PROPERTY(qlonglong trackDuration READ trackDuration WRITE setTrackDuration NOTIFY trackDurationChanged);


public:

    enum PlaybackState {
        PlaybackStopped = MediaPlayer2Player::PlaybackStopped,
        PlaybackRunning = MediaPlayer2Player::PlaybackRunning,
        PlaybackPaused = MediaPlayer2Player::PlaybackPaused
    };
    Q_ENUM(PlaybackState)

    DeclarativeMprisPlayer();

    QString playerName() const;
    void setPlayerName(const QString &name);
    Q_SIGNAL void playerNameChanged();

    PlaybackState playbackState();
    void setPlaybackState(PlaybackState state);
    Q_SIGNAL void playbackStateChanged();

    void setAlbum(const QString &album);
    QString album() const;
    Q_SIGNAL void albumChanged();

    QUrl albumArt() const;
    void setAlbumArt(const QUrl &url);
    Q_SIGNAL void albumArtChanged();

    QString trackTitle()  const;
    void setTrackTitle(const QString &title);
    Q_SIGNAL void trackTitleChanged();

    qlonglong trackDuration() const;
    void setTrackDuration(qlonglong trackDuration);
    Q_SIGNAL void trackDurationChanged();

    Q_SIGNAL void changeVolume();
    Q_SIGNAL void play();
    Q_SIGNAL void playPause();
    Q_SIGNAL void stop();
    Q_SIGNAL void goPrevious();
    Q_SIGNAL void goNext();

private:
    Mpris2 m_mpris;
};

