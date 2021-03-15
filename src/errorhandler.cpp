// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "errorhandler.h"

#include <QDebug>

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
{
    connect(&AsyncYTMusic::instance(), &AsyncYTMusic::errorOccurred, this, &ErrorHandler::errorOccurred);
}
