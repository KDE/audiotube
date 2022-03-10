#include "qpersistentdata.h"

#include <QtGlobal>
#include <QVariant>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QStringBuilder>
#include <QLoggingCategory>

#include <string_view>
#include <optional>
#include <tuple>

#include <lmdb.h>

Q_DECLARE_LOGGING_CATEGORY(PersistentDataLog)
Q_LOGGING_CATEGORY(PersistentDataLog, "qpersistentdata")

struct EnvironmentHolder {
    MDB_env *env = nullptr;
    int users = 0;
    int maxDbs = 10;
};

EnvironmentHolder DATABASE_ENVIRONMENT;

void qPersistentDataSetMaxDbs(int num)
{
    DATABASE_ENVIRONMENT.maxDbs = num;
}

///
/// Increments the number of references to the global database
/// environment on construction, and decrements it on destruction
///
/// This should ensure that an EnvHandle is always a valid handle to
/// an MDB_env
///
class EnvHandle {
public:
    EnvHandle() {
        DATABASE_ENVIRONMENT.users++;

        if (!DATABASE_ENVIRONMENT.env) {
            const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) % "/persistent-data/";
            QDir(path).mkpath(QStringLiteral("."));

            int s = mdb_env_create(&DATABASE_ENVIRONMENT.env);
            Q_ASSERT(s == MDB_SUCCESS);
            s = mdb_env_set_maxdbs(DATABASE_ENVIRONMENT.env, DATABASE_ENVIRONMENT.maxDbs);
            Q_ASSERT(s == MDB_SUCCESS);
            if (int status = mdb_env_open(DATABASE_ENVIRONMENT.env, path.toUtf8(), 0, 0664) != MDB_SUCCESS) {
                qCDebug(PersistentDataLog) << "Failed to open database with exit code" << status;
                mdb_env_close(DATABASE_ENVIRONMENT.env);
            }
        }
    }

    EnvHandle(EnvHandle &) {
        DATABASE_ENVIRONMENT.users++;
    }

    EnvHandle &operator=(const EnvHandle &) = default;
    EnvHandle(EnvHandle &&) = default;
    EnvHandle &operator=(EnvHandle &&) = default;

    ~EnvHandle() {
        DATABASE_ENVIRONMENT.users--;

        if (DATABASE_ENVIRONMENT.users == 0) {
            qCDebug(PersistentDataLog) << "Closing database handle";
            mdb_env_close(DATABASE_ENVIRONMENT.env);
        }
    }

    operator MDB_env*() {
        return DATABASE_ENVIRONMENT.env;
    }
};

/// Qt wrapper for MDB_val
struct LMDBValueView : public MDB_val {
    constexpr LMDBValueView() : MDB_val()
    {
        mv_size = 0;
        mv_data = nullptr;
    }

    constexpr LMDBValueView(const QByteArray &bytes) : MDB_val()
    {
        mv_size = static_cast<size_t>(bytes.size());
        mv_data = const_cast<char *>(bytes.data());
    }

    [[nodiscard]] constexpr const char *data() const {
        return static_cast<const char *>(mv_data);
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return mv_size;
    }

    [[nodiscard]] QByteArray toByteArray() const {
        return QByteArray(data(), size());
    }
};

struct QPersistentDataPrivate {
    QByteArray name;
    EnvHandle handle;

    ~QPersistentDataPrivate() = default;
};

QPersistentData::QPersistentData(EnvHandle handle, const QByteArray &name)
    : d(std::make_unique<QPersistentDataPrivate>())
{
    d->handle = handle;
    d->name = name;
}

QPersistentData::QPersistentData(const QByteArray &name)
    : QPersistentData(EnvHandle(), name)
{
}

int QPersistentData::insert(const QByteArray &key, const QByteArray &value)
{
    if (!d->handle) {
        return MDB_INVALID;
    }

    MDB_txn *wtxn = nullptr;
    if (int status = mdb_txn_begin(d->handle, nullptr, 0, &wtxn) != MDB_SUCCESS) {
        return status;
    }
    MDB_dbi dbi {};
    if (int status = mdb_dbi_open(wtxn, d->name, MDB_CREATE, &dbi) != MDB_SUCCESS) {
        return status;
    }

    LMDBValueView k(key);
    LMDBValueView v(value);

    if (int status = mdb_put(wtxn, dbi, &k, &v, 0) != MDB_SUCCESS) {
        return status;
    }
    if (int status = mdb_txn_commit(wtxn) != MDB_SUCCESS) {
        return status;
    }
    mdb_dbi_close(d->handle, dbi);
    return {};
}

QByteArray QPersistentData::value(const QByteArray &key) const
{
    if (!d->handle) {
        return {};
    }

    MDB_txn *rtxn = nullptr;
    if (int status = mdb_txn_begin(d->handle, nullptr, MDB_RDONLY, &rtxn) != MDB_SUCCESS) {
        qCDebug(PersistentDataLog) << "Failed to start transaction:" << status;
        return {};
    }

    MDB_dbi dbi {};
    if (int status = mdb_dbi_open(rtxn, d->name, MDB_CREATE, &dbi) != MDB_SUCCESS) {
        qCDebug(PersistentDataLog) << "Failed to open database handle:" << status;
        return {};
    }

    LMDBValueView k(key);
    LMDBValueView v;

    if (int status = mdb_get(rtxn, dbi, &k, &v) != MDB_SUCCESS) {
        qCDebug(PersistentDataLog) << "Failed to retrieve data from the database:" << status;
        return {};
    }
    mdb_txn_abort(rtxn);
    mdb_dbi_close(d->handle, dbi);

    return v.toByteArray();
}

bool QPersistentData::isValid() const
{
    return d->handle;
}

int QPersistentData::remove(const QByteArray &key)
{

    if (!d->handle) {
        return {};
    }

    MDB_txn *wtxn = nullptr;
    if (int status = mdb_txn_begin(d->handle, nullptr, 0, &wtxn) != MDB_SUCCESS) {
        return status;
    }
    MDB_dbi dbi {};
    if (int status = mdb_dbi_open(wtxn, d->name, MDB_CREATE, &dbi) != MDB_SUCCESS) {
        return status;
    }

    LMDBValueView k(key);

    if (int status = mdb_del(wtxn, dbi, &k, 0) != MDB_SUCCESS) {
        return status;
    }

    if (int status = mdb_txn_commit(wtxn) != MDB_SUCCESS) {
        return status;
    }
    mdb_dbi_close(d->handle, dbi);
    return {};

}

QPersistentData::~QPersistentData() = default;
QPersistentData::QPersistentData(QPersistentData &&other) noexcept = default;
