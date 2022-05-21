-- SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
--
-- SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

create table songs (
    video_id Text primary key not null,
    title Text not null,
    artist Text,
    album Text
);

create table played_songs (
    video_id Text primary key not null,
    plays Int not null,
    foreign key (video_id) references songs(video_id)
);

create table favourites (
    video_id Text primary key not null,
    foreign key (video_id) references songs(video_id)
);

create table searches (
    search_id integer primary key autoincrement not null,
    search_query Text not null
);
