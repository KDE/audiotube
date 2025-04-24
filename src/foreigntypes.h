// SPDX-FileCopyrightText: 2025 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <mprisplayer.h>
#include <mpris.h>

#include <qqmlintegration.h>

class MprisPlayerForeign
{
    Q_GADGET
    QML_FOREIGN(MprisPlayer)
    QML_NAMED_ELEMENT(MprisPlayer)
};


class MprisForeign
{
    Q_GADGET
    QML_FOREIGN(Mpris)
    QML_NAMED_ELEMENT(Mpris)
    QML_SINGLETON
};
