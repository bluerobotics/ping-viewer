import QtQuick 2.7
import QtQuick.Controls 2.2

import Logger 1.0

Item {
    id: root
    property var content: "<b><i>PingViewer<\b><\i> <br>"

    Connections {
        target: Logger
        onLogTextChanged: {
            content = textEdit.text + Logger.consumeLogText()
        }
    }
    ScrollView {
        id: view
        anchors.fill: parent
        clip: true

        Flickable {
            id: flickable

            TextEdit {
                id : textEdit
                readOnly: true
                selectByMouse: true
                textFormat: TextEdit.RichText
                text: content

                onTextChanged: {
                    // scroll when text is bigger than height
                    if(height > flickable.height) {
                        flickable.contentY = height - flickable.height
                    }
                }
            }
        }
    }
}