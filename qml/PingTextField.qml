import QtQuick 2.0
import QtQuick.Controls 2.2

TextField {

    selectByMouse: true

    onEditingFinished: {
        focus = false
        mainPage.forceActiveFocus()
    }

    onActiveFocusChanged: {
        if (activeFocus) {
            selectAll()
        } else {
            deselect()
        }
    }
}
