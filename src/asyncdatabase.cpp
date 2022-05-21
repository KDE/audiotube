// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "asyncdatabase.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QStringBuilder>
#include <QVariant>
#include <QSqlResult>
#include <QSqlError>
#include <QLoggingCategory>

#define SCHAMA_MIGRATIONS_TABLE "__qt_schema_migrations"

Q_DECLARE_LOGGING_CATEGORY(asyncdatabase)
Q_LOGGING_CATEGORY(asyncdatabase, "asyncdatabase")

namespace asyncdatabase_private {

// migrations
void createInternalTable(QSqlDatabase &database) {
    QSqlQuery query(QStringLiteral("create table if not exists " SCHAMA_MIGRATIONS_TABLE " ("
                                        "version Text primary key not null, "
                                        "run_on timestamp not null default current_timestamp)"), database);
    if (!query.exec()) {
        printSqlError(query);
    }
}

void markMigrationRun(QSqlDatabase &database, const QString &name) {
    qCDebug(asyncdatabase) << "Marking migration" << name << "as done.";

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("insert into " SCHAMA_MIGRATIONS_TABLE " (version) values (:name)"))) {
        printSqlError(query);
    }
    query.bindValue(QStringLiteral(":name"), QVariant(name));
    if (!query.exec()) {
        printSqlError(query);
    }
}

bool checkMigrationAlreadyRun(QSqlDatabase &database, const QString &name) {
    qCDebug(asyncdatabase) << "Checking whether migration" << name << "is already applied";
    QSqlQuery query(database);
    query.prepare(QStringLiteral("select count(*) from " SCHAMA_MIGRATIONS_TABLE " where version = :name"));
    query.bindValue(QStringLiteral(":name"), QVariant(name));
    query.exec();

    query.next();
    int count = query.value(0).toInt();
    bool isApplied = count > 0;
    if (isApplied) {
        qCDebug(asyncdatabase) << "… yes";
    } else {
        qDebug(asyncdatabase) << "… no";
    }
    return isApplied;
}

void runDatabaseMigrations(QSqlDatabase &database, const QString &migrationDirectory)
{
    createInternalTable(database);

    QDir dir(migrationDirectory);
    auto entries = dir.entryList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot, QDir::SortFlag::Name);

    for (const auto &entry : entries) {
        QDir subdir(entry);
        if (!checkMigrationAlreadyRun(database, subdir.dirName())) {
            QFile file(migrationDirectory % QDir::separator() % entry % QDir::separator() % "up.sql");
            if (!file.open(QFile::ReadOnly)) {
                qCDebug(asyncdatabase) << "Failed to open migration file" << file.fileName();
            }
            qCDebug(asyncdatabase) << "Running migration" << subdir.dirName();

            // Hackish
            const auto statements = file.readAll().split(';');

            bool migrationSuccessful = true;
            for (const QByteArray &statement : statements) {
                const auto trimmedStatement = statement.trimmed();
                QSqlQuery query(database);

                if (!trimmedStatement.isEmpty()) {
                    qCDebug(asyncdatabase) << "Running" << trimmedStatement;
                    if (!query.prepare(trimmedStatement)) {
                        printSqlError(query);
                    } else {
                        bool success = query.exec();
                        migrationSuccessful &= success;
                        if (!success) {
                            printSqlError(query);
                        }
                    }
                }
            }
            if (migrationSuccessful) {
                markMigrationRun(database, subdir.dirName());
            }
        }
    }
    qCDebug(asyncdatabase) << "Migrations finished";
}

// Internal asynchronous database class
QFuture<void> AsyncSqlDatabase::establishConnection(const DatabaseConfiguration &configuration)
{
    return runAsync<void>([=, this] {
        m_database = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase(configuration.type()));
        if (configuration.databaseName()) {
            m_database->setDatabaseName(*configuration.databaseName());
        }
        if (configuration.hostName()) {
            m_database->setHostName(*configuration.hostName());
        }
        if (configuration.userName()) {
            m_database->setUserName(*configuration.userName());
        }
        if (configuration.password()) {
            m_database->setPassword(*configuration.password());
        }

        m_database->open();
    });
}

AsyncSqlDatabase::AsyncSqlDatabase()
    : QObject()
    , m_database(nullptr)
{
}

Row AsyncSqlDatabase::retrieveRow(const QSqlQuery &query) {
    Row row;
    int i = 0;

    while (true) {
        if (query.isValid()) {
            QVariant value = query.value(i);
            if (value.isValid()) {
                row.push_back(std::move(value));
                i++;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    return row;
}

Rows AsyncSqlDatabase::retrieveRows(QSqlQuery &query)
{
    Rows rows;
    while (query.next()) {
        rows.push_back(retrieveRow(query));
    }

    return rows;
}

std::optional<Row> AsyncSqlDatabase::retrieveOptionalRow(QSqlQuery &query)
{
    query.next();

    if (query.isValid()) {
        return retrieveRow(query);
    } else {
        return std::nullopt;
    }
}

AsyncSqlDatabase::~AsyncSqlDatabase() = default;

void printSqlError(const QSqlQuery &query)
{
    qCDebug(asyncdatabase) << "SQL error:" << query.lastError().text();
}

QSqlQuery prepareQuery(const QSqlDatabase &database, const QString &sqlQuery)
{
    qCDebug(asyncdatabase) << "Running" << sqlQuery;
    QSqlQuery query(database);
    if (!query.prepare(sqlQuery)) {
        printSqlError(query);
    }
    return query;
}

}

struct DatabaseConfigurationPrivate : public QSharedData {
    QString type;
    std::optional<QString> hostName;
    std::optional<QString> databaseName;
    std::optional<QString> userName;
    std::optional<QString> password;
};

DatabaseConfiguration::DatabaseConfiguration() : d(new DatabaseConfigurationPrivate)
{}

DatabaseConfiguration::~DatabaseConfiguration() = default;
DatabaseConfiguration::DatabaseConfiguration(const DatabaseConfiguration &) = default;

void DatabaseConfiguration::setType(const QString &type) {
    d->type = type;
}

const QString &DatabaseConfiguration::type() const {
    return d->type;
}

void DatabaseConfiguration::setHostName(const QString &hostName) {
    d->hostName = hostName;
}

const std::optional<QString> &DatabaseConfiguration::hostName() const {
    return d->hostName;
}

void DatabaseConfiguration::setDatabaseName(const QString &databaseName) {
    d->databaseName = databaseName;
}

const std::optional<QString> &DatabaseConfiguration::databaseName() const {
    return d->databaseName;
}

void DatabaseConfiguration::setUserName(const QString &userName) {
    d->userName = userName;
}

const std::optional<QString> &DatabaseConfiguration::userName() const {
    return d->userName;
}

void DatabaseConfiguration::setPassword(const QString &password) {
    d->password = password;
}

const std::optional<QString> &DatabaseConfiguration::password() const {
    return d->password;
}


std::unique_ptr<ThreadedDatabase> ThreadedDatabase::establishConnection(DatabaseConfiguration config) {
    auto threadedDb = std::make_unique<ThreadedDatabase>();
    threadedDb->setObjectName(QStringLiteral("database thread"));
    threadedDb->m_db->moveToThread(&*threadedDb);
    threadedDb->start();
    threadedDb->m_db->establishConnection(config);
    return threadedDb;
}

ThreadedDatabase::ThreadedDatabase()
    : QThread()
    , m_db(std::make_unique<asyncdatabase_private::AsyncSqlDatabase>())
{
}

ThreadedDatabase::~ThreadedDatabase()
{
    quit();
    wait();
}
