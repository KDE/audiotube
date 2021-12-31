#include "library.h"

#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringBuilder>

#include <qpersistentdata.h>

Library::Library(QObject *parent)
    : QObject{parent}
    , m_storage("library")
    , m_cachedVideoTitles("videoTitles")
{
    connect(this, &Library::favouritesChanged, this, []() {
        qDebug() << "Favs changed" ;
    });
}

Library &Library::instance()
{
    static Library inst;
    return inst;
}

QStringList Library::favourites() const
{
    return m_storage.value<QStringList>("favourites");
}

void Library::addFavourite(const QString &videoId)
{
    auto favs = favourites();
    favs.push_front(videoId);
    m_storage.insert("favourites", favs);
    Q_EMIT favouritesChanged();
}

QStringList Library::searches() const
{
    auto s = m_storage.value<QStringList>("searches");
    qDebug() << "searches" << s;
    return s;
}

void Library::addSearch(const QString &text)
{
    m_storage.insert("searches", searches() << text);
    Q_EMIT searchesChanged();
}

QStringList Library::playbackHistory() const
{
    auto h = m_storage.value<QStringList>("playbackHistory");
    return h;
}

void Library::addPlaybackHistoryItem(const QString &videoId)
{
    m_storage.insert("playbackHistory", playbackHistory() << videoId);
    Q_EMIT playbackHistoryChanged();
}

QString Library::videoTitle(const QString &videoId)
{
    qDebug() << "Fetching video title" << videoId;
    auto t = m_cachedVideoTitles.value<QString>(videoId);
    qDebug() << "title" << t;
    return t;
}

void Library::addVideoTitle(const QString &videoId, const QString &title)
{
    m_cachedVideoTitles.insert(videoId, title);
}

QNetworkAccessManager &Library::nam()
{
    return m_networkImageCacher;
}

void ThumbnailSource::setVideoId(const QString &id) {
    if (m_videoId == id) {
        return;
    }

    m_videoId = id;
    Q_EMIT videoIdChanged();

    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) % QDir::separator() % "thumbnails";
    QDir(cacheDir).mkpath(QStringLiteral("."));
    const QString cacheLocation = cacheDir % QDir::separator() % id % ".webp";

    if (QFile::exists(cacheLocation)) {
        setCachedPath(QUrl::fromLocalFile(cacheLocation));
        return;
    }

    auto future = YTMusicThread::instance()->extractVideoInfo(id);
    connectFuture(future, this, [this, id, cacheLocation](video_info::VideoInfo &&info) {
        auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl(QString::fromStdString(info.thumbnail))));
        connect(reply, &QNetworkReply::finished, this, [id, reply, this, cacheLocation]() {
            QFile file(cacheLocation);
            file.open(QFile::WriteOnly);
            file.write(reply->readAll());
            setCachedPath(QUrl::fromLocalFile(cacheLocation));
        });
    });
}
