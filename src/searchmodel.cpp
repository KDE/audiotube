#include "searchmodel.h"

#include <QDebug>

SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_thread(new QThread(this))
{
    m_thread->setObjectName("YTMusicThread");
    m_ytm.moveToThread(m_thread);
    m_thread->start();

    connect(this, &SearchModel::searchQueryChanged, this, [=] {
        if (m_searchQuery.isEmpty()) {
            beginResetModel();
            m_searchResults.clear();
            endResetModel();
            return;
        }

        m_ytm.search(m_searchQuery);
    });
    connect(&m_ytm, &AsyncYTMusic::searchFinished, this, [=](const std::vector<search::SearchResultItem> &results) {
        beginResetModel();
        m_searchResults = results;
        endResetModel();
    });
}

SearchModel::~SearchModel()
{
    m_thread->quit();
    m_thread->wait();
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    return m_searchQuery.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return QString::fromStdString(std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, search::Album>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Artist>) {
                return arg.artist;
            } else if constexpr (std::is_same_v<T, search::Playlist>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Song>) {
                return arg.title;
            } else if constexpr (std::is_same_v<T, search::Video>) {
                return arg.title;
            } else {
                return std::string();
            }
        }, m_searchResults.at(index.row())));
    }

    return {};
}

QString SearchModel::searchQuery() const
{
    return m_searchQuery;
}

void SearchModel::setSearchQuery(const QString &searchQuery)
{
    m_searchQuery = searchQuery;
    Q_EMIT searchQueryChanged();
}
