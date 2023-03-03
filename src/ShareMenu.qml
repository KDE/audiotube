// SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick.Controls 2.1 as Controls
import QtQuick.Layouts 1.7
import QtQuick 2.7
import QtQuick.Window 2.15

import org.kde.kirigami 2.20 as Kirigami
import org.kde.purpose 1.0 as Purpose
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

        Purpose.AlternativesView {
            id: view
            pluginType: "ShareUrl"
            clip: true

            delegate: Kirigami.BasicListItem {
                id: shareDelegate

                required property string iconName
                required property string display
                required property int index

                label: shareDelegate.display
                onClicked: view.createJob (shareDelegate.index)
                Keys.onReturnPressed: view.createJob (shareDelegate.index)
                Keys.onEnterPressed: view.createJob (shareDelegate.index)
                leading: Kirigami.Icon {
                    source: shareDelegate.iconName
                }
                trailing: Kirigami.Icon {
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                    source: "arrow-right"
                }
            }

            onFinished: close()
        }
    }

    Controls.Drawer {
        id: inputDrawer
        edge: Qt.BottomEdge
        height:contents.implicitHeight+20
        width: applicationWindow().width
        interactive: false
        background: Kirigami.ShadowedRectangle{
            corners.topRightRadius: 10
            corners.topLeftRadius: 10
            shadow.size: 20
            shadow.color: Qt.rgba(0, 0, 0, 0.5)
            color: Kirigami.Theme.backgroundColor

        }
        onClosed: inputDrawer.interactive = false

        ColumnLayout {
            id: contents
            anchors.fill: parent
            Rectangle {
                Layout.margins: 5
                radius:50
                Layout.alignment: Qt.AlignHCenter
                color: Kirigami.Theme.textColor
                opacity: 0.7
                width: 40
                height: 4

            }
            Kirigami.Heading{
                text: i18n("Share to")
                leftPadding: 20

            }
            Purpose.AlternativesView {
                id: drawerView
                pluginType: "ShareUrl"
                clip: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                delegate: Kirigami.BasicListItem {
                    id: shareDrawerDelegate

                    required property string iconName
                    required property string display
                    required property int index

                    label: shareDrawerDelegate.display
                    onClicked: drawerView.createJob (shareDrawerDelegate.index)
                    Keys.onReturnPressed: view.createJob (shareDrawerDelegate.index)
                    Keys.onEnterPressed: view.createJob (shareDrawerDelegate.index)
                    leading: Kirigami.Icon {
                        source: shareDrawerDelegate.iconName
                    }
                    trailing: Kirigami.Icon {
                        implicitWidth: Kirigami.Units.iconSizes.small
                        implicitHeight: Kirigami.Units.iconSizes.small
                        source: "arrow-right"
                    }
                }

                onFinished: close()
            }

            Item{
                Layout.fillHeight: true
            }

        }
    }
}
