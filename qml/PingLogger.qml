import QtQuick 2.7
import QtQuick.Controls 2.2

import Logger 1.0
import SettingsManager 1.0

Item {
    id: root

    // Enable all categories
    property int enabledCategories: SettingsManager.enabledCategories
    property bool scrollLockEnabled: SettingsManager.logScrollLock

    onEnabledCategoriesChanged: {
        Logger.logModel.filter(enabledCategories)
        if(enabledCategories != SettingsManager.enabledCategories) {
            SettingsManager.enabledCategories = enabledCategories
        }
        listView.positionViewAtEnd()
    }

    onScrollLockEnabledChanged: {
        if(scrollLockEnabled != SettingsManager.logScrollLock) {
            SettingsManager.logScrollLock = scrollLockEnabled
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        contentWidth: parent.width
        clip: true
        flickableDirection: Flickable.HorizontalAndVerticalFlick
        model: Logger.logModel.filteredModel
        delegate: Row {
            width: parent.width
            spacing: 10
            Text {
                id: leftText
                text: time
                color: foreground == undefined ? "purple" : foreground
            }
            TextEdit {
                text: display
                width: parent.width - leftText.width
                wrapMode: Text.WordWrap
                readOnly: true
                selectByMouse: true
                color: foreground == undefined ? "purple" : foreground
            }
        }
        ScrollBar.horizontal: ScrollBar { }
        ScrollBar.vertical: ScrollBar { }

        // After we moved from StringListModel to our own "QQmlListModel"
        // We forgot to add the count signal to be able to do the scroll lock
        Connections {
            target: Logger.logModel
            onCountChanged: {
                if(scrollLockEnabled) {
                    listView.currentIndex = listView.count - 1
                }
            }
        }
    }
}
