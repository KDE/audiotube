#pragma once

#include <QObject>
#include <QUrl>

#include <ytmusic.h>

class VideoInfoExtractor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged REQUIRED)

    Q_PROPERTY(QUrl audioUrl READ audioUrl NOTIFY audioUrlChanged)
    Q_PROPERTY(QString Title READ title NOTIFY titleChanged)

public:
    VideoInfoExtractor(QObject *parent = nullptr);

    QUrl audioUrl() const;
    Q_SIGNAL void audioUrlChanged();

    QString videoId() const;
    void setVideoId(const QString &videoId);
    Q_SIGNAL void videoIdChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

private:
    QString m_videoId;
    video_info::VideoInfo m_videoInfo;
};
