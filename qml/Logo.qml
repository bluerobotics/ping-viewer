import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

ApplicationWindow {
    id: window
    visible: true
    flags: Qt.FramelessWindowHint
    width: image.width
    height: image.height
    color: 'transparent'
    Image {
        id: image
        source: "/imgs/br_logo.png"
        fillMode: Image.PreserveAspectFit
        mipmap: true
        opacity: 0

        onOpacityChanged: {
            window.close()
        }
    }

    OpacityAnimator {
        id: imageOpa
        target: image
        from: 0
        to: 1
        duration: 1750
        running: true
        easing.type: Easing.OutCubic
    }

    Component.onCompleted: {
        window.x = Screen.width / 2 - width / 2
        window.y = Screen.height / 2 - height / 2
    }
}