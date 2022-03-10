#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QSize>
#include <QIODevice>

#include <optional>
#include <memory>

struct QPersistentDataPrivate;
class MDB_env;
class MDB_cursor;
class MDB_txn;
class EnvHandle;
typedef unsigned int MDB_dbi;

///
/// Set the number of databases that can be used in parallel
///
void qPersistentDataSetMaxDbs(int num);

class QPersistentData {
public:
    QPersistentData(QPersistentData &&other) noexcept;
    ~QPersistentData();

    QPersistentData(const QByteArray &name);

    int insert(const QByteArray &key, const QByteArray &value);

    template <typename KeyType, typename ValueType>
    int insert(const KeyType &key, const ValueType &value) {
        QByteArray keyS;
        QByteArray valS;

        QDataStream keyStream(&keyS, QIODevice::WriteOnly);
        keyStream << key;
        QDataStream valStream(&valS, QIODevice::WriteOnly);
        valStream << value;

        return insert(keyS, valS);
    }

    [[nodiscard]] QByteArray value(const QByteArray &key) const;

    template <typename ValueType, typename KeyType>
    ValueType value(const KeyType &key) const {
        QByteArray keyS;
        QDataStream keyStream(&keyS, QIODevice::WriteOnly);
        keyStream << key;

        auto val = value(keyS);
        QDataStream vStream(val);

        ValueType out;
        vStream >> out;
        return out;
    }

    [[nodiscard]] bool isValid() const;

    int remove(const QByteArray &key);

    template <typename KeyType>
    void remove(const KeyType &key) {
        QByteArray keyS;

        QDataStream keyStream(&keyS, QIODevice::WriteOnly);
        keyStream << key;

        return remove(key);
    }

private:
    explicit QPersistentData(EnvHandle handle, const QByteArray &name);
    std::shared_ptr<QPersistentDataPrivate> d;
};
