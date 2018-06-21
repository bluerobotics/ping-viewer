import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Item {
    id: displaySettings
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var waterfallItem
    property alias replayItem: replayChB.checked

    ColumnLayout {
        id: settingsLayout
        RowLayout {
            GroupBox {
                id: displayGroup
                title: "Display"
                // Hack
                label.x: width/2 - label.contentWidth/2
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: 5
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Units:"
                        color: Style.textColor
                    }

                    ComboBox {
                        displayText: "Meters"
                        enabled: false
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Plot Theme:"
                        color: Style.textColor
                    }

                    ComboBox {
                        id: plotThemeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        model: waterfallItem.themes
                        onCurrentTextChanged: {
                            waterfallItem.theme = currentText
                        }
                    }

                    Text {
                        text: "Theme:"
                        color: Style.textColor
                    }

                    ComboBox {
                        id: themeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        model: ["Dark", "Light"]
                        onCurrentTextChanged: {
                            if (currentIndex) {
                                Style.useLightStyle()
                            } else {
                                Style.useDarkStyle()
                            }
                        }
                    }

                    CheckBox {
                        id: replayChB
                        text: "Enable replay menu"
                        checked: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        id: smoothDataChB
                        text: "Smooth Data"
                        checked: true
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckStateChanged: {
                            waterfallItem.smooth = checkState
                        }
                    }

                    CheckBox {
                        id: antialiasingDataChB
                        text: "Antialiasing"
                        checked: true
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckStateChanged: {
                            waterfallItem.antialiasing = checkState
                        }
                    }
                }
            }
        }
    }

    Settings {
        property alias plotThemeIndex: plotThemeCB.currentIndex
        property alias replayItemChecked: replayChB.checked
        property alias smoothDataState: smoothDataChB.checkState
        property alias themeIndex: themeCB.currentIndex
    }

}
