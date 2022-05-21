#include "asyncdatabase.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QStringBuilder>
#include <QVariant>
#include <QSqlResult>
#include <QSqlError>

namespace asyncdatabase_private {

// migrations
void create_internal_table(QSqlDatabase &database) {
    QSqlQuery query(QStringLiteral("CREATE TABLE IF NOT EXISTS __qt_schema_migrations ("
                                        "version TEXT PRIMARY KEY NOT NULL, "
                                        "run_on TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP)"), database);
    query.exec();
}

void mark_migration_run(QSqlDatabase &database, const QString &name) {
    qDebug() << "Marking migration" << name << "as done.";

    QSqlQuery query(database);
    query.prepare(QStringLiteral("INSERT INTO __qt_schema_migrations (version) VALUES (:name)"));
    query.bindValue(QStringLiteral(":name"), QVariant(name));
    query.exec();
}

bool check_migration_already_run(QSqlDatabase &database, const QString &name) {
    qDebug() << "Checking whether migration" << name << "is already applied...";
    QSqlQuery query(database);
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM __qt_schema_migrations WHERE version = :name"));
    query.bindValue(QStringLiteral(":name"), QVariant(name));
    query.exec();

    query.next();
    int count = query.value(0).toInt();
    bool isApplied = count > 0;
    if (isApplied) {
        qDebug() << "... yes";
    } else {
        qDebug() << "... no";
    }
    return isApplied;
}

void runDatabaseMigrations(QSqlDatabase &database, const QString &migrationDirectory)
{
    create_internal_table(database);

    QDir dir(migrationDirectory);
    auto entries = dir.entryList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot, QDir::SortFlag::Name);

    for (const auto &entry : entries) {
        QDir subdir(entry);
        if (!check_migration_already_run(database, subdir.dirName())) {
            QFile file(migrationDirectory % QDir::separator() % entry % QDir::separator() % "up.sql");
            if (!file.open(QFile::ReadOnly)) {
                qDebug() << "Failed to open migration file";
            }
            qDebug() << "Running migration" << subdir.dirName();
            database.exec(file.readAll());
            mark_migration_run(database, subdir.dirName());
        }
    }
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

AsyncSqlDatabase::~AsyncSqlDatabase() = default;

void printSqlError(const QSqlQuery &query)
{
    qDebug() << "SQL error:" << query.lastError().text();
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


std::unique_ptr<ThreadedDatabase> ThreadedDatabase::establishConnection(DatabaseConfiguration &config) {
    auto threadedDb = std::make_unique<ThreadedDatabase>();
    threadedDb->setObjectName(QStringLiteral("database thread"));
    threadedDb->m_db->establishConnection(config);
    threadedDb->m_db->moveToThread(&*threadedDb);
    threadedDb->start();
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
