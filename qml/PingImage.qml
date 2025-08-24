import Qt5Compat.GraphicalEffects
import QtQuick 2.15
import QtQuick.Controls.Material 2.2

Item {
    id: root

    property alias source: image.source
    property double angle: 0
    property bool selected: false
    property alias color: colorOverlay.color

    Image {
        id: image

        mipmap: true
        smooth: true
        visible: false
        antialiasing: true
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent
        // This resize the SVG in a correct way
        sourceSize.width: parent.width
        sourceSize.height: parent.height
    }

    ColorOverlay {
        id: colorOverlay

        anchors.fill: image
        source: image
        color: !root.enabled ? Material.background : selected ? Material.foreground : Material.primary

        transform: Rotation {
            origin.x: width / 2
            origin.y: height / 2
            angle: root.angle

            axis {
                x: 0
                y: 0
                z: 1
            }

        }

    }

}
