// SPDX-FileCopyrightText: 2024 Kavinu Nethsara <whjjackwhite@gmail.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "lyricsmodel.h"

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QAbstractListModel>

#include "library.h"

using namespace Qt::Literals::StringLiterals;

LyricsModel::LyricsModel(QObject *parent) : QAbstractListModel(parent) {}

int LyricsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_lyrics.count();
}

QVariant LyricsModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Line:
            return m_lyrics[index.row()].text;
        case Start:
            return m_lyrics[index.row()].start;
        case End:
            return m_lyrics[index.row()].end;
    }

    Q_UNREACHABLE();

    return {};
}

QHash<int, QByteArray> LyricsModel::roleNames() const
{
    return {
        {Line, "line"},
        {Start, "start"},
        {End, "end"}
    };
}

void LyricsModel::fetchLyrics() {
    QUrl url(u"https://lrclib.net/"_s);
    url.setPath(u"/api/get"_s);

    QUrlQuery query;
    query.addQueryItem(u"track_name"_s, m_song);
    query.addQueryItem(u"artist_name"_s, m_artist);
    query.addQueryItem(u"album_name"_s, m_album);
    query.addQueryItem(u"duration"_s, QString::number(m_duration));

    url.setQuery(query);

    QNetworkReply *reply = Library::instance().nam().get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [=, this] {
        setLyrics(reply);
    });
    connect(reply, &QNetworkReply::error, this, [=, this] {
        setLyrics(reply);
    });
}

void LyricsModel::setLyrics(QNetworkReply *reply) {
    beginResetModel();
    auto content = QJsonDocument::fromJson(reply->readAll()).object();
    QList<SyncLine> syncLyrics{};
    if (content.contains(u"syncedLyrics"_s)) {
        syncLyrics = parseSyncedLyrics(content[u"syncedLyrics"_s].toString());
    }
    m_lyrics = syncLyrics;
    Q_EMIT endResetModel();
    Q_EMIT countChanged();
}

QList<SyncLine> LyricsModel::parseSyncedLyrics(const QString &synclrc) {
    QRegularExpression regex{uR"(\[(\d{2}):(\d{2})\.(\d{2})\]\s*(.*))"_s};
    QList<SyncLine> lyrics;
    QStringList lines = synclrc.split("\n", Qt::KeepEmptyParts);
    for (int i = 0; i < lines.count(); i++) {
        auto match = regex.match(lines[i]);

        double time = 0;
        double finishTime = m_duration;
        QString line;

        if (match.hasMatch()) {
            int minutes = match.captured(1).toInt();
            int seconds = match.captured(2).toInt();
            int centiseconds = match.captured(3).toInt();
            time = minutes * 60 + seconds + centiseconds * 0.01;
            line = match.captured(4);
        }

        if (i < lines.count() - 1){
            auto futurematch = regex.match(lines[i + 1]);

            int minutes = futurematch.captured(1).toInt();
            int seconds = futurematch.captured(2).toInt();
            int centiseconds = futurematch.captured(3).toInt();
            finishTime = minutes * 60 + seconds + centiseconds * 0.01;
        }

        lyrics.push_back(SyncLine {
            time,
            finishTime,
            line
        });
    }
    return lyrics;
}

int LyricsModel::count() const {
    return m_lyrics.count();
}
