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

#include <KAboutData>
#include <KCrash>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>

#include <ThreadedDatabase>

#include "asyncytmusic.h"

using namespace Qt::StringLiterals;

class StyleFallback
{
public:
    StyleFallback()
    {
        if (QGuiApplication::instance()) {
            qFatal("StyleFallback must be created before Q(Gui)Application");
        }

        KIconTheme::initTheme();

        m_initialQuickControlsStyle = QQuickStyle::name();
    }

    void setup(QGuiApplication *app)
    {
        Q_ASSERT(app); // Force people to run this at the right time

        // Check if the platformtheme or user set up a style for us
        if (m_initialQuickControlsStyle != QQuickStyle::name()
            || !qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
            return;
        }

        qWarning() << "Detected that the platform did not set up a style, using defaults";

        // platformtheme did not handle QtQuick styling, set everything to fallback values
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
        QApplication::setStyle(QStyleFactory::create(QStringLiteral("Breeze")));
        QIcon::setThemeName(QStringLiteral("breeze"));
    }

private:
    QString m_initialQuickControlsStyle;
};

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    StyleFallback fallback;
    QApplication app(argc, argv);
    fallback.setup(&app);

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
                     i18n("© 2021–2024 Jonah Brüchert, 2021–2024 KDE Community"));
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

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.audiotube", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
