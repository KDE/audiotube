// SPDX-FileCopyrightText: 2024 Kavinu Nethsara <whjjackwhite@gmail.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QtQml>
#include <QString>
#include <QList>
#include <QAbstractListModel>
#include <QtConcurrent>
#include <QNetworkAccessManager>

struct SyncLine {
    double start;
    double end;
    QString text;
};

class LyricsModel: public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString song MEMBER m_song NOTIFY songChanged)
    Q_PROPERTY(QString artist MEMBER m_artist NOTIFY artistChanged)
    Q_PROPERTY(QString album MEMBER m_album NOTIFY albumChanged)
    Q_PROPERTY(int duration MEMBER m_duration NOTIFY durationChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:

    enum Role {
        Line,
        Start,
        End
    };
    Q_ENUM(Role);

    explicit LyricsModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    QVariant data(const QModelIndex &parent = QModelIndex(), int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void fetchLyrics();

    Q_SIGNAL void songChanged();
    Q_SIGNAL void artistChanged();
    Q_SIGNAL void albumChanged();
    Q_SIGNAL void durationChanged();
    Q_SIGNAL void countChanged();

private:
    void setLyrics(QNetworkReply *reply);
    QList<SyncLine> parseSyncedLyrics(const QString &synclrc);

    QString m_song;
    QString m_artist;
    QString m_album;
    int m_duration = 0;
    QList<SyncLine> m_lyrics{};
};
