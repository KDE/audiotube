// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "homemodel.h"
#include "asyncytmusic.h"
#include <QCoroFuture>

// --- HomeModel ---

HomeModel::HomeModel(QObject *parent)
    : AbstractShelfModel(parent)
{
    refresh();
}

void HomeModel::refresh() {
    setLoading(true);
    // Fetch more shelves to ensure we find "Quick picks"
    auto future = YTMusicThread::instance()->fetchHome(6);
    QCoro::connect(std::move(future), this, [=, this](std::vector<home::Shelf> result) {
        beginResetModel();

        // Find "Quick picks" and move it to the front
        auto it = std::find_if(result.begin(), result.end(), [](const home::Shelf &shelf) {
            return QString::fromStdString(shelf.title).compare(QStringLiteral("Quick picks"), Qt::CaseInsensitive) == 0;
        });
        
        if (it != result.end() && it != result.begin()) {
            std::rotate(result.begin(), it, it + 1);
        }
        
        m_shelves = std::move(result);
        m_shelfModels.clear();
        for (const auto &shelf : m_shelves) {
            addShelf(shelf.contents);
        }
        endResetModel();
        setLoading(false);
    });
}
