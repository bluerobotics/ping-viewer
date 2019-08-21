import QtQuick 2.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.1

import SliderRuler 1.0

RowLayout {
    id: root

    property var text
    property double value: 0
    property double from: 0
    property double to: 0
    property alias control: sliderControl
    property var valueText: null

    onValueChanged: {
        spinBox.value = value
        sliderControl.value = value
    }

    Label {
        horizontalAlignment: Text.AlignRight
        wrapMode: Text.Wrap
        Layout.fillWidth: true
        Layout.preferredWidth: 120
        text: root.text + ": "
    }

    Slider {
        id: sliderControl
        Layout.fillWidth: true
        Layout.minimumWidth: 300
        Layout.minimumHeight: 50
        stepSize: 1
        wheelEnabled: true
        from: root.from
        to: root.to
        enabled: visible
        background: SliderRuler {
            id: ticks
            anchors.fill: parent
            color: Material.accent
            from: parent.from
            to: parent.to
        }

        handle.x: ticks ? ticks.beginLine.x + sliderControl.visualPosition
                          * (ticks.endLine.x - ticks.beginLine.x) - handle.width / 2
                        : sliderControl.leftPadding + sliderControl.visualPosition * (sliderControl.availableWidth - handle.width)
    }

    Label {
        Layout.preferredWidth: 80
        text: valueText !== null ? valueText : ""
        visible: valueText !== null
    }

    SpinBox {
        id: spinBox
        Layout.minimumWidth: 150
        Layout.maximumWidth: 150
        visible: valueText === null
        from: root.from
        to: root.to
        stepSize: sliderControl.stepSize
        editable: true
        enabled: visible
    }

    Binding on value {
        when: sliderControl.pressed
        value: sliderControl.value
    }

    Binding on value {
        when: sliderControl.moved
        value: sliderControl.value
    }

    Binding on value {
        when: spinBox.up.pressed
        value: spinBox.value
    }

    Binding on value {
        when: spinBox.down.pressed
        value: spinBox.value
    }

    Binding on value {
        when: spinBox.focus
        value: spinBox.value
    }
}
