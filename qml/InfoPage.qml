import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Logger 1.0

Item {
    id: root
    height: mainLayout.height
    width: mainLayout.width

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

            Image {
                id: pingName
                Layout.preferredHeight: 50
                source: "/imgs/ping_name.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true
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
        Text {
            z: 1
            text: 'Version: <b>' + (GitTag == "" ? "No tags!" : GitTag)
            color: 'linen'
            textFormat: Text.RichText
        }
        Text {
            z: 1
            text: 'Repository: <b>' + createHyperLink(repository, repository.split('/')[4].toUpperCase())
            color: 'linen'
            textFormat: Text.RichText
            onLinkActivated: {
                print('Open link ', link)
                Qt.openUrlExternally(link)
            }
        }
        Text {
            z: 1
            text: 'Git commit: <b>' + commitIdToLink(GitVersion)
            color: 'linen'
            textFormat: Text.RichText
            onLinkActivated: {
                print('Open link ', link)
                Qt.openUrlExternally(link)
            }
        }
        Text {
            z: 1
            text: " From: " + GitVersionDate
            color: 'linen'
            textFormat: Text.RichText
        }
        RowLayout {
            id: btLayout

            Image {
                id: forumPost
                source: "/icons/chat_white.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true

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

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            Image {
                id: issue
                anchors.right: btLayout.right
                source: "/icons/report_white.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true

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
            color: "linen"
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
