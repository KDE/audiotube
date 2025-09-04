//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#ifndef MPRIS_H
#define MPRIS_H

#include <mprisqt.h>

#include <QtCore/QObject>
#include <QtCore/QString>

class Mpris : public QObject
{
    Q_OBJECT

    Q_ENUMS(PlaybackStatus LoopStatus Metadata)

public:

    enum PlaybackStatus {
        InvalidPlaybackStatus = -1,
        Playing,
        Paused,
        Stopped
    };

    enum LoopStatus {
        InvalidLoopStatus = -1,
        None,
        Track,
        Playlist
    };

    enum Metadata {
        InvalidMetadata = -1,
        TrackId,
        Length,
        ArtUrl,
        Album,
        AlbumArtist,
        Artist,
        AsText,
        AudioBPM,
        AutoRating,
        Comment,
        Composer,
        ContentCreated,
        DiscNumber,
        FirstUsed,
        Genre,
        LastUsed,
        Lyricist,
        Title,
        TrackNumber,
        Url,
        UseCount,
        UserRating
    };

    Mpris(QObject *parent = nullptr);
    ~Mpris();

    Q_INVOKABLE static QString metadataToString(Metadata metadata);

private:
    template<class T, int N>
    static int arraySize(T (&)[N])
    {
        return N;
    }
    template<typename T, typename U> struct is_same { static const bool value = false; };
    template<typename T> struct is_same<T, T> { static const bool value = true; };
    template<typename T> static void getEnumStringsAndSize(const char ***strings, int *size);
    template<typename T> static QString enumerationToString(T enumeration);
    template<typename T> static T enumerationFromString(const QString &string);

    friend class MprisRootAdaptor;
    friend class MprisPlayerAdaptor;
    friend class MprisPlayer;
    friend class MprisController;
};

#endif /* MPRIS_H */
