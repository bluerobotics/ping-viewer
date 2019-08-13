import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import Logger 1.0
import SettingsManager 1.0
import StyleManager 1.0
import Util 1.0

Item {
    id: root
    property string deviceType: 'No device'
    property string deviceRevision: 'No device'
    property string deviceFirmware: 'No device'
    property string deviceID: 'No device'

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
                source: "/imgs/ping.svg"
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
                source: "/imgs/ping_name.svg"
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: brIcon
                Layout.preferredWidth: 50
                source: "/imgs/br_icon.svg"
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
                    color: Material.primary
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    text: 'Repository: <b>' + createHyperLink(repository, repository.split('/')[4].toUpperCase())
                    color: Material.primary
                    textFormat: Text.RichText
                    onLinkActivated: {
                        print('Open link ', link)
                        Qt.openUrlExternally(link)
                    }
                }
                Text {
                    z: 1
                    text: 'Git commit: <b>' + commitIdToLink(GitVersion)
                    color: Material.primary
                    textFormat: Text.RichText
                    onLinkActivated: {
                        print('Open link ', link)
                        Qt.openUrlExternally(link)
                    }
                }
                Text {
                    z: 1
                    text: " From: " + GitVersionDate
                    color: Material.primary
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
                    color: Material.primary
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    // Add link to model in store
                    text: 'Model: <b>' + deviceRevision
                    color: Material.primary
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    text: 'Firmware Version: ' + deviceFirmware
                    color: Material.primary
                    textFormat: Text.RichText
                }
                Text {
                    z: 1
                    text: "ID: " + deviceID
                    color: Material.primary
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
                id: scrollLock
                source: log.scrollLockEnabled ? StyleManager.lockIcon() : StyleManager.unlockIcon()
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
                id: forumPost
                source: StyleManager.chatIcon()
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
                id: issue
                source: StyleManager.reportIcon()
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

            Item {
                Layout.fillWidth: true
            }

            PingImage {
                id: resetSettings
                source: StyleManager.resetIcon()
                height: 50
                width: 50

                MouseArea {
                    id: mouseAreaResetSettings
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: tooltipResetSettings.visible = true
                    onExited: tooltipResetSettings.visible = false
                    onClicked: {
                        print("Reset settings, interface need restart!")
                        popup.open()
                    }
                }

                ToolTip {
                    id: tooltipResetSettings
                    text: "Reset settings"
                }
            }
        }

        Rectangle {
            id: logLineDiv
            height: 2
            Layout.fillWidth: true
            color: Material.primary
        }

        RowLayout {
            PingLogger {
                id: log
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.fillHeight: true
                Label {
                    text: "Categories"
                    Layout.minimumWidth: parent.width
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                }
                ListView {
                    id: listView
                    clip: true
                    Layout.fillHeight: true
                    ScrollBar.vertical: ScrollBar {}

                    model: Logger.registeredCategory
                    delegate: CheckBox {
                        text: modelData
                        checked: SettingsManager.enabledCategories & Logger.getCategoryIndex(modelData.toString())
                        Layout.fillWidth: true
                        onCheckedChanged: {
                            if(checked) {
                                log.enabledCategories |= Logger.getCategoryIndex(modelData.toString())
                            } else {
                                log.enabledCategories ^= Logger.getCategoryIndex(modelData.toString())
                            }
                        }
                    }
                    onVisibleChanged: {
                        for(var i in flickableChildren) {
                            Layout.minimumWidth = Math.max(Layout.minimumWidth, flickableChildren[i].width)
                        }
                    }
                }
            }
        }
    }

    PingPopup {
        id: popup

        PingGroupBox {
            id: resetSettingsGroup
            title: "Reset settings"
            enabled: true
            anchors.fill: parent

            GridLayout {
                Layout.margins: 15
                columns: 5
                rowSpacing: 10
                columnSpacing: 10
                anchors.fill: parent

                Text {
                    text: "This action will reset the appication settings and do a restart of the program, do you want to proceed ?"
                    clip: true
                    Layout.margins: 25
                    Layout.columnSpan: 5
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: Material.primary
                    wrapMode: Text.WordWrap
                }

                PingButton {
                    text: "Abort"
                    Layout.fillWidth: true
                    Layout.columnSpan:  2
                    onClicked: {
                        print("Abort settings reset")
                        popup.close()
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.columnSpan: 1
                }

                PingButton {
                    text: "Yes"
                    Layout.fillWidth: true
                    Layout.columnSpan:  2
                    onClicked: {
                        print("Reset settings.")
                        SettingsManager.reset = true
                        Util.restartApplication()
                    }
                }

                Item {
                    height: 10
                    Layout.columnSpan: 5
                    Layout.fillWidth: true
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
