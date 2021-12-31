#pragma once

#include <QObject>
#include <QQuickAsyncImageProvider>
#include <QNetworkAccessManager>
#include <qpersistentdata.h>

#include "asyncytmusic.h"

class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList favourites READ favourites NOTIFY favouritesChanged)
    Q_PROPERTY(QStringList searches READ searches NOTIFY searchesChanged)
    Q_PROPERTY(QStringList playbackHistory READ playbackHistory NOTIFY playbackHistoryChanged)

public:
    explicit Library(QObject *parent = nullptr);

    static Library &instance();

    QStringList favourites() const;
    Q_SIGNAL void favouritesChanged();
    Q_INVOKABLE void addFavourite(const QString &videoId);

    QStringList searches() const;
    Q_SIGNAL void searchesChanged();
    Q_INVOKABLE void addSearch(const QString &text);

    QStringList playbackHistory() const;
    Q_SIGNAL void playbackHistoryChanged();
    Q_INVOKABLE void addPlaybackHistoryItem(const QString &videoId);

    Q_INVOKABLE QString videoTitle(const QString &videoId);
    Q_INVOKABLE void addVideoTitle(const QString &videoId, const QString &title);

    QNetworkAccessManager &nam();

private:
    QNetworkAccessManager m_networkImageCacher;
    QPersistentData m_storage;
    QPersistentData m_cachedVideoTitles;
};


class ThumbnailSource : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged)
    Q_PROPERTY(QUrl cachedPath READ cachedPath NOTIFY cachedPathChanged)

    QString videoId() const {
        return m_videoId;
    }
    void setVideoId(const QString &id);
    Q_SIGNAL void videoIdChanged();

    QUrl cachedPath() const {
        return m_cachedPath;
    }
    void setCachedPath(const QUrl &path) {
        m_cachedPath = path;
        cachedPathChanged();
    }
    Q_SIGNAL void cachedPathChanged();

private:
    QString m_videoId;
    QUrl m_cachedPath;
};
