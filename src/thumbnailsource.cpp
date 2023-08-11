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
    setCachedPath({});

    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) % QDir::separator() % "thumbnails";
    QDir(cacheDir).mkpath(QStringLiteral("."));

    // Clear cache if it is old, so people can profit from memory usage improvements from downscaling,
    // and get the new cropped thumbnails
    auto cacheVersionFile = QString(cacheDir % "/.cache_version");

    constexpr auto CURRENT_CACHE_VERSION = 1;

    auto getCacheVersion = [cacheVersionFile]() {
        QFile file(cacheVersionFile);
        if (!file.open(QFile::ReadOnly)) {
            return 0;
        }
        auto version = file.read(3); // Read at most three characters, we will not need more soon
        return version.toInt();
    };

    if (!QFile::exists(cacheVersionFile) || getCacheVersion() < CURRENT_CACHE_VERSION) {
        qDebug() << "Deleting and re-generating thumbnail cache";

        QDir dir(cacheDir);
        const auto entries = dir.entryList(QDir::Files);
        for (const auto &thumbnail : entries) {
            if (thumbnail.endsWith(QLatin1String(".webp"))) {
                QFile::remove(cacheDir % "/" % thumbnail);
            }
        }
        QFile file(cacheVersionFile);
        if (file.open(QFile::WriteOnly)) {
            file.seek(0);
            file.write(QString::number(CURRENT_CACHE_VERSION).toUtf8());
        }
    }

    const QString cacheLocation = cacheDir % QDir::separator() % id % ".webp";

    if (QFile::exists(cacheLocation)) {
        setCachedPath(QUrl::fromLocalFile(cacheLocation));
        return;
    }

    auto *reply = Library::instance().nam().get(QNetworkRequest(QUrl("https://i.ytimg.com/vi_webp/" % m_videoId % "/maxresdefault.webp")));

    auto storeResult = [this, cacheLocation, id](QNetworkReply *reply) {
        if (reply->error() != QNetworkReply::NoError) {
            return;
        }
        auto data = reply->readAll();
        reply->deleteLater();
        auto future = QtConcurrent::run([data = std::move(data), cacheLocation]() {
            // Scale cover down to save memory
            int targetHeight = 200 * qGuiApp->devicePixelRatio();
            auto scaled = QImage::fromData(data)
                .scaledToHeight(targetHeight);

            int targetLeft = scaled.width() / 2 - targetHeight / 2;
            auto cropped = scaled
                .copy(QRect(targetLeft, 0, targetHeight, targetHeight));

            cropped.save(cacheLocation);
        });

        QCoro::connect(std::move(future), this, [this, cacheLocation, id]() {
            // Check if video id was changed since we started fetching
            if (id == m_videoId) {
                setCachedPath(QUrl::fromLocalFile(cacheLocation));
            }
        });
    };

    connect(reply, &QNetworkReply::errorOccurred, this, [this, storeResult](auto error) {
        if (error == QNetworkReply::NetworkError::ContentNotFoundError) {
            qDebug() << "Naive thumbnail resolution failed, falling back to yt-dlp (slower)";

            QCoro::connect(YTMusicThread::instance()->extractVideoInfo(m_videoId), this, [this, storeResult](auto info) {
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
