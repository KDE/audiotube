//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#ifndef MPRIS_QT_H
#define MPRIS_QT_H

#if defined(MPRIS_QT_LIBRARY)
#  define MPRIS_QT_EXPORT Q_DECL_EXPORT
#else
#  define MPRIS_QT_EXPORT Q_DECL_IMPORT
#endif

#endif /* MPRIS_QT_H */
