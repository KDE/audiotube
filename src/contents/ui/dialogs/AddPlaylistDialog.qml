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
    id: addPlaylistDialog
    title: i18n("Add playlist")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    mainItem: Kirigami.FormLayout {
        Controls.TextField {
            id: titleField

            Kirigami.FormData.label: i18n("Playlist Title")
        }
        Controls.TextField {
            id: descriptionField

            Kirigami.FormData.label: i18n("Playlist Description")
        }
    }

    onAccepted: model.addPlaylist(titleField.text, descriptionField.text)
}
