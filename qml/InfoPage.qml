import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import Logger 1.0

Item {
    id: root
    height: mainLayout.height
    width: mainLayout.width
    property var deviceType: 'No device'
    property var deviceModel: 'No device'
    property var deviceFirmware: 'No device'
    property var deviceID: 'No device'

    ColumnLayout {
        id: mainLayout
        RowLayout {
            Layout.preferredHeight: 50

            Image {
                id: pingIcon
                Layout.preferredWidth: 50
                source: "/imgs/ping.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            PingImage {
                id: pingName
                Layout.fillHeight: true
                Layout.fillWidth: true
                source: "/imgs/ping_name.svg"
            }

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            Image {
                id: brIcon
                Layout.preferredWidth: 50
                source: "/imgs/br_icon.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
        }
        RowLayout {
            ColumnLayout {
                Text {
                    z: 1
                    text: 'Version: <b>' + (GitTag == "" ? "No tags!" : GitTag)
                    color: Style.textColor
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    text: 'Repository: <b>' + createHyperLink(repository, repository.split('/')[4].toUpperCase())
                    color: Style.textColor
                    textFormat: Text.RichText
                    onLinkActivated: {
                        print('Open link ', link)
                        Qt.openUrlExternally(link)
                    }
                }
                Text {
                    z: 1
                    text: 'Git commit: <b>' + commitIdToLink(GitVersion)
                    color: Style.textColor
                    textFormat: Text.RichText
                    onLinkActivated: {
                        print('Open link ', link)
                        Qt.openUrlExternally(link)
                    }
                }
                Text {
                    z: 1
                    text: " From: " + GitVersionDate
                    color: Style.textColor
                    textFormat: Text.RichText
                }
            }

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            ColumnLayout {
                Text {
                    z: 1
                    // Add link to store device
                    text: 'Device: <b>' + deviceType
                    color: Style.textColor
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    // Add link to model in store
                    text: 'Model: <b>' + deviceModel
                    color: Style.textColor
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    text: 'Firmware Version: ' + deviceFirmware
                    color: Style.textColor
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    text: "ID: " + deviceID
                    color: Style.textColor
                    textFormat: Text.RichText
                }
            }

        }
        RowLayout {
            id: btLayout
            Layout.preferredHeight: 40

            PingImage {
                id: forumPost
                source: "/icons/chat_white.svg"
                Layout.fillHeight: true
                Layout.fillWidth: true

                MouseArea {
                    id: mouseAreaForumPost
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: tooltipForumPost.visible = true
                    onExited: tooltipForumPost.visible = false
                    onClicked: Qt.openUrlExternally("http://discuss.bluerobotics.com")
                }

                ToolTip {
                    id: tooltipForumPost
                    text: "Forum"
                }
            }

            PingImage {
                id: scrollLock
                Layout.fillHeight: true
                Layout.fillWidth: true
                source: log.scrollLockEnabled ? "/icons/lock_white.svg" : "/icons/unlock_white.svg"

                MouseArea {
                    id: mouseAreaScrollLock
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: toolTipScrollLock.visible = true
                    onExited: toolTipScrollLock.visible = false
                    onClicked: {
                        log.scrollLockEnabled = !log.scrollLockEnabled
                    }
                }

                ToolTip {
                    id: toolTipScrollLock
                    text: "Scroll Lock"
                }
            }

            PingImage {
                id: issue
                source: "/icons/report_white.svg"
                Layout.fillHeight: true
                Layout.fillWidth: true
                anchors.right: btLayout.right

                MouseArea {
                    id: mouseAreaIssue
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: tooltipIssue.visible = true
                    onExited: tooltipIssue.visible = false
                    onClicked: Qt.openUrlExternally(repository + "/issues")
                }

                ToolTip {
                    id: tooltipIssue
                    text: "Report issue"
                }
            }
        }

        Rectangle {
            height: 2
            Layout.fillWidth: true
            color: Style.textColor
        }

        PingLogger {
            id: log
            height: 300
            Layout.fillWidth: true
            Component.onCompleted: {
                print(height, width)
            }
        }

        GridLayout {
            columns: 4
            rowSpacing: 5
            columnSpacing: 5
            Repeater {
                Layout.fillWidth: true
                model: Logger.registeredCategory
                CheckBox {
                    text: modelData
                    checked: Logger.getCategory(modelData)
                    Layout.columnSpan: 1
                    onCheckedChanged: {
                        Logger.setCategory(modelData.toString(), checked)
                    }
                }
            }
        }
    }

    property var repository: GitUrl.split('.git')[0]

    function createHyperLink(link, text) {
        return "<a href=\"" + link + "\">" + text + "</a>"
    }

    function commitIdToLink(id) {
        var link = repository + "/commit/" + id
        return createHyperLink(link, id)
    }
}
