// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "errorhandler.h"

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
{
    connect(&YTMusicThread::instance().get(), &AsyncYTMusic::errorOccurred, this, &ErrorHandler::errorOccurred);
}
