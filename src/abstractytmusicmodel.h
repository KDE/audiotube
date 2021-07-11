// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>

///
/// Base class for all models that work with data from the AsyncYTMusic class.
/// It provides a loading property, that is automatically set to false if a query fails.
///
class AbstractYTMusicModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit AbstractYTMusicModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);
    Q_SIGNAL void loadingChanged();

private:
    bool m_loading = false;
};

