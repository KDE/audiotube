// SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
// SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
// SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
// SPDX-FileCopyrightText: 2022 (c) Jonah Brüchert <jbb@kaidan.im>

// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QUrl>

class MediaPlayListProxyModel;
class ManageAudioPlayer;
class ManageMediaPlayerControl;
class ManageHeaderBar;
class AudioWrapper;

class MediaPlayer2Player : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player") // Docs: https://specifications.freedesktop.org/mpris-spec/latest/Player_Interface.html

    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(double Rate READ Rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(QVariantMap Metadata READ Metadata NOTIFY playbackStatusChanged)
    Q_PROPERTY(double Volume READ Volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qlonglong Position READ Position WRITE setPropertyPosition NOTIFY playbackStatusChanged)
    Q_PROPERTY(double MinimumRate READ MinimumRate CONSTANT)
    Q_PROPERTY(double MaximumRate READ MaximumRate CONSTANT)
    Q_PROPERTY(bool CanGoNext READ CanGoNext NOTIFY canGoNextChanged)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious NOTIFY canGoPreviousChanged)
    Q_PROPERTY(bool CanPlay READ CanPlay NOTIFY canPlayChanged)
    Q_PROPERTY(bool CanPause READ CanPause NOTIFY canPauseChanged)
    Q_PROPERTY(bool CanControl READ CanControl NOTIFY canControlChanged)
    Q_PROPERTY(bool CanSeek READ CanSeek NOTIFY canSeekChanged)
    Q_PROPERTY(int mediaPlayerPresent READ mediaPlayerPresent WRITE setMediaPlayerPresent NOTIFY mediaPlayerPresentChanged)

public:
    explicit MediaPlayer2Player(QObject* parent = nullptr);
    ~MediaPlayer2Player() override;

    QString PlaybackStatus() const;
    double Rate() const;
    QVariantMap Metadata() const;
    double Volume() const;
    qlonglong Position() const;
    double MinimumRate() const;
    double MaximumRate() const;
    bool CanGoNext() const;
    bool CanGoPrevious() const;
    bool CanPlay() const;
    bool CanPause() const;
    bool CanSeek() const;
    bool CanControl() const;
    int currentTrack() const;
    int mediaPlayerPresent() const;

    void setDesktopFileName(const QString &fileName);

    ///
    /// Signal to the mpris interface that the playback is now at a new position
    ///
    void setPosition(qlonglong pos);

    ///
    /// Signal the length of the current track to the mpris interface
    ///
    void setDuration(qlonglong duration);

    enum PlaybackState {
        PlaybackStopped,
        PlaybackRunning,
        PlaybackPaused
    };

    ///
    /// Set the current playback state
    ///
    void setPlaybackState(PlaybackState newState);

    ///
    /// Set the current track's title
    ///
    void setCurrentTrackTitle(const QString &title);

    ///
    /// Set the current track's URL
    ///
    void setCurrentTrackUrl(const QUrl &url);

    ///
    /// Set the album of the currently playing track
    ///
    void setCurrentTrackAlbum(const QString &album);

    ///
    /// Set current track's artist
    ///
    void setCurrentTrackArtist(const QString &artist);

    ///
    /// Set cover art url
    ///
    void setCurrentTrackCoverArt(const QUrl &url);

    ///
    /// Set cover art image
    ///
    void setCurrentTrackCoverArt(const QImage &image);

    void setCanGoPrevious(bool);
    void canGoForward(bool);
    void canPause(bool value);
    void canPlay(bool value);

Q_SIGNALS:
    void Seeked(qlonglong Position);

    void rateChanged(double newRate);

    ///
    /// Emitted when the volume should be changed to newVol
    ///
    void volumeChanged(double newVol);
    void playbackStatusChanged();
    void canGoNextChanged();
    void canGoPreviousChanged();
    void canPlayChanged();
    void canPauseChanged();
    void canControlChanged();
    void canSeekChanged();
    void currentTrackChanged();
    void mediaPlayerPresentChanged();

    ///
    /// Emitted when the player should skip to the next track
    ///
    void next();

    ///
    /// Emitted when it is requested to skip to the previous track
    /// over the MPRIS interface
    ///
    void previous();

    ///
    /// Emitted when play / pause should be toggled
    ///
    void playPause();

    ///
    /// Emitted when the playback should be stopped
    ///
    void stop();

    ///
    /// Emitted when playback should be resumed
    ///
    void play();

    ///
    /// Emitted when the player should seek
    ///
    void seek(qlonglong offset);

    ///
    /// Emitted when the player should skip to the given position
    ///
    void positionRequested(qlonglong position);

public Q_SLOTS:

    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qlonglong Offset);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong pos);
    void OpenUri(const QString &uri);

private Q_SLOTS:

    void playControlEnabledChanged();

    void skipBackwardControlEnabledChanged();

    void skipForwardControlEnabledChanged();

    void playerPlaybackStateChanged();

    void playerIsSeekableChanged();

    void audioPositionChanged();

    void playerSeeked(qint64 position);

    void audioDurationChanged();

private:
    void signalPropertiesChange(const QString &property, const QVariant &value);

    void setMediaPlayerPresent(int status);
    void setRate(double newRate);
    void setVolume(double volume);
    void setPropertyPosition(int newPositionInMs);

    QVariantMap getMetadataOfCurrentTrack();

    QVariantMap m_metadata;
    QString m_currentTrack;
    QString m_currentTrackId;
    double m_rate = 1.0;
    double m_volume = 0.0;
    int m_mediaPlayerPresent = 0;
    bool m_canPause = true;
    bool m_canPlay = false;
    bool m_canGoNext = false;
    bool m_canGoPrevious = false;
    qlonglong m_position = 0;
    bool m_playerIsSeekableChanged = false;
    mutable QDBusMessage mProgressIndicatorSignal;
    int mPreviousProgressPosition = 0;

    // Current track metadata
    qlonglong m_duration;
    PlaybackState m_playbackState = PlaybackStopped;
    QString m_currentTrackTitle;
    QUrl m_currentTrackUrl;
    QString m_currentTrackAlbum;
    QString m_currentTrackArtist;
    QUrl m_coverArtUrl;
};
