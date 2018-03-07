import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

Item {
    id: root

    height: mainLayout.height
    width: mainLayout.width

    ColumnLayout {
        id: mainLayout
        anchors.top: parent.top
        Layout.topMargin: 0
        Layout.minimumHeight: 200
        Layout.minimumWidth: 200

        Text {
            id: titleName
            Layout.minimumHeight: 40
            text: "Link Management"
            color: 'linen'
            font.bold: true
            Layout.fillWidth: true
            anchors.top: parent.top
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        ColumnLayout {
            id: content
            anchors.top: titleName.bottom
            anchors.right: root.right
            Layout.fillWidth: true
            Layout.leftMargin: 5
            Layout.rightMargin: 0
            Layout.minimumWidth: 300
            spacing: 1

            ListModel {
                id: linkModel
                ListElement {
                    argument: "2:/dev/ttyUSB0:115200"
                    connected: false
                }
            }

            ListView {
                id: listView
                model: linkModel
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                Layout.minimumHeight: 350
                Layout.fillWidth: true
                clip: true
                spacing: 4
                delegate: LinkManagementItem {
                    width: parent.width
                }
                ScrollBar.vertical: ScrollBar {
                    active: true
                    snapMode: ScrollBar.SnapAlways
                    width: 5
                }
            }

            Image {
                id: addLink
                mipmap: true
                source: "/icons/plus_one_white.svg"
                Layout.maximumHeight: 35
                fillMode: Image.PreserveAspectFit
                anchors.right: parent.right

                Rectangle {
                    z: addLink.z - 1
                    color: "Black"
                    opacity: 0.45
                    border.width: 1
                    anchors.fill: parent
                }

                MouseArea {
                    id: addLinkBt
                    anchors.fill: parent
                    onClicked: {
                        var data = {
                            'argument': "2:/dev/ttyUSB0:115200",
                            'connected': false,
                        }
                        linkModel.append(data)
                    }
                }
            }
        }
    }
}