// Style.qml
pragma Singleton
import QtQuick 2.0
import QtQuick.Controls.Material 2.1

QtObject {
    property color textColor: 'linen'
    property color iconColor: textColor
    property color color: isDark ? "black" : "linen"
    property string dark: "dark"
    property string light: "light"
    property bool isDark: true
    property int theme: Material.Dark

    function useLightStyle() {
        theme = Material.Light
        textColor = 'black'
        isDark = false;
    }

    function useDarkStyle() {
        theme = Material.Dark
        textColor = 'linen'
        isDark = true;
    }
}