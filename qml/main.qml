import QtQuick 2.4
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.1

import StyleManager 1.0

ApplicationWindow {
    id: window
    objectName: "window"
    title: "Ping Interface"
    visible: true
    height: 600
    width: 800

    Material.theme: StyleManager.theme
    Material.primary: StyleManager.primaryColor
    Material.accent: Material.Blue
    Material.foreground: Material.Blue

    MainPage {
        id: mainPage
        anchors.fill: parent
    }

    // Save windows position and size
    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }
}
