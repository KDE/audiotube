#pragma once

class QUrl;
class QSqlDatabase;

#include <QString>
#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QSqlQuery>
#include <QDebug>
#include <QThread>
#include <QSharedDataPointer>

#include <QCoro/Task>
#include <QCoro/QCoroFuture>

#include <memory>
#include <optional>
#include <tuple>
#include <vector>

#include "asyncdatabase_p.h"

using Row  = std::vector<QVariant>;
using Rows = std::vector<Row>;

struct DatabaseConfigurationPrivate;

///
/// Options for connecting to a database
///
class DatabaseConfiguration {
public:
    DatabaseConfiguration();
    DatabaseConfiguration(const DatabaseConfiguration &);
    ~DatabaseConfiguration();

    /// Set the name of the database driver, for example DATABASE_TYPE_SQLITE or a custom string.
    void setType(const QString &type);
    const QString &type() const;

    /// Set the hostname
    void setHostName(const QString &hostName);
    const std::optional<QString> &hostName() const;

    /// Set the name of the database (path of the file for SQLite)
    void setDatabaseName(const QString &databaseName);
    const std::optional<QString> &databaseName() const;

    /// Set user name
    void setUserName(const QString &userName);
    const std::optional<QString> &userName() const;

    /// Set password
    void setPassword(const QString &password);
    const std::optional<QString> &password() const;

private:
    QSharedDataPointer<DatabaseConfigurationPrivate> d;
};

///
/// The SQLite database driver
///
const QString DATABASE_TYPE_SQLITE = QStringLiteral("QSQLITE");

///
/// Parse the row into a tuple of the given types.
/// The types need to be deserializable from QVariant.
///
template <typename ...RowTypes>
auto parseRow(const Row &row) -> std::tuple<RowTypes...>
{
    auto tuple = std::tuple<RowTypes...>();
    int i = 0;
    asyncdatabase_private::iterate_tuple(tuple, [&](auto &elem) {
        elem = row.at(i).value<std::decay_t<decltype(elem)>>();
        i++;
    });
    return tuple;
}

///
/// Parse the rows into a list of tuples of the given types.
/// The types need to be deserializable from QVariant.
///
template <typename ...RowTypes>
auto parseRows(const Rows &rows) -> std::vector<std::tuple<RowTypes...>> {
    std::vector<std::tuple<RowTypes...>> parsedRows;
    parsedRows.reserve(rows.size());
    for (const auto &row : rows) {
        parsedRows.push_back(parseRow<RowTypes...>(row));
    }

    return parsedRows;
}

class ThreadedDatabase : public QThread {
public:
    static std::unique_ptr<ThreadedDatabase> establishConnection(DatabaseConfiguration &config);

    ///
    /// \brief Execute an SQL query on the database, ignoring the result.
    /// \param SQL query string to execute
    /// \param Parameters to bind to the placeholders in the SQL Query
    /// \return
    ///
    template <typename ...Args>
    auto execute(const QString &sqlQuery, Args... args) -> QFuture<void> {
        return m_db->execute(sqlQuery, args...);
    }

    ///
    /// Run database migrations in the given directory.
    /// The directory needs to contain a subdirectory for each migration.
    /// The subdirectories need to be named so that when sorted alphabetically the migrations will be run in the correct order.
    /// Each subdirectory needs to contain a file named up.sql.
    ///
    /// \param Directory which contains the migrations.
    /// \return
    ///
    auto runMigrations(const QString &migrationDirectory) -> QFuture<void> {
        return m_db->runMigrations(migrationDirectory);
    }

    ///
    /// \brief Execute an SQL query on the database, retrieving the result.
    /// \param SQL Query to execute
    /// \param parameters to bind to the placeholders in the SQL query.
    /// \return Future of a list of lists of variants.
    ///
    template <typename ...Args>
    auto getResults(const QString &sqlQuery, Args... args) -> QFuture<Rows> {
        return m_db->getResults(sqlQuery, args...);
    }

    ///
    /// \brief Like getResults, but for retrieving just one row.
    ///
    template <typename ...Args>
    auto getResult(const QString &sqlQuery, Args... args) -> QFuture<Row> {
        return m_db->getResult(sqlQuery, args...);
    }

    ThreadedDatabase();
    ~ThreadedDatabase();

private:
    std::unique_ptr<asyncdatabase_private::AsyncSqlDatabase> m_db;
};

inline QCoro::Task<> test() {
    DatabaseConfiguration config;
    config.setDatabaseName("test.sqlite");
    config.setType(DATABASE_TYPE_SQLITE);

    auto db = ThreadedDatabase::establishConnection(config);
    co_await db->runMigrations("/tmp/migrations/");

    co_await db->execute(QStringLiteral("INSERT OR IGNORE INTO searches (id, query) VALUES (?, ?)"), 0, "Hello World");
    co_await db->execute(QStringLiteral("INSERT OR IGNORE INTO searches (id, query) VALUES (?, ?)"), 1, "Rammstein");
    const auto row = co_await db->getResult(QStringLiteral("SELECT * FROM searches LIMIT 1"));
    const auto tuple = parseRow<int, QString>(row);
    const auto rows = co_await db->getResults(QStringLiteral("SELECT * FROM searches WHERE id = ? OR id = ?"), 1, 0);
    const auto tuples = parseRows<int, QString>(rows);

    for (const auto &row : tuples) {
        qDebug() << std::get<0>(row) << std::get<1>(row);
    }
}
