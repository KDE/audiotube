// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <variant>
#include <tuple>
#include <vector>
#include <QtGlobal>

template <typename Tuple, typename Func, std::size_t i>
inline constexpr void iterate_impl(Tuple &tup, Func fun)
{
    if constexpr(i >= std::tuple_size_v<std::decay_t<decltype(tup)>>) {
        return;
    } else {
        fun(std::get<i>(tup));
        return iterate_impl<Tuple, Func, i + 1>(tup, fun);
    }
}

template <typename Tuple, typename Func>
inline constexpr void iterate_tuple(Tuple &tup, Func fun)
{
    iterate_impl<Tuple, Func, 0>(tup, fun);
}


template <typename... Arguments>
class MultiIterableView {
public:
    explicit MultiIterableView(std::vector<Arguments> &...lists)
        : m_vectors(std::forward_as_tuple(lists...))
    {
    }

    [[nodiscard]] constexpr size_t size() const {
        size_t s = 0;
        iterate_tuple(m_vectors, [&](auto &vec) {
            s += vec.size();
        });
        return s;
    }

    [[nodiscard]] constexpr std::variant<Arguments...> operator[](size_t i) const {
        size_t s = 0;
        std::variant<Arguments...> out;
        iterate_tuple(m_vectors, [&](auto &vec) {
            if (i >= s && i < s + vec.size()) {
                out = vec[i - s];
            }
            s += vec.size();
        });
        return out;
    }

    [[nodiscard]] constexpr bool empty() const {
        bool empty = true;
        iterate_tuple(m_vectors, [&](auto &elem) {
            empty &= elem.empty();
        });
        return empty;
    }
private:
    std::tuple<std::vector<Arguments> &...> m_vectors;
};
