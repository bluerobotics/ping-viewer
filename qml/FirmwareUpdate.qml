import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Item {
    id: root
    visible: false
    property var icon: "/icons/chip_white.svg"
    property var ping: null

    Layout.fillHeight: true
    Layout.fillWidth: true

    Connections {
        target: ping
        onFlashProgress: {
            flashProgress.indeterminate = false;
            flashProgress.value = progress;
        }
        onFlashComplete: {
            console.log("flash complete!")
            flashProgress.value = 0.0;
            flashProgress.indeterminate = true;
        }
    }

    ColumnLayout {
        id: settingsLayout
        width: parent.width - 20
        height: parent.height - 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        RowLayout {
            GroupBox {
                id: firmwareGroup
                title: "Firmware Update"
                enabled: true
                // Hack
                label.x: width/2 - label.contentWidth/2
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: 10
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Current Firmware:"
                        color: Style.textColor
                    }

                    PingTextField {
                        enabled: false
                        Layout.columnSpan:  8
                        Layout.fillWidth: true
                        text: ping.fw_version_major + "." + ping.fw_version_minor
                    }

                    CheckBox {
                        id: bootLoaderCB
                        text: "Send reset"
                        checked: true
                    }

                    Text {
                        id: firmwareLabel
                        text: "Firmware File:"
                        color: Style.textColor
                    }

                    PingTextField {
                        id: firmwareFileName
                        enabled: false
                        Layout.columnSpan:  8
                        Layout.fillWidth: true
                        text: fileDialog.fileName
                    }

                    PingButton {
                        text: "Browse.."
                        enabled: flashProgress.indeterminate
                        onClicked: {
                            fileDialog.visible = true
                        }
                    }

                    PingButton {
                        text: "Firmware Update"
                        Layout.columnSpan:  10
                        Layout.fillWidth: true
                        enabled: fileDialog.fileUrl.toString().length && flashProgress.indeterminate

                        onClicked: {
                            ping.firmwareUpdate(fileDialog.fileUrl, bootLoaderCB)
                        }
                    }

                    ProgressBar {
                        id: flashProgress
                        indeterminate: true
                        Layout.columnSpan: 10
                        Layout.fillWidth: true
                        value: 0.0
                        from: 0.0
                        to: 100.0
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        visible: false
        property string fileName: ""
        nameFilters: ["Hex Files (*.hex)"]
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            var sizeToRemove = fileDialog.folder.toString().length - fileDialog.fileUrl.toString().length + 1
            fileName = fileDialog.fileUrl.toString().slice(sizeToRemove)
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
