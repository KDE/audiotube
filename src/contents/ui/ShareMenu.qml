// SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick.Controls as Controls
import QtQuick.Layouts 1.7
import QtQuick 2.7
import QtQuick.Window 2.15

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kirigamiaddons.components 1.0 as Components
import org.kde.purpose 1.0 as Purpose

import org.kde.ytmusic 1.0

Item{
    id:root
    property url url
    property string inputTitle
    function open(){
        if (Kirigami.Settings.isMobile){
            inputDrawer.open()
            inputDrawer.interactive = true
            drawerView.inputData = {
                "urls": [root.url.toString()],
                "title": "Title"

            }
        } else {
            inputSheet.open()
            view.inputData = {
                "urls": [root.url.toString()],
                "title": "Title"
            }
        }
    }

    Kirigami.Dialog {
        id: inputSheet
        title: i18n("Share to")
        preferredWidth: Kirigami.Units.gridUnit * 16
        standardButtons: Kirigami.Dialog.NoButton
        ColumnLayout {
            spacing:0
            KirigamiDelegates.SubtitleDelegate {
                Layout.fillWidth: true
                icon.name: "edit-copy"
                text: i18n("Copy Link")
                visible: view.depth === 1
                onClicked: {
                    Clipboard.text = root.url
                    inputSheet.close()
                    applicationWindow().showPassiveNotification(i18n("Link copied to clipboard"))
                }
            }
            Purpose.AlternativesView {
                Layout.fillHeight: true
                Layout.fillWidth: true
                id: view
                pluginType: "ShareUrl"
                clip: true

                delegate: Controls.ItemDelegate {
                    onClicked: view.createJob (shareDelegate.index)

                    contentItem: KirigamiDelegates.IconTitleSubtitle {
                        id: shareDelegate

                        required property string iconName
                        required property int index
                        required property string actionDisplay

                        title: shareDelegate.actionDisplay
                        Keys.onReturnPressed: view.createJob (shareDelegate.index)
                        Keys.onEnterPressed: view.createJob (shareDelegate.index)
                        icon.name: shareDelegate.iconName

                        Kirigami.Icon {
                            implicitWidth: Kirigami.Units.iconSizes.small
                            implicitHeight: Kirigami.Units.iconSizes.small
                            source: "arrow-right"
                        }
                    }
                }

                onFinished: inputSheet.close()
            }
        }
    }

    Components.BottomDrawer {
        id: inputDrawer

        parent: applicationWindow().overlay

        drawerContentItem: ColumnLayout {
            spacing:0
            KirigamiDelegates.SubtitleDelegate {
                Layout.fillWidth: true
                icon.name: "edit-copy"
                text: i18n("Copy Link")
                visible: drawerView.depth === 1
                onClicked: {
                    Clipboard.text = root.url
                    inputDrawer.close()
                    applicationWindow().showPassiveNotification(i18n("Link copied to clipboard"))
                }
            }
            Purpose.AlternativesView {
                id: drawerView
                pluginType: "ShareUrl"
                clip: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                delegate: KirigamiDelegates.SubtitleDelegate {
                    id: shareDrawerDelegate

                    required property string iconName
                    required property int index

                    text: shareDrawerDelegate.display
                    onClicked: drawerView.createJob (shareDrawerDelegate.index)
                    Keys.onReturnPressed: view.createJob (shareDrawerDelegate.index)
                    Keys.onEnterPressed: view.createJob (shareDrawerDelegate.index)
                    icon.name: shareDrawerDelegate.iconName

                    Kirigami.Icon {
                        implicitWidth: Kirigami.Units.iconSizes.small
                        implicitHeight: Kirigami.Units.iconSizes.small
                        source: "arrow-right"
                    }
                }

                onFinished: inputDrawer.close()
            }

            Item{
                Layout.fillHeight: true
            }

        }
    }
}
