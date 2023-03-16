// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "thumbnailsource.h"

#include <QStandardPaths>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QtConcurrent>
#include <QGuiApplication>
#include <QImage>
#include <QStringBuilder>

#include "asyncytmusic.h"
#include "library.h"

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

    auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl("https://i.ytimg.com/vi_webp/" % m_videoId % "/maxresdefault.webp")));

    auto storeResult = [this, cacheLocation](QNetworkReply *reply) {
        auto data = reply->readAll();
        reply->deleteLater();
        auto future = QtConcurrent::run([data = std::move(data), cacheLocation]() {
            // Scale cover down to save qmemory
            int targetHeight = 200 * qGuiApp->devicePixelRatio();
            auto scaled = QImage::fromData(data)
                .scaledToHeight(targetHeight);

            int targetLeft = scaled.width() / 2 - targetHeight / 2;
            auto cropped = scaled
                .copy(QRect(targetLeft, 0, targetHeight, targetHeight));

            cropped.save(cacheLocation);
        });

        connectFuture(future, this, [this, cacheLocation]() {
            setCachedPath(QUrl::fromLocalFile(cacheLocation));
        });
    };

    connect(reply, &QNetworkReply::errorOccurred, this, [this, storeResult](auto error) {
        if (error == QNetworkReply::NetworkError::ContentNotFoundError) {
            qDebug() << "Naive thumbnail resolution failed, falling back to yt-dlp (slower)";

            connectFuture(YTMusicThread::instance()->extractVideoInfo(m_videoId), this, [this, storeResult](auto info) {
                auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl(QString::fromStdString(info.thumbnail))));
                connect(reply, &QNetworkReply::finished, this, [reply, storeResult]() {
                    storeResult(reply);
                });
            });
        }
    });

    connect(reply, &QNetworkReply::finished, this, [reply, storeResult]() {
        storeResult(reply);
    });
}
