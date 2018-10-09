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
        model: Logger.logModel
        delegate: Row {
            width: parent.width
            spacing: 10
            Text {
                id: leftText
                text: time
                visible: enabledCategories & category
                color: foreground == undefined ? "purple" : foreground
            }
            Text {
                width: parent.width - leftText.width
                wrapMode: Text.WordWrap
                text: display
                visible: enabledCategories & category
                color: foreground == undefined ? "purple" : foreground
            }
        }
        ScrollBar.horizontal: ScrollBar { }
        ScrollBar.vertical: ScrollBar { }

        onCountChanged: {
            if(scrollLockEnabled) {
                // This appears to be better than positionViewAtEnd
                // Less bug
                positionViewAtIndex(count - 1, ListView.Beginning)
            }
        }
    }
}
