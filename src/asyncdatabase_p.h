// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <memory>
#include <tuple>
#include <optional>

#include <QFuture>
#include <QFutureWatcher>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QThread>

class DatabaseConfiguration;

namespace asyncdatabase_private {

// Helpers
template <typename Tuple, typename Func, std::size_t i>
inline constexpr void iterate_impl(Tuple &tup, Func fun)
{
    if constexpr(i >= std::tuple_size_v<std::decay_t<decltype(tup)>>) {
        return;
    } else {
        fun(std::get<i>(tup));
        return asyncdatabase_private::iterate_impl<Tuple, Func, i + 1>(tup, fun);
    }
}

template <typename Tuple, typename Func>
inline constexpr void iterate_tuple(Tuple &tup, Func fun)
{
    asyncdatabase_private::iterate_impl<Tuple, Func, 0>(tup, fun);
}

template <typename T, typename QObjectDerivedType, typename Function>
void connectFuture(const QFuture<T> &future, QObjectDerivedType *self, const Function &fun) {
    auto watcher = std::make_shared<QFutureWatcher<T>>();
    watcher->setFuture(future);
    QObject::connect(watcher.get(), &QFutureWatcherBase::finished, self, [self, watcher, fun, future] {
        if constexpr (std::is_same_v<void, T>) {
            if constexpr (std::is_member_function_pointer_v<Function>) {
                fun->*(self);
            } else {
                fun();
            }
        } else if (future.resultCount() > 0) {
            if constexpr (std::is_member_function_pointer_v<Function>) {
                (self->*fun)(watcher->result());
            } else {
                fun(watcher->result());
            }
        }
    });
}

using Row  = std::vector<QVariant>;
using Rows = std::vector<Row>;

void runDatabaseMigrations(QSqlDatabase &database, const QString &migrationDirectory);

void printSqlError(const QSqlQuery &query);

// non-template helper functions to allow patching a much as possible in the shared library
QSqlQuery prepareQuery(const QSqlDatabase &database, const QString &sqlQuery);
QSqlQuery runQuery(QSqlQuery &query);

struct AsyncSqlDatabasePrivate;

class AsyncSqlDatabase : public QObject {
    Q_OBJECT

public:
    QFuture<void> establishConnection(const DatabaseConfiguration &configuration);

    template <typename T, typename ...Args>
    auto getResults(const QString &sqlQuery, Args... args) -> QFuture<std::vector<T>> {
        return runAsync<std::vector<T>>([=, this] {
            auto query = executeQuery(sqlQuery, args...);
            auto dbrows = retrieveRows(query);
            const auto rows = parseRows<typename T::ColumnTypes>(dbrows);

            std::vector<T> deserializedRows;
            for (auto row : rows) {
                deserializedRows.push_back(T::fromSql(std::move(row)));
            }
            return deserializedRows;
        });
    }

    template <typename T, typename ...Args>
    auto getResult(const QString &sqlQuery, Args... args) -> QFuture<std::optional<T>> {
        return runAsync<std::optional<T>>([=, this]() -> std::optional<T> {
            auto query = executeQuery(sqlQuery, args...);
            if (const auto row = retrieveOptionalRow(query)) {
                return T::fromSql(parseRow<typename T::ColumnTypes>(*row));
            }

            return std::nullopt;
        });
    }

    template <typename ...Args>
    auto execute(const QString &sqlQuery, Args... args) -> QFuture<void> {
        return runAsync<void>([=, this] {
            executeQuery(sqlQuery, args...);
        });
    }

    auto runMigrations(const QString &migrationDirectory) -> QFuture<void> {
        return runAsync<void>([=, this] {
            runDatabaseMigrations(db(), migrationDirectory);
        });
    }

    AsyncSqlDatabase();
    ~AsyncSqlDatabase();

private:
    template <typename ...Args>
    QSqlQuery executeQuery(const QString &sqlQuery, Args... args) {
        auto query = prepareQuery(db(), sqlQuery);
        auto argsTuple = std::make_tuple<Args...>(std::move(args)...);
        int i = 0;
        asyncdatabase_private::iterate_tuple(argsTuple, [&](auto &arg) {
            query.bindValue(i, arg);
            i++;
        });
        return runQuery(query);
    }

    template <typename T, typename Functor>
    QFuture<T> runAsync(Functor func) {
        auto interface = std::make_shared<QFutureInterface<T>>();
        QMetaObject::invokeMethod(this, [interface, func] {
            if constexpr (!std::is_same_v<T, void>) {
                auto result = func();
                interface->reportResult(result);
            } else {
                func();
            }

            interface->reportFinished();
        });

        return interface->future();
    }

    Row retrieveRow(const QSqlQuery &query);
    Rows retrieveRows(QSqlQuery &query);
    std::optional<Row> retrieveOptionalRow(QSqlQuery &query);

    QSqlDatabase &db();

    std::unique_ptr<AsyncSqlDatabasePrivate> d;
};

}
