// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "downloadmanager.h"

#include <QUrl>
#include <QFile>
#include <QStandardPaths>
#include <QStringBuilder>

#include <QCoroNetworkReply>
#include <QFile>

#include "asyncytmusic.h"
#include "library.h"
#include "qdir.h"

constexpr auto BUFFER_SIZE = 4096;

DownloadManager::DownloadManager(QObject *parent)
    : QObject{parent}
{

}

QCoro::QmlTask DownloadManager::downloadSong(const QString &videoId)
{
    return download(videoId);
}

void DownloadManager::deleteDownload(const QString &videoId)
{
    QString directory = downloadDirectory();
    QFile::remove(directory % videoId);

    auto future = Library::instance().markSongDownloaded(videoId, false);
    QCoro::connect(std::move(future), &Library::instance(), [videoId]() {
        Q_EMIT Library::instance().downloadedChanged(videoId);
    });
}

QString DownloadManager::localPathOf(const QString &videoId)
{
    return downloadDirectory() % "/" % videoId;
}

QCoro::Task<> DownloadManager::download(const QString videoId)
{
    auto info = co_await YTMusicThread::instance()->extractVideoInfo(videoId);

    // If we don't know anything about the song yet, save what we know
    // This allows searching for the song if it was just downloaded
    QCoro::connect(Library::instance().getSong(videoId), this, [=](auto &&song) {
        if (!song) {
            Library::instance().addSong(videoId, QString::fromStdString(info.title), QString::fromStdString(info.artist), {});
        }
    });

    QUrl url = pickAudioUrl(info.formats);
    qDebug() << url;

    auto *reply = Library::instance().nam().get(QNetworkRequest(url));

    QString directory = downloadDirectory();
    QDir(directory).mkpath(".");

    QString downloadLocation = directory % "/" % videoId % ".part";

    auto *file = new QFile(downloadLocation);
    if (!file->open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "Failed to open" << downloadLocation << "for writing";
        co_return;
    }

    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        QByteArray buffer;
        buffer.reserve(BUFFER_SIZE);

        int read;
        do {
            read = reply->read(buffer.data(), BUFFER_SIZE);
            file->write(buffer.data(), read);
        } while (read > 0);
    });
    co_await reply;

    file->close();
    file->deleteLater();

    QString location = directory % "/" % videoId;
    QFile::rename(downloadLocation, location);

    Library::instance().markSongDownloaded(videoId, true);
    Q_EMIT Library::instance().downloadedChanged(videoId);
}

QString DownloadManager::downloadDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) % "/downloads/";
}

DownloadedWatcher::DownloadedWatcher(QObject *parent)
    : QObject(parent)
{
    auto update = [this]() {
        qDebug() << "checking downloaded";
        auto future = Library::instance().songDownloaded(m_videoId);

        QCoro::connect(std::move(future), this, [this](auto downloaded) {
            m_downloaded = downloaded;
            qDebug() << "downloaded" << m_downloaded;
            Q_EMIT downloadedChanged();
        });
    };

    connect(this, &DownloadedWatcher::videoIdChanged, this, update);

    connect(&Library::instance(), &Library::downloadedChanged, this, [=, this](const QString &videoId) {
        if (m_videoId == videoId) {
            update();
        }
    });
    update();
}
