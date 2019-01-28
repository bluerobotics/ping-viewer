import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    property alias title: text.text
    property alias text: textField.text
    property alias validator: textField.validator

    width: text.width + textField.width
    height: text.height

    signal editingFinished()


    Label {
        id: text
        color: Material.accent
        font.pixelSize: textField.text.font.pixelSize
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
            root.editingFinished()
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
