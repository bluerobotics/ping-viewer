import QtQuick 2.7
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0

Item {
    id: root

    property alias source: image.source
    property bool selected: false

    Image {
        id: image
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
        color: selected ? Material.foreground : Material.primary
    }
}
