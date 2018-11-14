import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: displaySettings
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var waterfallItem

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
                        color: Material.primary
                    }

                    ComboBox {
                        id: distanceUnitsCb
                        model: SettingsManager.distanceUnitsModel
                        textRole: "name"
                        currentIndex: SettingsManager.distanceUnitsIndex
                        Layout.columnSpan: 4
                        Layout.fillWidth: true
                        onCurrentIndexChanged: SettingsManager.distanceUnitsIndex = currentIndex
                    }

                    Text {
                        text: "Plot Theme:"
                        color: Material.primary
                    }

                    ComboBox {
                        id: plotThemeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        model: waterfallItem.themes
                        onCurrentTextChanged: waterfallItem.theme = currentText
                    }

                    Text {
                        text: "Theme:"
                        color: Material.primary
                    }

                    ComboBox {
                        id: themeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        model: ["Dark", "Light"]
                        onCurrentTextChanged: StyleManager.isDark = !currentIndex
                    }

                    CheckBox {
                        id: replayChB
                        text: "Enable replay menu"
                        checked: SettingsManager.replayMenu
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckedChanged: SettingsManager.replayMenu = checked
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

                    CheckBox {
                        id: debugChB
                        text: "Debug mode"
                        checked: SettingsManager.debugMode
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckedChanged: SettingsManager.debugMode = checked
                    }
                }
            }
        }
    }

    Settings {
        property alias plotThemeIndex: plotThemeCB.currentIndex
        property alias smoothDataState: smoothDataChB.checkState
        property alias themeIndex: themeCB.currentIndex
        property alias waterfallAntialiasingData: antialiasingDataChB.checkState
    }

}
