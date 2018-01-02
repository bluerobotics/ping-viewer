import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0
import "."

ApplicationWindow {
    id: window
    title: "Ping Interface"

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        QC1.SplitView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal

            Waterfall {
                id: waterfall
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Chart {
                id: chart
                Layout.fillHeight: true
                Layout.preferredWidth: 350
                Layout.minimumWidth: 350
            }
        }
    }

    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height

        property alias chartWidth: chart.width
    }

    LinearGradient {
        anchors.fill: parent
        z: parent.z-1
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#200000FF" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
}