import QtQuick 2.1

Canvas {
   property QtObject param
   property int size: 20;
   property string backgroundColor: "#0c002b";
   property string ringColor: "#adabb3";
   property string knobColor: "#706d6b";
   property string valueColor: "#ffffff";
   property string modulationColor: "#10b1ca"
   property double modulationMargin: .05;
   property double ringAngle: Math.PI * 5 / 3;

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
         if (mouse.button === Qt.LeftButton)
            knob.param.setToDefault();
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
      ctx.strokeStyle = "black";

      ctx.translate(size / 2, size / 2, size / 2);
      ctx.rotate(ringAngle + Math.PI);

      ctx.beginPath();
      ctx.arc(0, 0, size / 2, 0, 2 * Math.PI, false);
      ctx.fillStyle = backgroundColor;
      ctx.fill();
      ctx.stroke();

      ctx.beginPath();
      ctx.arc(0, 0, size / 2 - size * modulationMargin, 0, ringAngle, false);
      ctx.arc(0, 0, size / 2, ringAngle, 0, true);
      ctx.fillStyle = ringColor;
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
      ctx.rotate((param.normalizedValue - .5) * ringAngle - Math.PI / 2);

      var finalValueAngle = (param.normalizedFinalValue - param.normalizedValue) * ringAngle;
      ctx.arc(0, 0, size / 2, 0, finalValueAngle, finalValueAngle < 0);
      ctx.arc(0, 0, size / 2 - size * modulationMargin, finalValueAngle, 0, finalValueAngle > 0);
      ctx.fill();

      ctx.restore();
   }

   function drawValue(ctx) {
      var radius = size / 40;

      ctx.save();

      ctx.translate(size / 2, size / 2);

      ctx.fillStyle = valueColor;
      ctx.strokeStyle = "black";

      ctx.rotate((param.normalizedValue - .5) * ringAngle);
      ctx.beginPath();
      var y0 = -size / 2 + size * modulationMargin;
      var y1 = y0 + size / 3;
      ctx.rect(-radius, 0, 2 * radius, -size / 2);
      ctx.fill();
      ctx.stroke();

      ctx.restore();
   }
}
