import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    property alias title: text.text
    property var text: ""
    property alias validator: textField.validator

    width: text.width + textField.width
    height: text.height

    // Emited when edition if finished with enter/return key or when TextField loses focus
    signal editingFinished(var input)

    // This avoid connection interactions while user input is on
    onTextChanged: {
        if(textField.focus) {
            return
        }

        textField.text = root.text
    }


    Label {
        id: text
        color: Material.accent
    }

    TextField {
        id: textField

        anchors.left: text.right
        anchors.baseline: text.baseline
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
