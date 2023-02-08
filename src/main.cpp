// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KCrash>
#include <KAboutData>
#include <QQuickWindow>

#include "searchmodel.h"
#include "albummodel.h"
#include "videoinfoextractor.h"
#include "artistmodel.h"
#include "userplaylistmodel.h"
#include "errorhandler.h"
#include "playlistmodel.h"
#include "playerutils.h"
#include "library.h"
#include "blur.h"

#include "asyncdatabase.h"

constexpr auto URI = "org.kde.ytmusic";

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("audiotube"));

    KLocalizedString::setApplicationDomain("audiotube");

    KAboutData about(QStringLiteral("audiotube"),
                     i18n("AudioTube"),
                     QStringLiteral(AUDIOTUBE_VERSION_STRING),
                     i18n("YouTube Music Player"),
                     KAboutLicense::GPL_V3,
                     i18n("© 2021-2023 Jonah Brüchert, 2021-2023 KDE Community"));
    about.addAuthor(i18n("Jonah Brüchert"), i18n("Maintainer"), QStringLiteral("jbb@kaidan.im"));
    about.addAuthor(i18n("Mathis Brüchert"), i18n("Designer"), QStringLiteral("mbb@kaidan.im"));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=audiotube");
    auto future = YTMusicThread::instance()->version();
    connectFuture(future, &app, [&about](const auto &version) {
        about.addComponent(QStringLiteral("ytmusicapi"), i18n("Unofficial API for YouTube Music"), version);
        KAboutData::setApplicationData(about);
    });

    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.audiotube")));

    KCrash::initialize();

    QQmlApplicationEngine engine;

    qmlRegisterType<SearchModel>(URI, 1, 0, "SearchModel");
    qmlRegisterType<AlbumModel>(URI, 1, 0, "AlbumModel");
    qmlRegisterType<VideoInfoExtractor>(URI, 1, 0, "VideoInfoExtractor");
    qmlRegisterType<ArtistModel>(URI, 1, 0, "ArtistModel");
    qmlRegisterType<PlaylistModel>(URI, 1, 0, "PlaylistModel");
    qmlRegisterUncreatableType<PlaybackHistoryModel>(URI, 1, 0, "PlaybackHistoryModel","");
    qmlRegisterType<QSortFilterProxyModel>(URI, 1, 0, "SortFilterModel");
    qmlRegisterType<Blur>(URI, 1, 0, "Blur");
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

    qmlRegisterSingletonInstance<Library>(URI, 1, 0, "Library", &Library::instance());
    qmlRegisterType<ThumbnailSource>(URI, 1, 0, "ThumbnailSource");
    qmlRegisterAnonymousType<FavouriteWatcher>(URI, 1);
    qmlRegisterAnonymousType<WasPlayedWatcher>(URI, 1);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
