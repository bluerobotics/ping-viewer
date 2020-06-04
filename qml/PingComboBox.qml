import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ComboBox {
    id: root

    property var _maxWidth: 0

    onModelChanged: {
        _maxWidth = 0;
        for (var i = 0; i < model.length; i++) {
            // TextMetrics does not work with Material Style
            _maxWidth = Math.max((model[i].length + 1) * Qt.application.font.pixelSize, _maxWidth);
        }
        Layout.minimumWidth = _maxWidth + implicitIndicatorWidth + leftPadding + rightPadding;
    }
}
