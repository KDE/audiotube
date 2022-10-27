# AudioTube

AudioTube is a client for YouTube Music.

<a href='https://flathub.org/apps/details/org.kde.audiotube'><img width='190px' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-i-en.png'/></a>

## Links
* Project page: https://invent.kde.org/plasma-mobile/audiotube
* File issues: https://bugs.kde.org/describecomponents.cgi?product=audiotube

# Dependencies

It depends on Qt, Kirigami, KI18n, KCoreAddons and KCrash in addition to a few python packages:
 * ytmusicapi
 * yt_dlp

YouTube uses webp images for the cover art, so you will need to install qtimageformats.

# Troubleshooting

AudioTube only needs Python 3. If cmake still selects Python 2, you can pass `-DPYTHON_EXECUTABLE=/usr/bin/python3` to correct that.
