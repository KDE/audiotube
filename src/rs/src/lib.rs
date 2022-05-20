#[macro_use]
extern crate diesel;
#[macro_use]
extern crate diesel_migrations;

mod schema;

use std::error::Error;

use diesel::prelude::*;

use diesel::dsl::count;
use diesel::QueryResult;

use crate::schema::*;

embed_migrations!("migrations/");

pub struct LibraryDatabase {
    connection: SqliteConnection,
}

impl LibraryDatabase {
    pub fn new(storage_location: &str) -> Result<LibraryDatabase, Box<dyn Error>> {
        println!("Opening library Database");
        let connection = SqliteConnection::establish(storage_location)?;

        println!("Running migrations");
        embedded_migrations::run_with_output(&connection, &mut std::io::stdout())?;

        Ok(LibraryDatabase { connection })
    }

    fn add_song(&self, song: &ffi::Song) -> QueryResult<()> {
        let song = Song::from(song.clone());
        diesel::insert_or_ignore_into(songs::table)
            .values(song)
            .execute(&self.connection)?;
        Ok(())
    }

    fn add_play(&self, song: &ffi::Song) -> QueryResult<()> {
        self.add_song(song)?;
        diesel::insert_or_ignore_into(played_songs::table)
            .values(PlayedSong {
                video_id: song.video_id.clone(),
                plays: 1,
            })
            .execute(&self.connection)?;
        Ok(())
    }

    fn increment_plays(&self, song: &ffi::Song) -> QueryResult<()> {
        self.add_play(song)?;
        diesel::update(played_songs::table)
            .filter(played_songs::video_id.eq(&song.video_id))
            .set(played_songs::plays.eq(played_songs::plays + 1))
            .execute(&self.connection)?;
        Ok(())
    }

    fn is_favourited(&self, video_id: &str) -> QueryResult<bool> {
        let is_favourited: i64 = favourites::table
            .filter(favourites::video_id.eq(video_id))
            .select(count(favourites::video_id))
            .first(&self.connection)?;

        Ok(is_favourited > 0)
    }

    fn add_favourite(&self, song: &ffi::Song) -> QueryResult<()> {
        self.add_song(song)?;
        diesel::insert_or_ignore_into(favourites::table)
            .values(Favourite {
                video_id: song.video_id.clone(),
            })
            .execute(&self.connection)?;
        Ok(())
    }

    fn favourites(&self) -> QueryResult<Vec<ffi::Song>> {
        let favourites = favourites::table.load::<Favourite>(&self.connection)?;

        let mut songs = Vec::new();
        for fav in favourites {
            let song = songs::table
                .filter(songs::video_id.eq(fav.video_id))
                .get_result::<Song>(&self.connection)?;
            songs.push(song);
        }

        Ok(songs.into_iter().map(Into::<ffi::Song>::into).collect())
    }

    fn remove_favourite(&self, video_id: &str) -> QueryResult<()> {
        diesel::delete(favourites::table)
            .filter(favourites::video_id.eq(video_id))
            .execute(&self.connection)?;
        Ok(())
    }

    fn plays(&self) -> QueryResult<Vec<ffi::PlayedSong>> {
        let plays = played_songs::table.load::<PlayedSong>(&self.connection)?;

        let mut songs = Vec::new();
        for play in plays {
            let song = songs::table
                .filter(songs::video_id.eq(&play.video_id))
                .get_result::<Song>(&self.connection)?;
            songs.push(ffi::PlayedSong {
                video_id: song.video_id,
                title: song.title,
                artist: song.artist,
                plays: play.plays,
            })
        }

        Ok(songs)
    }

    fn add_search(&self, search_query: &str) -> QueryResult<()> {
        diesel::insert_or_ignore_into(searches::table)
            .values(Search {
                search_query: search_query.to_string(),
            })
            .execute(&self.connection)?;
        Ok(())
    }

    fn searches(&self) -> QueryResult<Vec<String>> {
        let searches = searches::table
            .load::<Search>(&self.connection)?
            .into_iter()
            .map(|s| s.search_query)
            .collect::<Vec<String>>();
        Ok(searches)
    }
}

fn new_library_database(storage_location: &str) -> Result<Box<LibraryDatabase>, Box<dyn Error>> {
    Ok(Box::from(LibraryDatabase::new(storage_location)?))
}

#[derive(Queryable, Insertable)]
pub struct Song {
    pub video_id: String,
    pub title: String,
    pub artist: String,
    pub album: Option<String>,
}

impl From<ffi::Song> for Song {
    fn from(song: ffi::Song) -> Song {
        Song {
            video_id: song.video_id,
            title: song.title,
            artist: song.artist,
            album: None,
        }
    }
}

impl Into<ffi::Song> for Song {
    fn into(self) -> ffi::Song {
        ffi::Song {
            video_id: self.video_id,
            title: self.title,
            artist: self.artist,
        }
    }
}

#[derive(Queryable, Insertable)]
pub struct PlayedSong {
    pub video_id: String,
    pub plays: i32,
}

#[derive(Queryable, Insertable)]
pub struct Favourite {
    pub video_id: String,
}

#[derive(Queryable, Insertable)]
#[table_name = "searches"]
pub struct Search {
    search_query: String,
}

#[cxx::bridge]
mod ffi {
    #[derive(Clone)]
    pub struct Song {
        pub video_id: String,
        pub title: String,
        pub artist: String,
    }

    pub struct PlayedSong {
        video_id: String,
        title: String,
        artist: String,
        plays: i32,
    }

    extern "Rust" {
        type LibraryDatabase;

        fn new_library_database(storage_location: &str) -> Result<Box<LibraryDatabase>>;
        fn increment_plays(self: &LibraryDatabase, song: &Song) -> Result<()>;
        fn add_favourite(self: &LibraryDatabase, song: &Song) -> Result<()>;
        fn is_favourited(&self, video_id: &str) -> Result<bool>;
        fn remove_favourite(&self, video_id: &str) -> Result<()>;
        fn favourites(self: &LibraryDatabase) -> Result<Vec<Song>>;
        fn plays(self: &LibraryDatabase) -> Result<Vec<PlayedSong>>;
        fn add_search(&self, search_query: &str) -> Result<()>;
        fn searches(&self) -> Result<Vec<String>>;
    }
}
