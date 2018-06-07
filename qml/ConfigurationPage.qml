//Based on TabWidgets source code
import QtQuick 2.3

Item {
    id: tabWidget
    anchors.fill: parent

    default property alias content: stack.children
    property int current: 0

    property var colorSelected: Style.isDark ? Qt.rgba(0,0,0,0.75) : Qt.rgba(1,1,1,0.75)
    property var colorUnselected: Style.isDark ? Qt.rgba(0,0,0,0.5) : Qt.rgba(1,1,1,0.5)

    onCurrentChanged: setOpacities()
    Component.onCompleted: setOpacities()

    function setOpacities() {
        for (var i = 0; i < content.length; ++i) {
            content[i].opacity = (i == current ? 1 : 0)
            content[i].visible = (i == current ? true : false)

            content[i].anchors.left = stack.left
            content[i].anchors.right = stack.right
            content[i].anchors.top = stack.top
            content[i].anchors.bottom = stack.bottom
        }
    }

    // Header background
    Rectangle {
        color: colorUnselected
        anchors.top: header.top
        anchors.bottom: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    // Content
    Rectangle {
        id: contentBackground
        color: colorSelected
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    // Header
    Row {
        id: header
        anchors.horizontalCenter: parent.horizontalCenter

        Repeater {
            model: content.length
            delegate: PingImage {
                width: 40
                height: 40
                source: content[index].icon === undefined ?
                    "http://cultofthepartyparrot.com/parrots/hd/parrot.gif" : content[index].icon
                selected: tabWidget.current == index

                Rectangle {
                    z: parent.z - 1
                    anchors.fill: parent
                    color: tabWidget.current == index ? colorUnselected : "transparent"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: tabWidget.current = index
                }
            }
        }
    }

    Item {
        id: stack
        anchors.fill: contentBackground
    }
}