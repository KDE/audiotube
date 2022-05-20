table! {
    favourites (video_id) {
        video_id -> Text,
    }
}

table! {
    played_songs (video_id) {
        video_id -> Text,
        plays -> Integer,
    }
}

table! {
    songs (video_id) {
        video_id -> Text,
        title -> Text,
        artist -> Text,
        album -> Nullable<Text>,
    }
}

table! {
    searches (search_query) {
        search_query -> Text,
    }
}

joinable!(favourites -> songs (video_id));
joinable!(played_songs -> songs (video_id));

allow_tables_to_appear_in_same_query!(favourites, played_songs, songs, searches);
