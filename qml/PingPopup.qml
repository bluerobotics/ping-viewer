import QtQuick 2.7
import QtQuick.Controls 2.2

Popup {
    id: root

    // Center popup
    x: (mainPage.width - width)/2 - parent.mapToItem(mainPage, 0, 0).x
    y: (mainPage.height - height)/2 - parent.mapToItem(mainPage, 0, 0).y

    // Get focus and any event
    modal: true
    focus: true

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
}