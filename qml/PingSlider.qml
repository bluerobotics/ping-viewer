import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.3

RowLayout {
    id: root

    property var text
    property double value: 0
    property double from: 0
    property double to: model ? model.length - 1 : 0
    property double ticksNumber: model ? to : 6
    property alias control: sliderControl
    property var model: undefined
    property var modelValue: undefined
    property alias modelIndex: root.value

    // Do the necessary logic if a model is used
    onModelValueChanged: {
        if (model == undefined || model[value] == Math.round(modelValue))
            return ;

        // Get something that is closer to an item in our model
        // E.g: New value is 0.99, select the value in the model that is closer [1]
        var closeItem = {
            "index": 0,
            "distance": Number.MAX_SAFE_INTEGER
        };
        for (var i in model) {
            if (typeof (closeItem.value) != "number") {
                if (model[i] == Math.round(modelValue)) {
                    closeItem.index = i;
                    break;
                }
            } else {
                if (Math.abs(model[i] - modelValue) < closeItem.distance) {
                    closeItem.index = i;
                    closeItem.distance = Math.abs(model[i] - modelValue);
                }
            }
        }
        // Update the new value
        root.value = closeItem.index;
    }

    Binding {
        target: spinBox
        property: "value"
        value: root.value
    }

    Binding {
        target: sliderControl
        property: "value"
        value: root.value
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

        background: Rectangle {
            x: sliderControl.leftPadding
            y: sliderControl.topPadding + sliderControl.availableHeight / 2 - height / 2
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
                    height: parent.implicitHeight * 6
                    x: index * (sliderControl.availableWidth - sliderControl.handle.width) / root.ticksNumber + sliderControl.handle.width / 2
                    y: parent.implicitHeight
                    color: Material.accent

                    Text {
                        id: tickText

                        anchors.top: tick.bottom
                        text: root.model ? root.model[index] : ((index / root.ticksNumber) * (sliderControl.to - sliderControl.from) + sliderControl.from).toFixed(0)
                        color: Material.accent
                        x: -width / 2
                    }

                }

            }

        }

    }

    Label {
        Layout.preferredWidth: 80
        text: root.model !== undefined ? root.model[root.value] : ""
        visible: !spinBox.visible
    }

    SpinBox {
        id: spinBox

        Layout.minimumWidth: 150
        Layout.maximumWidth: 150
        visible: model === undefined
        from: root.from
        to: root.to
        stepSize: sliderControl.stepSize
        editable: true
        enabled: visible
    }

    Binding on modelValue {
        when: root.value !== undefined && model !== undefined
        value: model[root.value]
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
