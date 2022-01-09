# AudioTube

AudioTube is a client for YouTube Music.

# Dependencies

It depends on Qt, Kirigami, KI18n, KCoreAddons and KCrash in addition to a few python packages:
 * ytmusicapi
 * yt_dlp

YouTube uses webp images for the cover art, so you will need to install qtimageformats.

# Troubleshooting

AudioTube only needs Python 3. If cmake still selects Python 2, you can pass `-DPYTHON_EXECUTABLE=/usr/bin/python3` to correct that.
