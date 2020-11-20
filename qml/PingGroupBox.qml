import QtQuick 2.15
import QtQuick.Controls 2.5

GroupBox {
    // We can't do the direct bind with the template element in the component body
    Binding {
        target: label
        property: "horizontalAlignment"
        when: visible
        value: Text.AlignHCenter
    }

}
