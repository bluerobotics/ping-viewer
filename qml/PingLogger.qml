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
            if(textEdit.lineCount > 100) {
                var lines = textEdit.text.split('\n');
                lines.splice(0, textEdit.lineCount - 100);
                content = lines.join('\n');
            }
        }
    }
    ScrollView {
        id: view
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.interactive: true

        Flickable {
            id: flickable

            TextEdit {
                id : textEdit
                readOnly: true
                selectByMouse: true
                textFormat: TextEdit.RichText
                renderType: TextEdit.NativeRendering
                wrapMode: TextEdit.WrapAnywhere
                text: content
                color: "linen"

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