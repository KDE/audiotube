// SPDX-FileCopyrightText: 2026 Tanveer Ahmed Mansuri
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <qqmlregistration.h>

#include "abstractshelfmodel.h" 

// Main model for the Home tab (vertical list of shelves)
class HomeModel : public AbstractShelfModel {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit HomeModel(QObject *parent = nullptr);

    void refresh();
};
