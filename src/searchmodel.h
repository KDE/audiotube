#pragma once


#include <QAbstractListModel>
#include <QThread>

#include "asyncytmusic.h"

class SearchModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QString searchQuery() const;
    void setSearchQuery(const QString &searchQuery);
    Q_SIGNAL void searchQueryChanged();

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

private:
    QString m_searchQuery;
    std::vector<search::SearchResultItem> m_searchResults;
    bool m_loading = false;

    AsyncYTMusic m_ytm;
    QThread *m_thread;
};
