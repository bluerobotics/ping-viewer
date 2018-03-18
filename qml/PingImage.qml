import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    id: root

    property var source: undefined

    Image {
        id: image
        source: root.source
        mipmap: true
        smooth: true
        visible: false
        antialiasing: true
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent
    }

    ColorOverlay {
        id: colorOverlay
        anchors.fill: image
        source: image
        color: Style.iconColor
    }
}