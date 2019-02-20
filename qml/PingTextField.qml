import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    property alias title: label.text
    property var text: ""
    property alias validator: textField.validator

    width: label.width + textField.width
    height: label.height

    // Emited when edition if finished with enter/return key or when TextField loses focus
    signal editingFinished(var input)

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

        onEditingFinished: {
            focus = false
            mainPage.forceActiveFocus()
            root.editingFinished(textField.text)
        }

        onActiveFocusChanged: {
            if (activeFocus) {
                selectAll()
            } else {
                deselect()
            }
        }
    }
}
