-- SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
--
-- SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

create table playlists (
    playlist_id Integer primary key autoincrement not null,
    title Text not null,
    description Text,
    created_on Timestamp not null default current_timestamp
);

create table playlist_entries (
    playlist_id Integer not null,
    video_id Text not null,
    primary key (playlist_id, video_id)
    foreign key (video_id) references songs(video_id),
    foreign key (playlist_id) references playlists(playlist_id)
);
