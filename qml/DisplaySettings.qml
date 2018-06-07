import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Item {
    id: displaySettings
    visible: false
    // To be used by Configuration Page
    property var icon: "/icons/sun_white.svg"
    property var waterfallItem
    property bool replayItem: replayChB.checked

    Layout.fillHeight: true
    Layout.fillWidth: true

    ColumnLayout {
        id: settingsLayout
        width: parent.width - 20
        height: parent.height - 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        RowLayout {
            GroupBox {
                id: displayGroup
                title: "Interface"
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
                        id: enableReadoutChb
                        text: "Enable readout widget"
                        enabled: false
                        checked: true
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        id: replayChB
                        text: "Enable replay menu"
                        checked: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        id: debugWidgetChB
                        text: "Enable sensor debug widget"
                        checked: true
                        enabled: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        text: "Enable waterfall"
                        checked: true
                        enabled: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        text: "Enable chart"
                        checked: true
                        enabled: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }
                }
            }

            GroupBox {
                title: "Data Visualization"
                // Hack
                label.x: width/2 - label.contentWidth/2
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: 5
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Plot Theme:"
                        textFormat: Text.RichText
                        color: Style.textColor
                    }

                    ComboBox {
                        id: plotThemeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        model: waterfallItem.themes
                        onCurrentTextChanged: {
                            waterfallItem.theme = currentText
                        }
                    }

                    CheckBox {
                        id: smoothDataChB
                        text: "Enable anti-aliasing"
                        checked: true
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckStateChanged: {
                            // TODO: uncouple filter and anti-aliasing
                            waterfallItem.smooth = checkState
                            filterChB.checked = checkState
                        }
                    }

                    CheckBox {
                        id: filterChB
                        text: "Enable filter"
                        checked: true
                        enabled: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Filter type:"
                        textFormat: Text.RichText
                        color: Style.textColor
                    }

                    ComboBox {
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        enabled: false
                        model: ["low-pass", "high-pass", "EKF", "generic digital"]
                    }

                    Text {
                        text: "Smoothing factor:"
                        textFormat: Text.RichText
                        color: Style.textColor
                    }

                    //smoothing factor
                    SpinBox {
                        id: spinbox
                        editable: true
                        from: 1
                        value: 80
                        to: 100
                        stepSize: 1
                        Layout.columnSpan:  4
                        enabled: false

                        property int decimals: 2
                        property real realValue: value / 100

                        validator: DoubleValidator {
                            bottom: Math.min(spinbox.from, spinbox.to)
                            top:  Math.max(spinbox.from, spinbox.to)
                        }

                        textFromValue: function(value, locale) {
                            return Number(value / 100).toLocaleString(locale, 'f', spinbox.decimals)
                        }

                        valueFromText: function(text, locale) {
                            return Math.max(Math.min(Number.fromLocaleString(locale, text) * 100, from), to)
                        }
                    }

                    Repeater {
                        model: 2
                        Rectangle {
                            color: "transparent"
                            Layout.columnSpan:  5
                            Layout.fillWidth: true
                            height: plotThemeCB.height
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
