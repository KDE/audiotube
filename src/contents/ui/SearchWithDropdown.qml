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
        id: searchField
        autoAccept: false
        width: root.width
        selectByMouse: true
        onFocusChanged: {
            if (wideScreen && focus)
                popup.open()
        }

        onTextEdited: {
            Library.searches.filter = text
            if(completionList.count === 0) {
                //no matching search -> message should display
                Library.temporarySearch = ""
            }
            else {
                Library.temporarySearch = text
            }
        }

        Keys.onPressed: {
            if(completionList.count > 0) {
                if (event.key === Qt.Key_Down) {
                    if(completionList.selectedDelegate === -1 || completionList.selectedDelegate === completionList.count - 1) {
                        completionList.selectedDelegate = 0
                        mainScrollView.contentItem.contentY = 0
                    }
                    else {
                        ++completionList.selectedDelegate
                        if(!completionList.isSelectedDelegateVisible()) {
                            if(!recentsRepeater.visible) {
                                mainScrollView.contentItem.contentY = (completionList.selectedDelegate + 1) * completionList.empiricDelegateHeight - mainScrollView.height
                            }
                            else {
                                mainScrollView.contentItem.contentY = (completionList.selectedDelegate + 1) * completionList.empiricDelegateHeight + recentsRepeater.height + mainScrollViewLayout.spacing - mainScrollView.height
                            }
                        }
                    }
                    event.accepted = true
                }
                else if(event.key === Qt.Key_Up) {
                    if(completionList.selectedDelegate === -1 || completionList.selectedDelegate === 0) {
                        completionList.selectedDelegate = completionList.count - 1
                        mainScrollView.contentItem.contentY = mainScrollView.contentHeight - mainScrollView.height
                    }
                    else {
                        --completionList.selectedDelegate
                        if(!completionList.isSelectedDelegateVisible()) {
                            if(!recentsRepeater.visible) {
                                mainScrollView.contentItem.contentY = completionList.empiricDelegateHeight * completionList.selectedDelegate
                            }
                            else {
                                mainScrollView.contentItem.contentY = completionList.empiricDelegateHeight * completionList.selectedDelegate + recentsRepeater.height + mainScrollViewLayout.spacing
                            }
                        }
                    }
                    event.accepted = true
                }
                if(event.accepted) {
                    text = completionList.itemAt(completionList.selectedDelegate).text
                }
                else {
                    completionList.selectedDelegate = -1
                    mainScrollView.contentItem.contentY = 0
                }
            }
        }

        onAccepted: {
            popup.close()
            completionList.selectedDelegate = -1
            Library.temporarySearch = ""

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
                ? (Library.searches.filter && recentsRepeater.count > 0 //can't use recentsRepeater.visible directly, because it always returns false at this stage
                    ? (Math.min(fieldContainer.height+ (completionList.count) * (completionList.delegateHeight + completionList.delegatePadding) + Kirigami.Separator.implicitHeight + recentsRepeater.implicitHeight, Kirigami.Units.gridUnit * 20))+2*(popup.shadowSize+popup.expansion)
                    : (completionList.count === 0
                        ?(Math.min(fieldContainer.height+ noMatchingSearchLabel.height + 2*noMatchingSearchLabel.Layout.margins, Kirigami.Units.gridUnit * 20))+2*(popup.shadowSize+popup.expansion)
                        :(Math.min(fieldContainer.height+ (completionList.count) * (completionList.delegateHeight + 2.725*completionList.delegatePadding) + 2*mainScrollViewLayout.spacing, Kirigami.Units.gridUnit * 20))+2*(popup.shadowSize+popup.expansion)
                    )
                )
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
            to: searchField.height
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
                id: mainScrollView

                clip: true
                
                Layout.fillHeight: true
                Layout.fillWidth: true

                Keys.enabled: false
                contentWidth: availableWidth

                ColumnLayout {
                    id: mainScrollViewLayout

                    width: mainScrollView.contentWidth
                    
                    HorizontalCoverView {
                        id: recentsRepeater
                        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff

                        contentHeight: 120
                        itemSpacing: 0

                        Layout.fillWidth: true

                        visible: Library.searches.filter && recentsRepeater.count > 0 //if changed, adjust playOpenHeight

                        model: LocalSearchModel {
                            searchQuery: Library.searches.filter
                        }

                        delegate: searchAlbum
                    }
                    Kirigami.Separator {
                        visible: recentsRepeater.visible
                        Layout.fillWidth: true
                        implicitWidth: popup.width
                    }
                    RowLayout{
                        id: noMatchingSearchLabel

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

                        property int selectedDelegate: -1
                        property int delegateHeight: Kirigami.Units.gridUnit
                        property int delegatePadding: Kirigami.Units.smallSpacing
                        property int empiricDelegateHeight: recentsRepeater.visible ? (mainScrollView.contentHeight - recentsRepeater.height) / count : (mainScrollView.contentHeight + mainScrollViewLayout.spacing) / (count)

                        id: completionList
                        model: Library.searches

                        function isSelectedDelegateVisible() {
                            if(!recentsRepeater.visible) {
                                return selectedDelegate * empiricDelegateHeight > mainScrollView.contentItem.contentY && (selectedDelegate + 1) * empiricDelegateHeight < mainScrollView.contentItem.contentY + mainScrollView.height
                            }
                            else {
                                    return selectedDelegate * empiricDelegateHeight + recentsRepeater.height > mainScrollView.contentItem.contentY && (selectedDelegate + 1) * empiricDelegateHeight + recentsRepeater.height < mainScrollView.contentItem.contentY + mainScrollView.height
                            }
                        }

                        delegate: Kirigami.AbstractListItem {
                            id: completionDelegate
                            highlighted: focus || (completionList.selectedDelegate == index)
                            Kirigami.Theme.colorSet: Kirigami.Theme.Window
                            Kirigami.Theme.inherit: false
                            Layout.fillWidth: true
                            height: completionList.delegateHeight
                            text: model.display
                            padding: completionList.delegatePadding
                            spacing: 0

                            contentItem: RowLayout {
                                Kirigami.Icon {
                                    source: "search"
                                    implicitHeight: completionList.delegateHeight
                                    implicitWidth: completionList.delegateHeight
                                    color: Kirigami.Theme.disabledTextColor
                                }
                                Controls.Label {
                                    text: model.display
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                                Controls.ToolButton {
                                    icon.name: "list-remove"
                                    text: i18n("remove from search history")
                                    display: Controls.AbstractButton.IconOnly
                                    onClicked: {
                                        Library.removeSearch(model.display)
                                    }
                                    implicitHeight: completionList.delegateHeight
                                    visible: Library.temporarySearch == "" || index != 0
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
