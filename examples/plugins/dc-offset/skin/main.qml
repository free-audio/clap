import QtQuick 2.1
import QtQuick.Controls 2.1

Rectangle {
    width: 300
    height: 200
    color: "#224477"

    Dial {
        id: dc_offset_knob
        property QtObject param: plugin.param(0)
        from: param.minValue
        to: param.maxValue
        value: param.value
        inputMode: Dial.Vertical
        onMoved: {
            param.value = value;
        }
    }
}