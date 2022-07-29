#include "declarativemprisplayer.h"

#include <QTimer>

DeclarativeMprisPlayer::DeclarativeMprisPlayer()
{
    QTimer::singleShot(0, [this]() {
        connect(m_mpris.player(), &MediaPlayer2Player::play, this, &DeclarativeMprisPlayer::play);
        connect(m_mpris.player(), &MediaPlayer2Player::playPause, this, &DeclarativeMprisPlayer::playPause);
        connect(m_mpris.player(), &MediaPlayer2Player::Stop, this, &DeclarativeMprisPlayer::stop);
    });
}

QString DeclarativeMprisPlayer::playerName() const {
    return m_mpris.playerName();
}

void DeclarativeMprisPlayer::setPlayerName(const QString &name) {
    m_mpris.setPlayerName(name);
    Q_EMIT playerNameChanged();
}

DeclarativeMprisPlayer::PlaybackState DeclarativeMprisPlayer::playbackState() {
    return PlaybackState(m_mpris.player()->playbackState());
}

void DeclarativeMprisPlayer::setPlaybackState(PlaybackState state) {
    m_mpris.player()->setPlaybackState(MediaPlayer2Player::PlaybackState(state));
}

void DeclarativeMprisPlayer::setAlbum(const QString &album) {
    m_mpris.player()->setCurrentTrackAlbum(album);
}

QString DeclarativeMprisPlayer::album() const {
    return m_mpris.player()->currentTrackAlbum();
}

QUrl DeclarativeMprisPlayer::albumArt() const {
    return m_mpris.player()->currentTrackCoverArt();
}

void DeclarativeMprisPlayer::setAlbumArt(const QUrl &url) {
    m_mpris.player()->setCurrentTrackCoverArt(url);
}

QString DeclarativeMprisPlayer::trackTitle() const
{
    return m_mpris.player()->currentTrackTitle();
}

void DeclarativeMprisPlayer::setTrackTitle(const QString &title)
{
    m_mpris.player()->setCurrentTrackTitle(title);
}

void DeclarativeMprisPlayer::setTrackDuration(qlonglong trackDuration)
{
    return m_mpris.player()->setDuration(trackDuration);
}
qlonglong DeclarativeMprisPlayer::trackDuration() const { return 0; }
