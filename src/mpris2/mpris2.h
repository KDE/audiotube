/***************************************************************************
   SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
   SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#pragma once

#include <QObject>
#include <memory>

class MediaPlayer2;
class MediaPlayer2Player;

class Mpris2 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString playerName
               READ playerName
               WRITE setPlayerName
               NOTIFY playerNameChanged)

public:
    explicit Mpris2(QObject* parent = nullptr);
    ~Mpris2() override;

    [[nodiscard]] QString playerName() const;

    MediaPlayer2Player *playerInterface() {
        return m_mp2p.get();
    }

    void setPlayerName(const QString &playerName);

Q_SIGNALS:
    void raisePlayer();

    void playerNameChanged();

    void playListModelChanged();

    void audioPlayerManagerChanged();

    void manageMediaPlayerControlChanged();

    void headerBarManagerChanged();

    void audioPlayerChanged();

    void showProgressOnTaskBarChanged();

private:

    void initDBusService();

    std::unique_ptr<MediaPlayer2> m_mp2;
    std::unique_ptr<MediaPlayer2Player> m_mp2p;
    QString m_playerName;
    bool mShowProgressOnTaskBar = true;
};
