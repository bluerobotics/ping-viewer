import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: root

    property alias title: label.text
    property string text: ""
    property alias validator: textField.validator

    // Emited when edition if finished with enter/return key or when TextField loses focus
    signal editingFinished()

    Layout.minimumWidth: label.width + textField.width
    Layout.minimumHeight: label.height
    // This avoid connection interactions while user input is on
    onTextChanged: {
        if (textField.focus)
            return ;

        textField.text = text;
    }

    TextMetrics {
        id: textMetrics

        property var enabled: validator === undefined && !(validator instanceof RegExpValidator)

        font: textField.font
        text: enabled ? validator.top : ""
    }

    Label {
        id: label

        color: Material.accent
    }

    TextField {
        id: textField

        property var inputWidth: textMetrics.enabled ? textMetrics.width : contentWidth

        anchors.left: label.right
        anchors.baseline: label.baseline
        anchors.leftMargin: 5
        selectByMouse: true
        // Change input box style
        rightPadding: leftPadding
        topPadding: 0
        bottomPadding: 0
        horizontalAlignment: TextInput.AlignRight
        background.implicitWidth: inputWidth + 2 * leftPadding
        background.implicitHeight: contentHeight * 1.1
        // We can't access background rectangle item before the item is complete
        // It's necessary to wait for acceptableInput change
        onAcceptableInputChanged: {
            background.border.color = acceptableInput ? "green" : "red";
        }
        // editingFinished() is only emitted when TextField has focus
        // That's why we are using accepted()
        onAccepted: {
            focus = false;
            root.parent.forceActiveFocus();
            // It's necessary to update the input variable
            root.text = textField.text;
            root.editingFinished();
        }
        onActiveFocusChanged: {
            // TODO: We should update the user about wrong inputs here
            // This allow us to correct not valid inputs to the last valid value
            if (!acceptableInput)
                textField.text = root.text;
            else
                root.text = textField.text;
            if (activeFocus) {
                selectAll();
            } else {
                deselect();
                root.editingFinished();
            }
        }
    }

}
