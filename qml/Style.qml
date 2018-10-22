// Style.qml
pragma Singleton
import QtQuick 2.11
import QtQuick.Controls.Material 2.2

QtObject {
    property color dark: "black"
    property color light: "linen"
    property bool isDark: true
    property color primaryColor: isDark ? light : dark
    property int theme: isDark ? Material.Dark : Material.Light

    function useLightStyle() {
        isDark = false
    }

    function useDarkStyle() {
        isDark = true
    }
}
