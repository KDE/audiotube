#SPDX-FileCopyrightText: 2023 Théophile Gilgien <theophile.gilgien@gmail.com>
#
#SPDX-License-Identifier: BSD-2-Clause

add_library(qtdbusextended STATIC
    dbusextendedabstractinterface.cpp
    dbusextendedpendingcallwatcher.cpp
)

target_link_libraries(qtdbusextended
    Qt::DBus
)
