import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.1

ApplicationWindow {
    id: window
    title: "Ping Interface"
    visible: true
    height: 600
    width: 800

    Material.theme: Material.Dark
    Material.accent: Material.Blue
    Material.foreground: Material.Blue

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: mainPage

        MainPage {
            id: mainPage
        }

        SettingsPage {
            id: settingsPage
        }

        pushEnter: Transition {
            PropertyAnimation {
                property: "y"
                from: -window.height
                to: 0
                duration: 200
            }
        }
        pushExit: Transition {
            PropertyAnimation {
                property: "y"
                from: 0
                to: window.height
                duration: 200
            }
        }
        popEnter: Transition {
            PropertyAnimation {
                property: "y"
                from: window.height
                to: 0
                duration: 200
            }
        }
        popExit: Transition {
            PropertyAnimation {
                property: "y"
                from: 0
                to: -window.height
                duration: 200
            }
        }
    }

    // Save windows position and size
    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }
}
