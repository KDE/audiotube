create table songs (
    video_id Text primary key not null,
    title Text not null,
    artist Text not null,
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
    search_query Text primary key not null
)
