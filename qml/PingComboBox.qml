import QtQml 2.2
import QtQuick.Controls 2.2

ComboBox {
    id: root
    currentIndex: -1

    property var setting

    onCurrentTextChanged: {
        // Empty string should not pass
        // ComboBox start with it and emit a signal for that
        if(!currentText){
            return
        }
        setting = currentText
    }

    Component.onCompleted: {
        // Load the correct model index
        for(var i in model) {
            if(model[i] == setting) {
                currentIndex = i
                return
            }
        }
    }
}