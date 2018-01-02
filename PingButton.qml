import QtQuick 2.1
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.0

Item {
    id: pingButton

    property alias text: buttonText.text

    signal clicked

    implicitWidth: buttonText.implicitWidth + 5
    implicitHeight: buttonText.implicitHeight + 10

    Button {
        id: buttonText
        width: parent.width
        height: parent.height

        onClicked: pingButton.clicked()
    }
}
