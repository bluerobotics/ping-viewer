import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0 as QtLabs
import Qt.labs.settings 1.0

Item {
    id: csvUpdate
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var ping: null

    Connections {
        target: ping
        onCsvProgress: {
            csvProgress.indeterminate = false;
            csvProgress.value = progress;
        }
        onCsvComplete: {
            console.log("CSV complete!")
            csvProgress.value = 0.0;
            csvProgress.indeterminate = true;
        }
    }

    ColumnLayout {
        id: settingsLayout
        width: 600
        RowLayout {
            GroupBox {
                id: csvGroup
                title: "Bin to CSV"
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
                        id: logInputLabel
                        text: "Log file:"
                        color: Style.textColor
                    }

                    PingTextField {
                        id: logFileName
                        enabled: false
                        Layout.columnSpan:  8
                        Layout.fillWidth: true
                        text: fileDialog.fileName
                    }

                    PingButton {
                        text: "Browse.."
                        enabled: csvProgress.indeterminate
                        onClicked: {
                            fileDialog.visible = true
                        }
                    }

                    Text {
                        id: csvOutputLabel
                        text: "Output dir:"
                        color: Style.textColor
                    }

                    PingTextField {
                        enabled: false
                        Layout.columnSpan:  8
                        Layout.fillWidth: true
                        text: dirDialog.folderName
                    }

                    PingButton {
                        text: "Browse.."
                        enabled: csvProgress.indeterminate
                        onClicked: {
                            dirDialog.visible = true
                        }
                    }

                    PingButton {
                        text: "Convert"
                        Layout.columnSpan:  10
                        Layout.fillWidth: true
                        enabled: fileDialog.fileUrl.toString().length

                        onClicked: {
                            print('convert file!')
                            ping.logToCSV(fileDialog.fileUrl, dirDialog.folder)
                        }
                    }

                    ProgressBar {
                        id: csvProgress
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
        title: "Please choose log a file"
        folder: shortcuts.home
        visible: false
        property string fileName: ""
        nameFilters: ["Log files (*.bin)"]
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            var sizeToRemove = fileDialog.folder.toString().length - fileDialog.fileUrl.toString().length + 1
            fileName = fileDialog.fileUrl.toString().slice(sizeToRemove)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    QtLabs.FolderDialog {
        id: dirDialog
        title: "Please choose output directory"
        folder: shortcuts.home
        visible: false
        property string folderName: ""
        onAccepted: {
            console.log("You chose: " + dirDialog.folder)
            var splitted = dirDialog.folder.toString().split('/')
            folderName = splitted[splitted.length - 1]
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
