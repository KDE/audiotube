// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QStyle>
#include <QStyleFactory>

#include <KLocalizedContext>
#include <KLocalizedString>
#include <KCrash>
#include <KAboutData>

#include <QCoroQml>

#include <mprisplugin.h>

#include "clipboard.h"
#include "searchmodel.h"
#include "albummodel.h"
#include "videoinfoextractor.h"
#include "artistmodel.h"
#include "userplaylistmodel.h"
#include "errorhandler.h"
#include "playlistmodel.h"
#include "playerutils.h"
#include "library.h"
#include "thumbnailsource.h"
#include "blur.h"
#include "localplaylistmodel.h"
#include "localplaylistsmodel.h"
#include "playlistimporter.h"
#include "downloadmanager.h"

#include <ThreadedDatabase>

constexpr auto URI = "org.kde.ytmusic";

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // set default style and icon theme
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") && QQuickStyle::name().isEmpty()) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
        QApplication::setStyle(QStyleFactory::create(QStringLiteral("Breeze")));
    }

    // WORKAROUND: Force QtMultimedia gstreamer backend
    /*
      ffmpeg unfortunately loses the connection to youtube's servers with the following error message after ~2 minutes
      at the time of writing (2023-10-29 with ffmpeg 6.0 and Qt 6.6.1)
        [tls @ 0x561f31d3d500] Error in the pull function.
        [tls @ 0x561f31d3d500] IO error: Connection reset by peer
        [mov,mp4,m4a,3gp,3g2,mj2 @ 0x561f3225e000] Packet corrupt (stream = 0, dts = 4590016).
        [mov,mp4,m4a,3gp,3g2,mj2 @ 0x561f3225e000] stream 0, offset 0x19c06c: partial file
        [aac @ 0x561f3244bc00] Input buffer exhausted before END element found
    */
    qputenv("QT_MEDIA_BACKEND", "gstreamer");

    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("audiotube"));

    KLocalizedString::setApplicationDomain("audiotube");

    KAboutData about(QStringLiteral("audiotube"),
                     i18n("AudioTube"),
                     QStringLiteral(AUDIOTUBE_VERSION_STRING),
                     i18nc("YouTube Music is a music streaming service by Google", "Stream music from YouTube Music"),
                     KAboutLicense::GPL_V3,
                     i18n("© 2021-2023 Jonah Brüchert, 2021-2023 KDE Community"));
    about.addAuthor(i18n("Jonah Brüchert"), i18n("Maintainer"), QStringLiteral("jbb@kaidan.im"));
    about.addAuthor(i18n("Mathis Brüchert"), i18n("Designer"), QStringLiteral("mbb@kaidan.im"));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=audiotube");
    auto future = YTMusicThread::instance()->version();
    QCoro::connect(std::move(future), &app, [&about](const auto &version) {
        about.addComponent(QStringLiteral("ytmusicapi"),
                           i18nc("YouTube Music is a music streaming service by Google",
                                 "Unofficial API for YouTube Music"),
                           version);
        KAboutData::setApplicationData(about);
    });

    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.audiotube")));

    KCrash::initialize();

    QQmlApplicationEngine engine;

    MprisPlugin(&engine).registerTypes("org.nemomobile.qtmpris");
    QCoro::Qml::registerTypes();

    qmlRegisterType<SearchModel>(URI, 1, 0, "SearchModel");
    qmlRegisterType<AlbumModel>(URI, 1, 0, "AlbumModel");
    qmlRegisterType<VideoInfoExtractor>(URI, 1, 0, "VideoInfoExtractor");
    qmlRegisterType<ArtistModel>(URI, 1, 0, "ArtistModel");
    qmlRegisterType<PlaylistModel>(URI, 1, 0, "PlaylistModel");
    qmlRegisterUncreatableType<PlaybackHistoryModel>(URI, 1, 0, "PlaybackHistoryModel","");
    qmlRegisterType<QSortFilterProxyModel>(URI, 1, 0, "SortFilterModel");
    qmlRegisterType<Blur>(URI, 1, 0, "Blur");
    qmlRegisterType<LocalSearchModel>(URI, 1, 0, "LocalSearchModel");
    qmlRegisterType<LocalPlaylistModel>(URI, 1, 0, "LocalPlaylistModel");
    qmlRegisterType<LocalPlaylistsModel>(URI, 1, 0, "LocalPlaylistsModel");
    qmlRegisterType<PlaylistImporter>(URI, 1, 0, "PlaylistImporter");
    qmlRegisterSingletonType(URI, 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    qmlRegisterSingletonType<UserPlaylistModel>(URI, 1, 0, "UserPlaylistModel", [](QQmlEngine *, QJSEngine *) {
        return new UserPlaylistModel();
    });
    qmlRegisterSingletonType<ErrorHandler>(URI, 1, 0, "ErrorHandler", [](QQmlEngine *, QJSEngine *) {
        return new ErrorHandler();
    });

    qmlRegisterSingletonType<PlayerUtils>(URI, 1, 0, "PlayerUtils", [](QQmlEngine *, QJSEngine *) {
        return new PlayerUtils();
    });
    qmlRegisterSingletonType<Clipboard>(URI, 1, 0, "Clipboard", [](QQmlEngine *, QJSEngine *) {
        return new Clipboard();
    });
    qmlRegisterSingletonType<DownloadManager>(URI, 1, 0, "DownloadManager", [](QQmlEngine *, QJSEngine *) {
        return new DownloadManager();
    });

    qmlRegisterSingletonInstance<Library>(URI, 1, 0, "Library", &Library::instance());
    qmlRegisterType<ThumbnailSource>(URI, 1, 0, "ThumbnailSource");
    qmlRegisterType<DownloadedWatcher>(URI, 1, 0, "DownloadedWatcher");
    qmlRegisterAnonymousType<FavouriteWatcher>(URI, 1);
    qmlRegisterAnonymousType<WasPlayedWatcher>(URI, 1);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
