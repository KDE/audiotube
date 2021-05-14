#ifndef MPRISAPPLICATION_H
#define MPRISAPPLICATION_H

#include <QObject>

#include <memory>

struct MprisApplicationPrivate;

class MprisApplication : public QObject
{
    Q_OBJECT

    // DBus property names
    Q_PROPERTY(bool CanQuit READ canQuit NOTIFY canQuitChanged)
    Q_PROPERTY(bool CanRaise READ canRaise NOTIFY canRaiseChanged)
    Q_PROPERTY(bool CanSetFullscreen READ canSetFullscreen NOTIFY canSetFullscreenChanged)
    Q_PROPERTY(QString DesktopEntry READ desktopEntry NOTIFY desktopEntryChanged)
    Q_PROPERTY(bool Fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool HasTrackList READ hasTrackList NOTIFY hasTrackListChanged)
    Q_PROPERTY(QString Identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes NOTIFY supportedMimeTypesChanged)
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes NOTIFY supportedUriSchemes)

    // QML property names
    Q_PROPERTY(bool canQuit READ canQuit WRITE setCanQuit NOTIFY canQuitChanged)
    Q_PROPERTY(bool canRaise READ canRaise WRITE setCanRaise NOTIFY canRaiseChanged)
    Q_PROPERTY(bool canSetFullscreen READ canSetFullscreen NOTIFY canSetFullscreenChanged)
    Q_PROPERTY(QString desktopEntry READ desktopEntry NOTIFY desktopEntryChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool hasTrackList READ hasTrackList NOTIFY hasTrackListChanged)
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QStringList supportedMimeTypes READ supportedMimeTypes NOTIFY supportedMimeTypesChanged)
    Q_PROPERTY(QStringList supportedUriSchemes READ supportedUriSchemes NOTIFY supportedUriSchemes)

    friend class MediaPlayer2Adaptor;

public:
    explicit MprisApplication(const QString &appId, QObject *parent = nullptr);
    ~MprisApplication();

    bool canQuit() const;
    void setCanQuit(bool canQuit);
    Q_SIGNAL void canQuitChanged();

    bool canRaise() const;
    void setCanRaise(bool canRaise);
    Q_SIGNAL void canRaiseChanged();

    bool canSetFullscreen() const;
    void setCanSetFullscreen(bool can);
    Q_SIGNAL void canSetFullscreenChanged();

    QString desktopEntry() const;
    void setDesktopEntry(const QString &entry);
    Q_SIGNAL void desktopEntryChanged();

    bool fullscreen() const;
    void setFullscreen(bool value);
    Q_SIGNAL void fullscreenChanged();

    bool hasTrackList() const;
    void setHasTrackList(bool has);
    Q_SIGNAL void hasTrackListChanged();

    QString identity() const;
    void setIdentity(const QString &appName);
    Q_SIGNAL void identityChanged();

    QStringList supportedMimeTypes() const;
    void setSupportedMimeTypes(const QStringList &types);
    Q_SIGNAL void supportedMimeTypesChanged();

    QStringList supportedUriSchemes() const;
    void setSupportedUriSchemes(const QStringList &schemes);
    Q_SIGNAL void supportedUriSchemesChanged();

protected:
    void Quit();
    void Raise();

Q_SIGNALS:
    void quitRequested();
    void raiseRequested();

private:
    std::unique_ptr<MprisApplicationPrivate> d;
};

#endif // MPRISAPPLICATION_H
