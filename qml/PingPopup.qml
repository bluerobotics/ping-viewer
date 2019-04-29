import QtQuick 2.7
import QtQuick.Controls 2.2

Popup {
    id: root

    anchors.centerIn: parent

    // Get focus and any event
    modal: true
    focus: true

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
}