import QtQuick                      2.7
import QtQuick.Controls             1.2

Item {
    id: pingItem

    signal  activated()

    property bool inPopup: false
    property bool smartVisibility: false
    property bool iconVisible: (pingBtMouseArea.containsMouse || pingItemMouseArea.containsMouse || !smartVisibility) && !inPopup
    property var item: null
    property var hideItem: true
    property var hoverParent: undefined

    onItemChanged: {
        item.parent = itemRect
        item.opacity = itemRect.opacity
        item.anchors.horizontalCenter = itemRect.horizontalCenter
        item.anchors.verticalCenter = itemRect.verticalCenter
    }

    MouseArea {
        id: pingItemMouseArea
        anchors.fill: parent
        enabled: smartVisibility
        hoverEnabled: true
        onClicked: {
            hideItem: true
        }
    }

    onHideItemChanged: {
        itemRect.hide = hideItem
    }

    Rectangle {
        id: iconRect
        anchors.left: parent.left
        anchors.top: parent.top

        height: openIcon.height
        width: openIcon.width

        visible: iconVisible

        color: Qt.rgba(0,0,0,0.4)

        Image {
            id: openIcon
            source:         "/icons/arrow_right_white.svg"
            fillMode:       Image.PreserveAspectFit
            mipmap:         true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            visible: parent.visible
            property var flip: !itemRect.hide

            RotationAnimator on rotation {
                id: rotateIcon
                from: 0
                to: 0
                duration: 200
                running: true
            }

            onFlipChanged: {
                rotateIcon.from = rotateIcon.to
                rotateIcon.to = flip ? 180 : 0
                rotateIcon.running = true
            }
        }

        MouseArea {
            id: pingBtMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                itemRect.hide = !itemRect.hide
            }
        }
    }

    Rectangle {
        id: itemRect
        opacity: 0
        anchors.left: iconRect.right
        anchors.top: iconRect.top

        height: item.height
        width: item.width

        color: Qt.rgba(0,0,0,0.4)
        property var hide: true

        onOpacityChanged: {
            item.opacity = itemRect.opacity
        }

        OpacityAnimator {
            id: rectOpa
            target: itemRect
            from: 0
            to: 0
            duration: 200
            running: true
        }

        OpacityAnimator {
            id: itemOpa
            target: item
            from: 0
            to: 0
            duration: 200
            running: true
        }

        onHideChanged: {
            rectOpa.from = rectOpa.to
            rectOpa.to = hide ? 0 : 1
            rectOpa.running = true

            itemOpa.from = itemOpa.to
            itemOpa.to = hide ? 0 : 1
            itemOpa.running = true
        }
    }
}
