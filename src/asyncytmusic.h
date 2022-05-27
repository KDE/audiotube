// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>

#include <vector>

#include <ytmusic.h>

Q_DECLARE_METATYPE(std::vector<artist::Artist::Album>);
Q_DECLARE_METATYPE(std::vector<search::SearchResultItem>)
Q_DECLARE_METATYPE(artist::Artist)
Q_DECLARE_METATYPE(album::Album)
Q_DECLARE_METATYPE(song::Song)
Q_DECLARE_METATYPE(playlist::Playlist)
Q_DECLARE_METATYPE(video_info::VideoInfo)
Q_DECLARE_METATYPE(watch::Playlist)
Q_DECLARE_METATYPE(std::optional<QString>)
Q_DECLARE_METATYPE(std::vector<meta::Artist>)
Q_DECLARE_METATYPE(meta::Artist)

///
/// Lazy initialized unique_ptr
///
template <typename T>
class Lazy {
public:
    T *operator->() {
        return get().operator->();
    }

    inline std::unique_ptr<T> &get() {
        if (!m_item) {
            m_item = std::make_unique<T>();
        }
        Q_ASSERT(m_item);
        return m_item;
    }

private:
    std::unique_ptr<T> m_item = nullptr;
};

///
/// Similar to QObject::connect, except that the first two arguments are replaced with a QFuture.
///
template <typename T, typename QObjectDerivedType, typename Function>
void connectFuture(const QFuture<T> &future, QObjectDerivedType *self, const Function &fun) {
    auto watcher = std::make_shared<QFutureWatcher<T>>();
    watcher->setFuture(future);
    QObject::connect(watcher.get(), &QFutureWatcherBase::finished, self, [self, watcher, fun, future] {
        if constexpr (std::is_same_v<void, T>) {
            if constexpr (std::is_member_function_pointer_v<Function>) {
                (self->*fun)();
            } else {
                fun();
            }
        } else if (future.resultCount() > 0) {
            if constexpr (std::is_member_function_pointer_v<Function>) {
                (self->*fun)(watcher->result());
            } else {
                fun(watcher->result());
            }
        }
    });
}

///
/// Applies a function to a the contained value of a QFuture once it finishes.
/// @returns a QFuture that finishes once the future given as first argument finished
///
template <typename T, typename Func>
QFuture<std::invoke_result_t<Func, T>> mapFuture(const QFuture<T> &future, Func mapFunction) {
    using ReturnType = std::invoke_result_t<Func, T>;
    auto watcher = std::make_shared<QFutureWatcher<T>>();
    watcher->setFuture(future);

    auto interface = std::make_shared<QFutureInterface<ReturnType>>();
    QObject::connect(watcher.get(), &QFutureWatcherBase::finished, watcher.get(), [interface, watcher, mapFunction] {
        auto result = watcher->result();
        auto mapped = mapFunction(std::move(result));
        interface->reportResult(mapped);
        interface->reportFinished();
    });

    return interface->future();
}

class AsyncYTMusic : public QObject
{
    friend class YTMusicThread;

    Q_OBJECT

public:
    // public functions need to be thread safe
    QFuture<std::vector<search::SearchResultItem>> search(const QString &query);

    QFuture<artist::Artist> fetchArtist(const QString &channelId);

    QFuture<album::Album> fetchAlbum(const QString &browseId);

    QFuture<std::optional<song::Song> > fetchSong(const QString &videoId);

    QFuture<playlist::Playlist> fetchPlaylist(const QString &playlistId);

    QFuture<std::vector<artist::Artist::Album>> fetchArtistAlbums(const QString &channelId, const QString &params);

    QFuture<video_info::VideoInfo> extractVideoInfo(const QString &videoId);

    QFuture<watch::Playlist> fetchWatchPlaylist(const std::optional<QString> &videoId = std::nullopt ,
                            const std::optional<QString> &playlistId = std::nullopt);

    Q_SIGNAL void errorOccurred(const QString &error);

protected:
    explicit AsyncYTMusic(QObject *parent = nullptr);

private:
    /// Invokes the given function on the thread of the YTMusic object, and handles exceptions that occur while invoking it.
    template <typename Func>
    QFuture<std::invoke_result_t<Func>> invokeAndCatchOnThread(Func fun) {
        using ReturnType = std::invoke_result_t<Func>;
        auto interface = std::make_shared<QFutureInterface<ReturnType>>();
        QMetaObject::invokeMethod(this, [=, this]() {
            try {
                ReturnType val = fun();
                interface->reportResult(val);
                interface->reportFinished();
            } catch (const std::exception &err) {
                interface->reportFinished();
                Q_EMIT errorOccurred(QString::fromLocal8Bit(err.what()));
            }
        });
        return interface->future();
    }

    // Python interpreter will be initialized from the thread calling the methods
    Lazy<YTMusic> m_ytm;
};

class YTMusicThread : private QThread {
public:
    static YTMusicThread &instance();
    ~YTMusicThread() override;

    AsyncYTMusic *operator->();
    AsyncYTMusic &get();

private:
    YTMusicThread();

    AsyncYTMusic *m_ytm;
};
