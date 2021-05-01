// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <variant>
#include <tuple>
#include <vector>
#include <QtGlobal>

template <typename... Arguments>
class MultiIterableView {
private:
    template<typename T, size_t i, size_t len>
    inline size_t count(const T &tuple) const {
        if constexpr(i < len) {
            return std::get<i>(tuple).size() + count<T, i + 1, len>(tuple);
        } else {
            return 0;
        }
    }

    template<typename T, size_t tuple_i, size_t tuple_len>
    inline std::variant<Arguments...> get_item(const T &tuple, size_t i) const {
        if constexpr(tuple_i < tuple_len) {
            if (i < std::get<tuple_i>(tuple).size()) {
                return std::get<tuple_i>(tuple)[i];
            } else {
                return get_item<T, tuple_i + 1, tuple_len>(tuple, i - std::get<tuple_i>(tuple).size());
            }
        } else {
            Q_UNREACHABLE();
        }
    }
public:
    explicit MultiIterableView(const std::vector<Arguments> &...lists)
        : m_vectors(std::make_tuple(lists...))
    {
    }

    size_t size() const {
        constexpr size_t nvecs = std::tuple_size_v<decltype(m_vectors)>;
        return count<decltype(m_vectors), 0, nvecs>(m_vectors);
    }

    std::variant<Arguments...> operator[](size_t i) const {
        constexpr size_t nvecs = std::tuple_size_v<decltype(m_vectors)>;
        return get_item<decltype(m_vectors), 0, nvecs>(m_vectors, i);
    }
private:
    std::tuple<std::vector<Arguments>...> m_vectors;
};
