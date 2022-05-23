// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

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
template <typename RowTypesTuple>
auto parseRow(const Row &row) -> RowTypesTuple
{
    auto tuple = RowTypesTuple();
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
template <typename RowTypesTuple>
auto parseRows(const Rows &rows) -> std::vector<RowTypesTuple> {
    std::vector<RowTypesTuple> parsedRows;
    parsedRows.reserve(rows.size());
    for (const auto &row : rows) {
        parsedRows.push_back(parseRow<RowTypesTuple>(row));
    }

    return parsedRows;
}

struct ThreadedDatabasePrivate;

class ThreadedDatabase : public QThread {
public:
    ///
    /// \brief Connect to a database
    /// \param configuration of the database connection
    /// \return
    ///
    static std::unique_ptr<ThreadedDatabase> establishConnection(DatabaseConfiguration config);

    ///
    /// \brief Execute an SQL query on the database, ignoring the result.
    /// \param SQL query string to execute
    /// \param Parameters to bind to the placeholders in the SQL Query
    /// \return
    ///
    template <typename ...Args>
    auto execute(const QString &sqlQuery, Args... args) -> QFuture<void> {
        return db().execute(sqlQuery, args...);
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
        return db().runMigrations(migrationDirectory);
    }

    ///
    /// \brief Execute an SQL query on the database, retrieving the result.
    /// \param SQL Query to execute
    /// \param parameters to bind to the placeholders in the SQL query.
    /// \return Future of a list of lists of variants.
    ///
    template <typename T, typename ...Args>
    auto getResults(const QString &sqlQuery, Args... args) -> QFuture<std::vector<T>> {
        return db().getResults<T, Args...>(sqlQuery, args...);
    }

    ///
    /// \brief Like getResults, but for retrieving just one row.
    ///
    template <typename T, typename ...Args>
    auto getResult(const QString &sqlQuery, Args... args) -> QFuture<std::optional<T>> {
        return db().getResult<T, Args...>(sqlQuery, args...);
    }

    ThreadedDatabase();
    ~ThreadedDatabase();

private:
    asyncdatabase_private::AsyncSqlDatabase &db();

    std::unique_ptr<ThreadedDatabasePrivate> d;
};
