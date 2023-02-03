// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QDebug>

#include <Desert/Reflection>

#include "updateablemodel.h"

template <typename T>
using AutoListModelBase = UpdateableModel<QAbstractListModel, T>;

template <typename T>
concept IsComparable = requires(T v, T o)
{
    { v == o } -> std::same_as<bool>;
    { v < o } -> std::same_as<bool>;
};

template <typename T>
requires desert::IsDesertObject<T> && IsComparable<T>
class AutoListModel : public AutoListModelBase<T>
{
public:
    AutoListModel(QObject *parent = nullptr) : AutoListModelBase<T>(parent) {

    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        int i = Qt::UserRole + 1;
        T mock_obj;
        desert::iterate_struct(mock_obj, [&](auto attribute) {
            roles.insert(i, attribute.attributeName().toUtf8());
            i++;
        });
        return roles;
    }

    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_data.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        int structIndex = role - Qt::UserRole - 1;
        if (structIndex >= pfr::tuple_size_v<T>) {
            qDebug() << "Tried to fetch a role that is not defined by this model. Role id:" << role;
            return {};
        }
        return structAttributeAt<0, T>(m_data.at(index.row()), structIndex);
    }

    void updateData(std::vector<T> &&updatedData) {
        // allow comparing items. Could also have used an operator< on Data
        auto lessThan = [](const T &lhs, const T &rhs) { return lhs < rhs; };
        // check if the item has changed data, but is otherwise the same object. Needs to return the actual changes made.
        auto itemHasChanged = [](const T &lhs, const T &rhs) {
            typename AutoListModelBase<T>::DataChanges changes;
            if (lhs != rhs)
                changes.changedColumns.append(0);
            changes.changedRoles = QVector<int>{}; // TODO more specific updates
            return changes;
        };

        // call the updateData method to trigger merging in the changes to the model
        auto changes = AutoListModelBase<T>::updateData(updatedData.cbegin(), updatedData.cend(), m_data, lessThan, itemHasChanged);

        // the method above returns some stats, so lets print them.
        qDebug() << "changes in model: inserts:" << changes.inserts << " deletes:" << changes.removals
                 << " updates:" << changes.updates;
    }

private:
    std::vector<T> m_data;
};

template <size_t i, typename T>
requires desert::IsDesertObject<T>
QVariant structAttributeAt(const T &object, size_t index) {
    if constexpr (i >= pfr::tuple_size_v<T>) {
        Q_UNREACHABLE();
    } else if (i == index) {
        return QVariant::fromValue(pfr::get<i>(object).value);
    } else {
        return structAttributeAt<i + 1>(object, index);
    }
}
