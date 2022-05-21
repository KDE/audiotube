#pragma once

#include <memory>
#include <tuple>
#include <optional>

#include <QFuture>
#include <QFutureWatcher>
#include <QSqlQuery>
#include <QSqlDatabase>

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

class AsyncSqlDatabase : public QObject {
    Q_OBJECT

public:
    QFuture<void> establishConnection(const DatabaseConfiguration &configuration);

    template <typename ...Args>
    auto getResults(const QString &sqlQuery, Args... args) -> QFuture<Rows> {
        return runAsync<Rows>([=, this] {
            auto query = executeQuery(sqlQuery, args...);
            // Position on first row
            query.next();
            Rows rows;
            do {
                rows.push_back(retrieveRow(query));
            } while (query.next());

            return rows;
        });
    }

    template <typename ...Args>
    auto getResult(const QString &sqlQuery, Args... args) -> QFuture<Row> {
        return runAsync<Row>([=, this] {
            auto query = executeQuery(sqlQuery, args...);
            query.next();

            if (query.isValid()) {
                return retrieveRow(query);
            } else {
                return Row {};
            }
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
            runDatabaseMigrations(*m_database, migrationDirectory);
        });
    }

    AsyncSqlDatabase();
    ~AsyncSqlDatabase();

private:
    template <typename ...Args>
    QSqlQuery executeQuery(const QString &sqlQuery, Args... args) {
        auto argsTuple = std::make_tuple<Args...>(std::move(args)...);
        QSqlQuery query(*m_database);
        if (!query.prepare(sqlQuery)) {
            printSqlError(query);
        }
        int i = 0;
        asyncdatabase_private::iterate_tuple(argsTuple, [&](auto &arg) {
            query.bindValue(i, arg);
            i++;
        });

        if (!query.exec()) {
            printSqlError(query);
        }

        return query;
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

    Row retrieveRow(const QSqlQuery &query) {
        Row row;
        int i = 0;

        while (true) {
            QVariant value = query.value(i);
            if (value.isValid()) {
                row.push_back(std::move(value));
                i++;
            } else {
                break;
            }
        }
        return row;
    }

    std::unique_ptr<QSqlDatabase> m_database;
};

}
