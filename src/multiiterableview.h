// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <variant>
#include <tuple>
#include <vector>
#include <QtGlobal>

// MultiIterableView is usually used on a small number of vectors.
// The number is already known, so we can convert this into an if else if else etc tree
// instead of recursively calling the function
#ifdef __GNUC__
#define REMOVE_RECURSION __attribute__((always_inline))
#else
#define REMOVE_RECURSION
#endif

template <typename... Arguments>
class MultiIterableView {
private:
    template<typename T, size_t i, size_t len>
    REMOVE_RECURSION inline size_t count(const T &tuple, int initial_size = 0) const {
        if constexpr(i < len) {
            return count<T, i + 1, len>(tuple, initial_size + std::get<i>(tuple).size());
        } else {
            return initial_size;
        }
    }

    template<typename T, size_t tuple_i, size_t tuple_len>
    REMOVE_RECURSION inline std::variant<Arguments...> get_item(const T &tuple, size_t i) const {
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

    template<typename T, size_t i, size_t len>
    REMOVE_RECURSION inline bool everything_empty(const T &tuple, bool initial_state = true) const {
        if constexpr(i < len) {
            return everything_empty<T, i + 1, len>(tuple, initial_state && std::get<i>(tuple).empty());
        } else {
            return initial_state;
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

    bool empty() const {
        constexpr size_t nvecs = std::tuple_size_v<decltype(m_vectors)>;
        return everything_empty<decltype(m_vectors), 0, nvecs>(m_vectors);
    }
private:
    std::tuple<std::vector<Arguments>...> m_vectors;
};
