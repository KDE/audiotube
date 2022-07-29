# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

add_library(mpris2 STATIC
    mediaplayer2.cpp
    mediaplayer2.h
    mediaplayer2player.cpp
    mediaplayer2player.h
    mpris2.cpp
    mpris2.h
    declarativemprisplayer.cpp
    declarativemprisplayer.h
)

target_link_libraries(mpris2 PUBLIC Qt::Core Qt::Gui Qt::DBus KF5::CoreAddons)
