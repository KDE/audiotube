#SPDX-FileCopyrightText: 2023 Théophile Gilgien <theophile.gilgien@gmail.com>
#
#SPDX-License-Identifier: BSD-2-Clause

add_library(declarative STATIC
    mprisplugin.cpp
)

target_link_libraries(declarative
    Qt::Qml
    Qt::Core
    Qt::DBus
    qt-mpris
)
