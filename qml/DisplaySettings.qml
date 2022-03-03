import DeviceManager 1.0
import Qt5Compat.GraphicalEffects
import QtQml 2.2
import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import SettingsManager 1.0
import StyleManager 1.0

ColumnLayout {
    id: settingsLayout

    RowLayout {
        PingGroupBox {
            id: displayGroup

            title: "Display"
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

                    Layout.columnSpan: 4
                    Layout.fillWidth: true
                    currentIndex: !SettingsManager.darkTheme
                    model: ["Dark", "Light"]
                    onCurrentTextChanged: StyleManager.isDark = !currentIndex
                }

                CheckBox {
                    id: alwaysOnTopCB

                    text: "Enable Always-On-Top mode"
                    checked: SettingsManager.alwaysOnTop
                    Layout.columnSpan: 5
                    Layout.fillWidth: true
                    onCheckedChanged: {
                        if (checked)
                            window.flags |= Qt.WindowStaysOnTopHint;
                        else
                            window.flags &= ~Qt.WindowStaysOnTopHint;
                        SettingsManager.alwaysOnTop = checked;
                    }
                }

                Label {
                    text: "Opacity:"
                }

                ComboBox {
                    id: opacityCB

                    Layout.columnSpan: 4
                    Layout.fillWidth: true
                    currentIndex: SettingsManager.applicationOpacityIndex
                    model: [100, 90, 80, 70]
                    displayText: currentText + "%"
                    onCurrentTextChanged: {
                        window.opacity = currentText / 100;
                        SettingsManager.applicationOpacityIndex = currentIndex;
                    }
                }

                CheckBox {
                    id: replayChB

                    text: "Enable replay menu"
                    checked: SettingsManager.replayMenu
                    Layout.columnSpan: 5
                    Layout.fillWidth: true
                    onCheckedChanged: SettingsManager.replayMenu = checked
                }

                CheckBox {
                    id: realTimeReplayChB

                    text: "Enable real-time replay"
                    checked: SettingsManager.realTimeReplay
                    visible: SettingsManager.replayMenu
                    Layout.columnSpan: 5
                    Layout.fillWidth: true
                    onCheckedChanged: SettingsManager.realTimeReplay = checked
                }

                Loader {
                    sourceComponent: DeviceManager.primarySensor ? DeviceManager.primarySensor.sensorVisualizer().displaySettings : null
                    Layout.columnSpan: 5
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                CheckBox {
                    id: debugChB

                    text: "Debug mode"
                    checked: SettingsManager.debugMode
                    Layout.columnSpan: 5
                    Layout.fillWidth: true
                    onCheckedChanged: SettingsManager.debugMode = checked
                }

            }

        }

    }

}
