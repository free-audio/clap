import QtQuick 2.1
import QtQuick.Controls 2.1
import clap 1.0

Rectangle {
    width: 200
    height: 200
    color: "#224477"

    Knob {
        id: dc_offset_knob
        param: plugin.param(0)
        size: 160
        x: 20
        y: 20
    }
}
