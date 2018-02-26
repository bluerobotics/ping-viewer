import QtQuick 2.7
import QtQuick.Controls 2.2

import Logger 1.0

Item {
    id: root

    property var scrollLockEnabled: false

    ListView {
        property bool loadComplete: false

        Component.onCompleted: {
            loadComplete = true
        }

        id: logView
        anchors.fill: parent
        contentWidth: 1500  // TODO auto-fit
        clip: true
        flickableDirection: Flickable.HorizontalAndVerticalFlick
        model: Logger.logModel
        delegate: Text { text: display }
        ScrollBar.horizontal: ScrollBar { }
        ScrollBar.vertical: ScrollBar { }

        Connections {
            target: Logger.logModel
            onDataChanged: {
                if (scrollLockEnabled && logView.loadComplete) {
                    logView.positionViewAtEnd();
                }
            }
        }
    }
}
