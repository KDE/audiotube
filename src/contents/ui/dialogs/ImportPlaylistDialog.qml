// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.audiotube

Kirigami.PromptDialog {
    property QtObject model
    id: importPlaylistDialog
    title: i18n("Import playlist")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    mainItem: Kirigami.FormLayout {
        Controls.TextField {
            id: urlField

            Kirigami.FormData.label: i18n("Playlist URL (Youtube)")
        }
    }

    onAccepted: model.importPlaylist(urlField.text)
}
