import QtQuick 2.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3

RowLayout {
    id: root

    property var text
    property alias value : sliderControl.value
    property alias control: sliderControl

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
    }

    Label {
        Layout.fillWidth: true

        Layout.preferredWidth: 40
        text: root.value
    }
}
