// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.1
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.14 as Kirigami
import QtGraphicalEffects 1.0
import org.kde.ytmusic 1.0

Item {
    function forceFocus(){searchField.forceActiveFocus()}
    function accepted(){searchField.accepted()}
    property alias filterExpression: searchField.filterExpression
    property alias text: searchField.text

    id: root
    Controls.TextField{
        id: dummy
        x:searchField.x
        y:searchField.y
        width: searchField.width
        readOnly: true
    }
    Kirigami.SearchField {
        property var filterExpression: new RegExp(`.*${searchField.text}.*`, "i")

        id: searchField
        autoAccept: false
        width: root.width
        selectByMouse: true
        onFocusChanged: {
            if (wideScreen && focus)
                popup.open()
        }
        onAccepted: {
            popup.close()

            while (pageStack.depth > 0) {
                pageStack.pop()
            }

            pageStack.layers.clear()

            if (text) {
                Library.addSearch(text)
                pageStack.push("qrc:/SearchPage.qml", {
                            "searchQuery": text,
                            objectName: "searchPage"
                               })
            } else {
                wideScreen
                        ? pageStack.push("qrc:/LibraryPage.qml")
                        : pageStack.push("qrc:/SearchHistoryPage.qml")
            }
            searchField.focus = false

        }
    }
    Controls.Popup {
        id: popup
        property int expansion: 5
        property int shadowSize: 15
        onAboutToShow:{
            searchField.parent = fieldContainer
            onOpened: searchField.background.visible = false
            playOpenHeight.running = true
            playOpenWidth.running = true
            playOpenX.running = true
            playOpenY.running = true
            playOpenRadius.running = true

        }
        onAboutToHide:{
            searchField.parent = root
            onOpened: searchField.background.visible = true
            searchField.focus = false
            playCloseHeight.running = true
            playCloseWidth.running = true
            playCloseX.running = true
            playCloseY.running = true
            playCloseRadius.running = true

        }

        x: -(popup.shadowSize+popup.expansion)
        y: -(popup.shadowSize+popup.expansion)

        rightPadding:popup.shadowSize+1
        leftPadding:popup.shadowSize+1
        bottomPadding:popup.shadowSize
        rightInset: popup.shadowSize
        leftInset: popup.shadowSize
        bottomInset: popup.shadowSize
        leftMargin:-popup.shadowSize

        background: Kirigami.ShadowedRectangle{
            id: bg
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            color: Kirigami.Theme.backgroundColor
            radius: popup.expansion+2
            shadow.size: popup.shadowSize
            shadow.yOffset: popup.expansion
            shadow.color: Qt.rgba(0, 0, 0, 0.2)

            border.width: 1
            border.color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.3);
            NumberAnimation on radius{
                id: playOpenRadius
                easing.type: Easing.OutCubic
                running: false
                from: 2
                duration: 100
                to: popup.expansion+2
            }
            NumberAnimation on radius{
                id: playCloseRadius
                easing.type: Easing.OutCubic
                running: false
                from: popup.expansion+2
                duration: 100
                to: 2
            }

        }
        width: searchField.width + 2*(popup.shadowSize+popup.expansion)
        height: completionList
                ? (Math.min(content.implicitHeight, Kirigami.Units.gridUnit * 20))+2*(popup.shadowSize+popup.expansion)
                : (Kirigami.Units.gridUnit * 20)+2*(popup.shadowSize+popup.expansion)

        NumberAnimation on height{
            id: playOpenHeight
            easing.type: Easing.OutCubic
            running: false
            from: searchField.height
            duration: 200
            to: completionList
                ? (Math.min(fieldContainer.height+ completionList.count * Kirigami.Units.gridUnit * 2 + recents.implicitHeight, Kirigami.Units.gridUnit * 20))+2*(popup.shadowSize+popup.expansion)
                : (Kirigami.Units.gridUnit * 20)+2*(popup.shadowSize+popup.expansion)
        }
        NumberAnimation on width{
            id: playOpenWidth
            easing.type: Easing.OutCubic
            running: false
            from: searchField.width
            duration: 100
            to: searchField.width +2*(popup.shadowSize+popup.expansion)
        }
        NumberAnimation on x{
            id: playOpenX
            easing.type: Easing.OutCubic
            running: false
            from: 0
            duration: 100
            to: -(popup.shadowSize+popup.expansion)
        }
        NumberAnimation on y{
            id: playOpenY
            easing.type: Easing.OutCubic
            running: false
            from: 0
            duration: 100
            to: -popup.expansion
        }



        NumberAnimation on height{
            id: playCloseHeight
            easing.type: Easing.OutCubic
            running: false
            from: completionList
                  ? (Math.min(content.implicitHeight, Kirigami.Units.gridUnit * 20))+2*(popup.shadowSize+popup.expansion)
                  : (Kirigami.Units.gridUnit * 20)+2*(popup.shadowSize+popup.expansion)
            duration: 100
            to: searchField.heigth
        }
        NumberAnimation on width{
            id: playCloseWidth
            easing.type: Easing.OutCubic
            running: false
            from: searchField.width + 2*(popup.shadowSize+popup.expansion)
            duration: 100
            to: searchField.width + 2*(popup.shadowSize)
        }
        NumberAnimation on x{
            id: playCloseX
            easing.type: Easing.OutCubic
            running: false
            from: -(popup.shadowSize+popup.expansion)
            duration: 100
            to: -popup.shadowSize
        }
        NumberAnimation on y{
            id: playCloseY
            easing.type: Easing.OutCubic
            running: false
            from: - popup.expansion
            duration: 100
            to: -0
        }




        contentItem: ColumnLayout{
            id: content
            width: parent.width
            Controls.Control{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Controls.Control{
                    x: popup.expansion
                    y:-popup.expansion
                    id: fieldContainer
                    height:searchField.height

                }
                implicitHeight: fieldContainer.height
            }

            Controls.ScrollView {
                Layout.fillHeight: true
                Layout.fillWidth: true

                contentWidth: -1
                Keys.enabled: false

                contentItem: ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Controls.ScrollView {
                        id: recents

                        Layout.fillWidth: true
                        Layout.maximumWidth: popup.width

                        visible: searchField.text && recentsRepeater.count > 0

                        leftPadding: 10
                        rightPadding: 40
                        topPadding: 10
                        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                        RowLayout {
                            spacing: 10
                            Layout.fillWidth: true
                            Layout.maximumWidth: popup.width - 23

                            Repeater {

                                id: recentsRepeater
                                Layout.fillWidth: true
                                model: SortFilterModel {
                                    filterRole: PlaybackHistoryModel.Title
                                    filterRegularExpression: searchField.filterExpression
                                    sourceModel: Library.playbackHistory
                                }
                                delegate: Kirigami.DelegateRecycler {
                                    Layout.alignment: Qt.AlignTop
                                    width: popup.width
                                    sourceComponent: searchAlbum
                                }
                            }
                        }
                    }
                    Kirigami.Separator{
                        Layout.fillWidth: true
                        visible: searchField.text && recentsRepeater.count > 0
                        implicitWidth: popup.width

                    }
                    RowLayout{
                        Layout.margins: 10
                        visible: completionList.count == 0
                        Kirigami.Icon {
                            source: "documentinfo"
                            implicitHeight: Kirigami.Units.gridUnit
                            implicitWidth: Kirigami.Units.gridUnit
                            color: Kirigami.Theme.disabledTextColor
                        }
                        Controls.Label {
                            text: i18n("No matching previous searches")
                            Layout.fillWidth: true
                            color: Kirigami.Theme.disabledTextColor

                        }
                    }
                    Repeater {
                        Layout.fillWidth: true

                        id: completionList
                        model: SortFilterModel {
                            sourceModel: Library.searches
                            filterRegularExpression: searchField.filterExpression
                        }
                        delegate: Kirigami.AbstractListItem {
                            id: completionDelegate
                            highlighted: focus
                            Kirigami.Theme.colorSet: Kirigami.Theme.Window
                            Kirigami.Theme.inherit: false
                            Layout.fillWidth: true
                            height: Kirigami.Units.gridUnit * 2
                            contentItem: RowLayout {

                                Kirigami.Icon {
                                    source: "search"
                                    implicitHeight: Kirigami.Units.gridUnit
                                    implicitWidth: Kirigami.Units.gridUnit
                                    color: Kirigami.Theme.disabledTextColor
                                }
                                Controls.Label{
                                    text: model.display
                                    Layout.fillWidth: true
                                }
                                Controls.ToolButton {
                                    icon.name: "list-remove"
                                    text: i18n("remove from search history")
                                    display: Controls.AbstractButton.IconOnly
                                    onClicked: {
                                        Library.removeSearch(model.display)
                                    }
                                }

                            }
                            onClicked: {
                                searchField.text = model.display
                                searchField.accepted()
                            }
                        }
                    }
                }
            }
        }
    }

}
