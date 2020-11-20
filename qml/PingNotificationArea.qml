import NotificationManager 1.0
import QtGraphicalEffects 1.0
import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import StyleManager 1.0

PingItem {
    id: pingItem

    icon: StyleManager.bellIcon()
    state: "bottom-right"
    visible: scroll.count

    item: PingGroupBox {
        title: "Notifications:"
        spacing: 0
        implicitHeight: Math.min(300, window.height)
        implicitWidth: Math.min(500, window.width * 2 / 3)

        ColumnLayout {
            anchors.fill: parent

            ListView {
                id: scroll

                clip: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: NotificationManager.model

                delegate: Column {
                    Rectangle {
                        height: 1
                        width: scroll.width
                        color: Material.primary
                    }

                    RowLayout {
                        width: scroll.width

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

                            // This variable shows if the text is bigger than 2 lines
                            property bool isBigText: false

                            text: display
                            Layout.alignment: Qt.AlignTop
                            color: foreground
                            height: closeIcon.height
                            // Set max width
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignJustify
                            wrapMode: Text.WordWrap
                            // Open link
                            onLinkActivated: Qt.openUrlExternally(link)
                            Component.onCompleted: {
                                textItem.isBigText = textItem.lineCount > 2;
                                textItem.maximumLineCount = textItem.isBigText ? 2 : textItem.lineCount;
                            }

                            MouseArea {
                                anchors.fill: parent
                                // Do not handle clicks to expand text is a link
                                acceptedButtons: parent.hoveredLink ? Qt.NoButton : Qt.LeftButton
                                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                                onClicked: {
                                    if (!textItem.isBigText)
                                        return ;

                                    var isSmall = textItem.maximumLineCount === 2;
                                    textItem.maximumLineCount = isSmall ? -1 : 2;
                                    gradient.visible = !isSmall;
                                }
                            }

                            LinearGradient {
                                id: gradient

                                anchors.fill: textItem
                                Component.onCompleted: gradient.visible = textItem.isBigText

                                gradient: Gradient {
                                    GradientStop {
                                        position: 0
                                        color: "transparent"
                                    }

                                    GradientStop {
                                        position: 2
                                        color: "black"
                                    }

                                }

                            }

                        }

                        PingImage {
                            id: closeIcon

                            width: 30
                            height: 30
                            Layout.alignment: Qt.AlignBottom
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

            PingImage {
                source: StyleManager.clearIcon()
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                height: 36
                width: 36

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    visible: scroll.count
                    onClicked: NotificationManager.model.removeAll()
                }

            }

        }

    }

}
