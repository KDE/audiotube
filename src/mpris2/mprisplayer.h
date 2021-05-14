#ifndef MPRISPLAYER_H
#define MPRISPLAYER_H

#include <QObject>
#include <QString>

#include <memory>

class PlayerAdaptor;
class QDBusObjectPath;
class MprisTrack {};

class MprisPlayer : public QObject
{
    Q_OBJECT

    friend class PlayerAdaptor;

public:
    explicit MprisPlayer(QObject *parent = nullptr);
    ~MprisPlayer();

    void setCanQuit(bool canQuit);
    void setCanRaise(bool canRaise);
    void setCanSetFullscreen(bool canSetFullscreen);
    void setDesktopEntry(const QString &entry);
    void setFullscreen(bool fullscreen);
    void setHasTrackList(bool hasTrackList);
    void setIdentity(const QString &playerName);
    void setSupportedMimeTypes(const QStringList &mimeTypes);
    void setSupportedUriSchemes(const QStringList &uriSchemes);

protected:
    void Next();
    void OpenUri(const QString &uri);
    void Pause();
    void Play();
    void PlayPause();
    void Previous();
    void Seek(qlonglong offset);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong position);
    void Stop();

Q_SIGNALS:
    void nextRequested();
    void openUriRequested(const QString &uri);
    void pauseRequested();
    void playRequested();
    void playPauseRequested();
    void previousRequested();
    void seekRequested(qlonglong offset);
    void setPositionRequested(const MprisTrack &track, qlonglong position);
    void stopRequested();

private:
    std::unique_ptr<PlayerAdaptor> m_adaptor;
};

#endif // MPRISPLAYER_H
