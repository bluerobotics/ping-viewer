import QtQuick 2.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3

RowLayout {
    id: root

    property string text
    property string value
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
        wheelEnabled: true
        value: root.value
    }

    Label {
        Layout.fillWidth: true

        Layout.preferredWidth: 40
        text: root.value
    }
}
