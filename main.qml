import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: window
    title: "Ping Interface"

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
    }

     Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }
}