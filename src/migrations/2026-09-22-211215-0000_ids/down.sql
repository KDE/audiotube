-- This file should undo anything in `up.sql`
alter table songs drop column artist_id;
alter table songs drop column album_id;
