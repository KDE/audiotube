#pragma once

#include <QAbstractListModel>

#include "asyncytmusic.h"

class AlbumModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString browseId READ browseId WRITE setBrowseId NOTIFY browseIdChanged REQUIRED)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit AlbumModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QString browseId() const;
    void setBrowseId(const QString &value);
    Q_SIGNAL void browseIdChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

private:
    QString m_browseId;
    bool m_loading;

    album::Album m_album;
};
