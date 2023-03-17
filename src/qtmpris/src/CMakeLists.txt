#SPDX-FileCopyrightText: 2023 Théophile Gilgien <theophile.gilgien@gmail.com>
#
#SPDX-License-Identifier: BSD-2-Clause

add_library(qt-mpris STATIC
    mpris.cpp
    mprisrootadaptor.cpp
    mprisplayeradaptor.cpp
    mprisplayer.cpp
    mpriscontroller.cpp
    mprismanager.cpp
    mprisplayerinterface.cpp
    mprisrootinterface.cpp
)
target_link_libraries(qt-mpris
    Qt::Core
    Qt::DBus
    qtdbusextended
)
