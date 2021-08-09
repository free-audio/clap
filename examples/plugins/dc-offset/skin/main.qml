import QtQuick 2.1
import QtQuick.Controls 2.1

Rectangle {
    width: 300
    height: 200
    color: "#224477"

    Dial {
        property QtObject param: plugin.getParam(0)
        inputMode: Dial.Vertical
    }
}