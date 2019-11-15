import QtQuick 2.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.1

RowLayout {
    id: root

    property var text
    property double value: 0
    property double from: 0
    property double to: 0
    property double ticksNumber: 6
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

        background:
            Rectangle {
            x: sliderControl.leftPadding
            y: sliderControl.topPadding + sliderControl.availableHeight/2 - height/2
            implicitWidth: sliderControl.availableWidth
            implicitHeight: 1
            width: sliderControl.availableWidth
            height: implicitHeight
            color: Material.accent

            Repeater {
                id: internalTicks
                model: ticksNumber + 1

                delegate: Rectangle {
                    id: tick
                    width: 1
                    height: parent.implicitHeight*6
                    x: index*(sliderControl.availableWidth - sliderControl.handle.width)/root.ticksNumber + sliderControl.handle.width/2
                    y: parent.implicitHeight
                    color: Material.accent

                    Text {
                        anchors.top: tick.bottom
                        id: textDelegate
                        text: ((index/root.ticksNumber)*(sliderControl.to - sliderControl.from) + sliderControl.from).toFixed(0)
                        color: Material.accent
                        x: -width/2
                    }
                }
            }
        }
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
