import QtQuick 2.0
import QtQuick.Controls 2.2

TextField {
    onEditingFinished: {
        focus = false
        mainPage.forceActiveFocus()
    }
}
