// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.ytmusic 1.0
import ".."


Kirigami.PromptDialog {
    property var modelData
    property LocalPlaylistsModel playlistsModel
    id: addPlaylistDialog
    title: i18n("Rename playlist")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    mainItem: Kirigami.FormLayout {
        Controls.TextField {
            id: titleField

            Kirigami.FormData.label: i18n("Playlist Title")
            text: modelData ? modelData.title : ""
        }
        Controls.TextField {
            id: descriptionField

            Kirigami.FormData.label: i18n("Playlist Description")
            text: modelData ? modelData.description : ""
        }
    }

    onAccepted: playlistsModel.renamePlaylist(modelData.playlistId, titleField.text, descriptionField.text)
}
