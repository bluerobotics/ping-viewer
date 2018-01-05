import QtQuick.Controls 2.3
import QtQuick 2.7

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
}