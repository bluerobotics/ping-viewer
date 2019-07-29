import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import NotificationManager 1.0
import StyleManager 1.0

Item {
    id: root
    visible: repeater.count
    height: pingItem.height
    width: pingItem.width

    PingItem {
        id: pingItem
        icon: StyleManager.bellIcon()
        state: "bottom-right"
        visible: true
        margin: 0
        item: ColumnLayout {
            id: mainNotificationLayout
            spacing: 5
            ScrollView {
                id: scroll
                clip: true
                Layout.maximumHeight: window.height/2

                ScrollBar.vertical: ScrollBar {
                    // This will help to find which ScrollBar is not being used
                    objectName: "verticalBar"
                    interactive: false
                    policy: ScrollBar.AlwaysOn
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    // We control the scrollbar size here
                    // to allow a better control inside the RowLayout leftMargin
                    width: 7
                }

                Component.onCompleted: {
                    // This will delete all scrollbars that are not used
                    // Doing: ScrollBar.vertical: ScrollBar {
                    // does not remove old childrens
                    // This is possible to check with gammaray
                    // where it's possible to see the QQuickScrollBar items
                    // that overlap in the right and bottom part of the widget
                    for(var i in scroll.data) {
                        if(scroll.data[i].objectName === "" && scroll.data[i].toString().includes("QQuickScrollBar")) {
                            scroll.data[i].destroy()
                        }
                    }
                }

                ColumnLayout {
                    Text {
                        text: "Notifications:"
                        color: Material.primary
                        Layout.alignment: Qt.AlignHCenter
                        Layout.margins: 5
                    }
                    Repeater {
                        id: repeater
                        model: NotificationManager.model

                        ColumnLayout {
                            Rectangle {
                                height: 1
                                Layout.fillWidth: true
                                color: Material.primary
                            }

                            RowLayout {
                                // This values depends of vertical scrollbar
                                Layout.leftMargin: 10
                                Layout.fillWidth: true

                                PingImage {
                                    id: iconItem
                                    Layout.preferredWidth: 30
                                    Layout.preferredHeight: 30
                                    Layout.alignment: Qt.AlignTop
                                    source: icon
                                }

                                Text {
                                    id: timeItem
                                    text: time
                                    Layout.alignment: Qt.AlignTop
                                    color: foreground
                                }

                                Text {
                                    id: textItem
                                    text: display
                                    Layout.alignment: Qt.AlignTop
                                    color: foreground
                                    height: iconItem.height
                                    // Set max width
                                    Layout.preferredWidth: textItem.font.pixelSize*40
                                    horizontalAlignment: Text.AlignJustify
                                    wrapMode: Text.WordWrap
                                    // Open link
                                    onLinkActivated: Qt.openUrlExternally(link)
                                    // This variable shows if the text is bigger than 2 lines
                                    property bool isBigText: false

                                    Component.onCompleted: {
                                        textItem.isBigText = textItem.lineCount > 2
                                        textItem.maximumLineCount = textItem.isBigText ? 2 : textItem.lineCount
                                    }


                                    MouseArea {
                                        anchors.fill: parent
                                        // Do not handle clicks to expand text is a link
                                        acceptedButtons: parent.hoveredLink ? Qt.NoButton : Qt.LeftButton
                                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                                        onClicked: {
                                            if(!textItem.isBigText) {
                                                return
                                            }
                                            var isSmall = textItem.maximumLineCount === 2
                                            textItem.maximumLineCount = isSmall ? -1 : 2
                                            gradient.visible = !isSmall
                                        }
                                    }

                                    LinearGradient  {
                                        id: gradient
                                        anchors.fill: textItem
                                        gradient: Gradient {
                                            GradientStop { position: 0; color: "transparent" }
                                            GradientStop { position: 2; color: "black" }
                                        }
                                        Component.onCompleted: gradient.visible = textItem.isBigText
                                    }
                                }

                                PingImage {
                                    Layout.preferredWidth: 30
                                    Layout.preferredHeight: 30
                                    Layout.alignment: Qt.AlignTop
                                    source: StyleManager.closeIcon()
                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: NotificationManager.model.remove(index)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            PingImage {
                source: StyleManager.clearIcon()
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                height: 36
                Layout.fillWidth: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    visible: repeater.count
                    onClicked: NotificationManager.model.removeAll()
                }
            }
        }
    }
}
