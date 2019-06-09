import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: displaySettings
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width

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

                    Label {
                        text: "Units:"
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

                    Label {
                        text: "Theme:"
                    }

                    ComboBox {
                        id: themeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        currentIndex: !SettingsManager.darkTheme
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

                    Loader {
                        sourceComponent: sensorVisualizerLoader.item.displaySettings
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
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
}
