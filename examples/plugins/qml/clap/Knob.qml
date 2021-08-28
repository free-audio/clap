import QtQuick 2.1
import QtQuick.Controls 2.1

Item {
    property QtObject param
    property int size: 20
    id: knob
    width: size
    height: size

    Rectangle {
        width: knob.size
        height: knob.size
        radius: knob.size / 2
        color: "#332277"

        MouseArea {
            anchors.fill: parent
            drag.axis: Drag.YAxis
            property real lastY: 0
            onPressed: (mouse) => {
                if (mouse.button === Qt.LeftButton) {
                    lastY = mouse.y;
                    knob.param.isAdjusting = true
                }
            }
            onReleased: (mouse) => {
                if (mouse.button === Qt.LeftButton) {
                    knob.param.isAdjusting = false
                }
            }
            onPositionChanged: (mouse) => {
                if (!(mouse.buttons & Qt.LeftButton))
                    return;
                knob.param.normalizedValue += ((mouse.modifiers & Qt.ShiftModifier) ? 0.001 : 0.01) * (mouse.y - lastY);
                lastY = mouse.y;
            }
        }
    }

    Item {
        Rectangle {
            x: knob.size / 2 - knob.size / 40
            y: knob.size / 20
            height: knob.size / 10
            width: knob.size / 20
            radius: knob.size / 40
            color: "#2282ff"
        }

        transform: Rotation {
            angle: knob.param.normalizedModulation * 270 - 135
            origin.x: knob.size / 2
            origin.y: knob.size / 2
        }
    }

    Rectangle {
        x: knob.size / 2 - knob.size / 20
        y: knob.size / 20
        height: knob.size / 10
        width: knob.size / 10
        radius: knob.size / 40
        color: "#dd82ff"
    }

    transform: Rotation {
        angle: knob.param.normalizedValue * 270 - 135
        origin.x: knob.size / 2
        origin.y: knob.size / 2
    }
}
