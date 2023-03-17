//SPDX-FileCopyrightText: 2015 Jolla Ltd. <valerio.valerio@jolla.com>
//SPDX-FileContributor: Andres Gomez
//
//SPDX-License-Identifier: LGPL-2.1-or-later


#include "mpris.h"

static const char *playbackStatusStrings[] = { "Playing", "Paused", "Stopped" };
static const char *loopStatusStrings[] = { "None", "Track", "Playlist" };
static const char *metadataStrings[] = { "mpris:trackid", "mpris:length", "mpris:artUrl", "xesam:album",
                                         "xesam:albumArtist", "xesam:artist", "xesam:asText", "xesam:audioBPM",
                                         "xesam:autoRating", "xesam:comment", "xesam:composer", "xesam:contentCreated",
                                         "xesam:discNumber", "xesam:firstUsed", "xesam:genre", "xesam:lastUsed",
                                         "xesam:lyricist", "xesam:title", "xesam:trackNumber", "xesam:url",
                                         "xesam:useCount", "xesam:userRating" };


Mpris::Mpris(QObject *parent)
    : QObject(parent)
{
}

Mpris::~Mpris()
{
}

QString Mpris::metadataToString(Mpris::Metadata metadata)
{
    return enumerationToString(metadata);
}


// Private

template <typename T> void Mpris::getEnumStringsAndSize(const char ***strings, int *size)
{
    if (is_same<T, Mpris::LoopStatus>::value) {
        *strings = loopStatusStrings;
        *size = arraySize(loopStatusStrings);
    } else if (is_same<T, Mpris::PlaybackStatus>::value) {
        *strings = playbackStatusStrings;
        *size = arraySize(playbackStatusStrings);
    } else if (is_same<T, Mpris::Metadata>::value) {
        *strings = metadataStrings;
        *size = arraySize(metadataStrings);
    } else {
        *strings = 0;
        *size = 0;
    }
}
template void Mpris::getEnumStringsAndSize<Mpris::LoopStatus>(const char ***strings, int *size);
template void Mpris::getEnumStringsAndSize<Mpris::PlaybackStatus>(const char ***strings, int *size);
template void Mpris::getEnumStringsAndSize<Mpris::Metadata>(const char ***strings, int *size);


template <typename T> QString Mpris::enumerationToString(T enumeration)
{
    const char **strings;
    int size;

    getEnumStringsAndSize<T>(&strings, &size);

    return enumeration >= 0 && enumeration < size
                                             ? QString(QLatin1String(strings[enumeration]))
                                             : QString();
}
template QString Mpris::enumerationToString<Mpris::LoopStatus>(Mpris::LoopStatus enumeration);
template QString Mpris::enumerationToString<Mpris::PlaybackStatus>(Mpris::PlaybackStatus enumeration);
template QString Mpris::enumerationToString<Mpris::Metadata>(Mpris::Metadata enumeration);


template <typename T> T Mpris::enumerationFromString(const QString &string)
{
    const char **strings;
    int size;

    getEnumStringsAndSize<T>(&strings, &size);

    for (int i = 0; i < size; ++i) {
        if (string == QLatin1String(strings[i])) {
            return T(i);
        }
    }

    return T(-1);
}
template Mpris::LoopStatus Mpris::enumerationFromString<Mpris::LoopStatus>(const QString &string);
template Mpris::PlaybackStatus Mpris::enumerationFromString<Mpris::PlaybackStatus>(const QString &string);
template Mpris::Metadata Mpris::enumerationFromString<Mpris::Metadata>(const QString &string);
