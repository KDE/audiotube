// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "abstractytmusicmodel.h"

#include "asyncytmusic.h"


AbstractYTMusicModel::AbstractYTMusicModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

bool AbstractYTMusicModel::loading() const
{
    return m_loading;
}

void AbstractYTMusicModel::setLoading(bool loading)
{
    if (m_loading == loading)
        return;

    m_loading = loading;
    Q_EMIT loadingChanged();
}
