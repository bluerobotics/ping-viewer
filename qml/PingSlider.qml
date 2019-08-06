import QtQuick 2.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3

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
        stepSize: 1
        wheelEnabled: true
        from: root.from
        to: root.to
        onValueChanged: root.value = value
    }

    Label {
        Layout.preferredWidth: 40
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
        onValueChanged: root.value = value
        stepSize: sliderControl.stepSize
        editable: true
    }
}
