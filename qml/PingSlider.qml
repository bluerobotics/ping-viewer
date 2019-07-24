import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3

RowLayout {
    id: root

    property string text
    property string value
    property alias control: sliderControl

    Label {
        id: titleLabel
        wrapMode: Text.Wrap
        Layout.preferredWidth: titleTextMetrics.width
        horizontalAlignment: Text.AlignLeft
        text: root.text + ": "

        TextMetrics {
            id: titleTextMetrics
            font: titleLabel.font
            text: titleLabel.text
        }
    }

    Slider {
        id: sliderControl
        Layout.fillWidth: true
        wheelEnabled: true
        value: root.value
    }

    Label {
        id: valueLabel
        // Set the width to be the max value of the slider
        // allowing a fixed width for the entire range of valid values
        Layout.preferredWidth: valueTextMetrics.width
        horizontalAlignment: Text.AlignRight
        text: root.value

        TextMetrics {
            id: valueTextMetrics
            font: valueLabel.font
            text: sliderControl.to
        }
    }
}
