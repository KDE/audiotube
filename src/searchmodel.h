#pragma once


#include <QAbstractListModel>
#include <QThread>

#include "asyncytmusic.h"

class SearchModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)

public:
    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_SIGNAL void searchQueryChanged();

    QString searchQuery() const;
    void setSearchQuery(const QString &searchQuery);

private:
    QString m_searchQuery;
    std::vector<search::SearchResultItem> m_searchResults;

    AsyncYTMusic m_ytm;
    QThread *m_thread;
};
