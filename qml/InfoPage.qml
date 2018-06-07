import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import Logger 1.0

Item {
    id: root
    property string deviceType: 'No device'
    property string deviceModel: 'No device'
    property string deviceFirmware: 'No device'
    property string deviceID: 'No device'

    onWidthChanged: updateGridLayout()
    onVisibleChanged: updateGridLayout()

    function updateGridLayout() {
        if(logCategoryGrid.maxItemSize*logCategoryGrid.columns > width) {
            if(logCategoryGrid.columns > 1) {
                logCategoryGrid.columns -= 1
            }
        } else if(logCategoryGrid.maxItemSize*(logCategoryGrid.columns + 1) < width) {
            logCategoryGrid.columns += 1
        }
    }

    ColumnLayout {
        id: mainLayout
        width: root.width
        height: root.height
        RowLayout {
            id: headerLayout
            width: mainLayout.width
            Layout.rightMargin: 10
            Layout.leftMargin: 10
            Layout.topMargin: 10

            Image {
                id: pingIcon
                Layout.preferredWidth: 50
                source: "/imgs/ping.png"
                fillMode: Image.PreserveAspectFit
                mipmap: true
                smooth: true
                antialiasing: true
            }

            Item {
                Layout.fillWidth: true
            }

            PingImage {
                id: pingName
                height: 50
                width: 150
                source: "/imgs/ping_name.png"
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: brIcon
                Layout.preferredWidth: 50
                source: "/imgs/br_icon.png"
                fillMode: Image.PreserveAspectFit
                mipmap: true
                smooth: true
                antialiasing: true
            }
        }

        RowLayout {
            id: textRow
            Layout.fillWidth: true
            Layout.rightMargin: 10
            Layout.leftMargin: 10

            ColumnLayout {
                id: textColumn
                Text {
                    z: 1
                    text: 'Version: <b>' + (GitTag === "" ? "No tags!" : GitTag)
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

            Item {
                Layout.fillWidth: true
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
            Layout.preferredHeight: 50
            width: root.width
            Layout.rightMargin: 10
            Layout.leftMargin: 10

            PingImage {
                id: forumPost
                source: "/icons/chat_white.svg"
                height: 50
                width: 50

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

            Item {
                Layout.fillWidth: true
            }

            PingImage {
                id: scrollLock
                source: log.scrollLockEnabled ? "/icons/lock_white.svg" : "/icons/unlock_white.svg"
                height: 50
                width: 50

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

            Item {
                Layout.fillWidth: true
            }

            PingImage {
                id: issue
                source: "/icons/report_white.svg"
                height: 50
                width: 50

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
            id: logLineDiv
            height: 2
            Layout.fillWidth: true
            color: Style.textColor
        }

        PingLogger {
            id: log
            height: 300
            Layout.fillWidth: true
            Layout.fillHeight: true
            Component.onCompleted: {
                print(height, width)
            }
        }
        GridLayout {
            id: logCategoryGrid
            rowSpacing: 5
            columnSpacing: 5
            columns: 5
            Layout.fillWidth: true
            property var maxItemSize: 100
            Repeater {
                model: Logger.registeredCategory
                CheckBox {
                    text: modelData
                    checked: Logger.getCategory(modelData)
                    Layout.columnSpan: 1
                    onCheckedChanged: {
                        Logger.setCategory(modelData.toString(), checked)
                    }
                }
                onModelChanged: {
                    for(var i in model) {
                        var size = model[i].length*font.font.pixelSize
                        logCategoryGrid.maxItemSize = logCategoryGrid.maxItemSize > size ? logCategoryGrid.maxItemSize : size
                        print(model[i], model[i].length*font.font.pixelSize)
                    }
                }
            }
        }
    }

    // Used to get text size
    Text {
        id: font
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
