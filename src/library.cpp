#include "library.h"

#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringBuilder>

#include <lib.rs.h>

namespace ranges = std::ranges;

Library::Library(QObject *parent)
    : QObject{parent}
    , m_database(new_library_database(
        QString(
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) % QDir::separator() % "library.sqlite")
                .toStdString()))
{
}

Library &Library::instance()
{
    static Library inst;
    return inst;
}

FavouritesModel *Library::favourites()
{
    return new FavouritesModel(m_database->favourites(), this);
}

void Library::addFavourite(const QString &videoId, const QString &title, const QString &artist)
{
    m_database->add_favourite(Song {
        videoId.toStdString(),
        title.toStdString(),
        artist.toStdString()
    });
}

void Library::removeFavourite(const QString &videoid)
{
    m_database->remove_favourite(videoid.toStdString());
    Q_EMIT favouritesChanged();
}

bool Library::isFavourited(const QString &videoId)
{
    return m_database->is_favourited(videoId.toStdString());
}

QStringList Library::searches() const
{
    QStringList out;
    const auto searches = m_database->searches();

    ranges::transform(searches, std::back_inserter(out), [](auto string) {
        return QString::fromStdString(std::string(string));
    });

    return out;
}

void Library::addSearch(const QString &text)
{
    m_database->add_search(text.toStdString());
    Q_EMIT searchesChanged();
}

PlaybackHistoryModel *Library::playbackHistory()
{
    return new PlaybackHistoryModel(m_database->plays(), this);
}

void Library::addPlaybackHistoryItem(const QString &videoId, const QString &title, const QString &artist)
{
    m_database->increment_plays(Song {
        videoId.toStdString(),
        title.toStdString(),
        artist.toStdString()
    });
    Q_EMIT playbackHistoryChanged();
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
