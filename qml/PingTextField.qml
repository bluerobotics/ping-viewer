import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    property alias title: label.text
    property string text: ""
    property alias validator: textField.validator

    width: label.width + textField.width
    height: label.height

    // Emited when edition if finished with enter/return key or when TextField loses focus
    signal editingFinished()

    // This avoid connection interactions while user input is on
    onTextChanged: {
        if(textField.focus) {
            return
        }

        textField.text = text
    }


    Label {
        id: label
        color: Material.accent
    }

    TextField {
        id: textField

        anchors.left: label.right
        anchors.baseline: label.baseline
        anchors.leftMargin: 5
        selectByMouse: true

        // editingFinished() is only emitted when TextField has focus
        // That's why we are using accepted()
        onAccepted: {
            focus = false
            mainPage.forceActiveFocus()
            // It's necessary to update the input variable
            root.text = textField.text
            root.editingFinished()
        }

        onActiveFocusChanged: {
            // TODO: We should update the user about wrong inputs here
            // This allow us to correct not valid inputs to the last valid value
            if(!acceptableInput) {
                textField.text = root.text
            }

            if (activeFocus) {
                selectAll()
            } else {
                deselect()
            }
        }
    }
}
