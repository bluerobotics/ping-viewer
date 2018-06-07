import QtQuick                      2.7
import QtQuick.Controls             1.2
import QtGraphicalEffects 1.0

Item {
    id: pingItem
    z: 1
    height: iconRect.height
    width: iconRect.width
    // Default state
    state: "top-left"
    signal  activated()

    property bool flip: !hideItem
    property bool hideItem: true
    property bool iconVisible: (pingBtMouseArea.containsMouse || pingItemMouseArea.containsMouse || !smartVisibility) && !inPopup
    property bool inPopup: false
    property bool isSubItem: false
    property bool pingpong: false
    property bool smartVisibility: false
    property bool spin: false
    property int animationType: Easing.Linear
    property bool clicked: false
    property var color: hideItem ? colorUnselected : colorSelected
    property var colorSelected: Style.isDark ? Qt.rgba(0,0,0,0.75) : Qt.rgba(1,1,1,0.75)
    property var colorUnselected: Style.isDark ? Qt.rgba(0,0,0,0.5) : Qt.rgba(1,1,1,0.5)
    property real finalAngle: 180
    property var finalAngleValue: spin && !pingpong ? 360 : finalAngle
    property var hoverParent: undefined
    property var icon: undefined
    property var item: null
    property real marginMult: 1.05
    property real startAngle: 0

    onItemChanged: {
        if(item == null) {
            return
        }
        item.parent = itemRect
        item.enabled = false
        item.visible = false
        item.z = 1
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
            hideItem = true
        }
    }

    onHideItemChanged: {
        itemRect.hide = hideItem
    }

    onFlipChanged: {
        rotateIcon.from = rotateIcon.to
        rotateIcon.to = pingItem.flip ? pingItem.finalAngleValue : pingItem.startAngle
        rotateIcon.running = true
    }

    Rectangle {
        id: iconRect
        anchors.left: parent.left
        anchors.top: parent.top

        height: 40
        width: 40

        visible: iconVisible

        color: isSubItem ? 'transparent' : pingItem.color

        PingImage {
            id: openIcon
            source: pingItem.icon
            anchors.fill: parent
            selected: !pingItem.hideItem

            RotationAnimator on rotation {
                id: rotateIcon
                from: startAngle
                to: startAngle
                duration: pingItem.pingpong ? 1000 : (pingItem.spin ? 2000 : 200)
                direction: pingItem.pingpong ?
                    RotationAnimation.Shortest : (itemRect.hide ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise)
                running: true
                easing.type: animationType
                onRunningChanged: {
                    if(running) {
                        return
                    }

                    if((pingItem.spin && pingItem.clicked) || pingItem.pingpong) {
                        pingItem.flip = !pingItem.flip
                    }
                }
            }
        }

        MouseArea {
            id: pingBtMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                itemRect.hide = !itemRect.hide
                hideItem = itemRect.hide
                pingItem.clicked = !pingItem.clicked
            }
        }
    }

    states: [
        State {
            name: "top-left"
            AnchorChanges {
                target: itemRect
                anchors.top: iconRect.top
                anchors.bottom: undefined
                anchors.right: undefined
                anchors.left: iconRect.right
            }
        },
        State {
            name: "bottom-left"
            AnchorChanges {
                target: itemRect
                anchors.top: undefined
                anchors.bottom: iconRect.bottom
                anchors.right: undefined
                anchors.left: iconRect.right
            }
        },
        State {
            name: "bottom-right"
            AnchorChanges {
                target: itemRect
                anchors.top: undefined
                anchors.bottom: iconRect.bottom
                anchors.right: iconRect.left
                anchors.left: undefined
            }
        },
        State {
            name: "fill-right"
            AnchorChanges {
                target: itemRect
                anchors.top: mainPage.top
                anchors.bottom: undefined
                anchors.right: undefined
                anchors.left: iconRect.right
            }
            PropertyChanges {
                target: itemRect
                y: - pingItem.y
                height: mainPage.height
                width: mainPage.width - pingItem.width
            }
        }
    ]

    Rectangle {
        id: itemRect
        opacity: 0

        height: item != null ? item.height*marginMult : 0
        width: item != null ? item.width*marginMult : 0
        color: pingItem.color
        property bool hide: true

        onOpacityChanged: {
            item.opacity = itemRect.opacity
            item.visible = itemRect.opacity != 0
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
            if(item != null) {
                item.enabled = !hide
                rectOpa.from = rectOpa.to
                rectOpa.to = hide ? 0 : 1
                rectOpa.running = true

                itemOpa.from = itemOpa.to
                itemOpa.to = hide ? 0 : 1
                itemOpa.running = true
            }
        }
    }
}
