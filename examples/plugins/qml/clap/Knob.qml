import QtQuick 2.1

Canvas {
    property QtObject param
    property int size: 20
    property string backgroundColor: "#222222";
    property string valueColor: "#ffffff";
    property string modulationColor: "#3344ff"

    id: knob
    width: size
    height: size

    property real lastY: 0

    Connections {
        target: param
        function onValueChanged() {
            knob.requestPaint();
        }

        function onModulationChanged() {
            knob.requestPaint();
        }
    }

    MouseArea {
        anchors.fill: parent
        drag.axis: Drag.YAxis
        property real lastY: 0
        onPressed: (mouse) => {
                       if (mouse.button === Qt.LeftButton) {
                           lastY = mouse.y;
                           knob.param.isAdjusting = true;
                       }
                   }
        onReleased: (mouse) => {
                        if (mouse.button === Qt.LeftButton) {
                            knob.param.isAdjusting = false;
                        }
                    }
        onPositionChanged: (mouse) => {
                               if (!(mouse.buttons & Qt.LeftButton))
                               return;
                               knob.param.normalizedValue += ((mouse.modifiers & Qt.ShiftModifier) ? 0.001 : 0.01) * (lastY - mouse.y);
                               lastY = mouse.y;
                               knob.requestPaint();
                           }
        onDoubleClicked: (mouse) => {
                             if (!(mouse.buttons & Qt.LeftButton))
                             return;
                             knob.param.value = knob.param.defaultValue;
                         }
    }

    onPaint: {
        var ctx = getContext("2d");

        drawBackground(ctx);
        drawModulation(ctx);
        drawValue(ctx);
    }

    function drawBackground(ctx) {
        ctx.save()

        ctx.fillStyle = backgroundColor;
        ctx.strokeStyle = "black";

        ctx.beginPath();
        ctx.arc(size / 2, size / 2, size / 2, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.stroke();

        ctx.restore()
    }

    function drawModulation(ctx) {
        var radius = size / 15;

        ctx.save();

        ctx.translate(size / 2, size / 2);

        ctx.fillStyle = modulationColor;
        ctx.strokeStyle = "black";

        ctx.beginPath();
        ctx.rotate((param.normalizedModulation - .5) * Math.PI * 4 / 3);
        ctx.arc(0, -size / 2 + 2 * radius, radius, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.stroke();

        ctx.restore();
    }

    function drawValue(ctx) {
        var radius = size / 15;

        ctx.save();

        ctx.translate(size / 2, size / 2);

        ctx.fillStyle = valueColor;
        ctx.strokeStyle = "black";

        ctx.beginPath();
        ctx.rotate((param.normalizedValue - .5) * Math.PI * 4 / 3);
        ctx.arc(0, -size / 2 + 2 * radius, radius, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.stroke();

        ctx.restore();
    }
}
