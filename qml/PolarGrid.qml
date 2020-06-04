import QtQuick 2.12
import QtQuick.Shapes 1.12
import SettingsManager 1.0

Item {
    id: root

    property var maxDistance: 100
    property var angle: 360

    width: 400
    height: width
    onAngleChanged: canvas.requestPaint()

    Repeater {
        id: repeater

        property var numberOfRings: 4
        property var radius: root.height / 2
        property var centerX: root.width / 2
        property var centerY: root.height / 2
        // Distance text properties
        property string units: SettingsManager.distanceUnits["distance"]
        property real scalar: SettingsManager.distanceUnits["distanceScalar"]

        function formatDistance(distance) {
            // Change precision based in distance
            var precision = 0;
            if (root.maxDistance < 5)
                precision = 1;

            // Round number to the displayed precision
            // This is done my moving the nth (precision) decimal number to the 'units' position
            // and after that back to the decimal position
            // $result = \frac{round(x \times 10^p)}{10^p}$
            var powerOfTen = Math.pow(10, precision);
            return (Math.round(distance * scalar * powerOfTen) / powerOfTen).toFixed(precision) + units;
        }

        anchors.fill: parent
        model: numberOfRings

        delegate: Text {
            // Calculate the angle and radius for each item
            // The final angle in radians will be the last angle in the right side
            property var itemRadian: ((root.angle / 2) % 90) * Math.PI / 180 - ((root.angle % 180) ? Math.PI / 2 : 0)
            property var itemRadius: repeater.radius * (index + 1) / repeater.numberOfRings

            font.bold: true
            font.pixelSize: 15
            text: repeater.formatDistance(root.maxDistance * (1 + index) / repeater.numberOfRings)
            //The position should change for each visible angle
            x: (1.2 * width / 2 + itemRadius) * Math.cos(itemRadian) + Math.abs((1.2 * width / 2) * Math.sin(itemRadian)) + repeater.centerX - width / 2
            y: (1.1 * height / 2 + itemRadius) * Math.sin(itemRadian) - Math.abs((1.1 * height / 2) * Math.cos(itemRadian)) + repeater.centerY - height / 2
            style: Text.Outline
            color: "white"
            styleColor: "black"
        }

    }

    Shape {
        anchors.fill: parent
        vendorExtensionsEnabled: false

        // polar axis
        ShapePath {
            id: shapePathPolar

            property var centerX: root.width / 2
            property var centerY: root.height / 2

            fillColor: "transparent"
            strokeColor: "white"
            strokeWidth: 1

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: root.width / 2
                radiusY: root.height / 2
                startAngle: -root.angle / 2
                sweepAngle: root.angle
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (3 / 4) * root.width / 2
                radiusY: (3 / 4) * root.height / 2
                startAngle: -root.angle / 2
                sweepAngle: root.angle
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (2 / 4) * root.width / 2
                radiusY: (2 / 4) * root.height / 2
                startAngle: -root.angle / 2
                sweepAngle: root.angle
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (1 / 4) * root.width / 2
                radiusY: (1 / 4) * root.height / 2
                startAngle: -root.angle / 2
                sweepAngle: root.angle
            }

        }

        // PathAngleArc uses 3 o'clock position as 0 degrees
        // We are going to move it to 12h to save us the trouble to correct each PathAngleArc
        transform: Rotation {
            origin.x: width / 2
            origin.y: height / 2
            angle: -90
        }

    }

    Canvas {
        id: canvas

        property var angle: root.angle / 45 > 3 ? 45 : root.angle / 4

        anchors.fill: parent
        opacity: 0.8
        onPaint: {
            var centerX = root.width / 2;
            var centerY = root.height / 2;
            var radius = root.height / 2 - shapePathPolar.strokeWidth;
            var ctx = getContext("2d");
            ctx.reset();
            ctx.lineWidth = 1;
            ctx.strokeStyle = "white";
            ctx.beginPath();
            for (var i = 0; i <= root.angle / angle; i++) {
                ctx.moveTo(centerX, centerY);
                // Correct arc to centralize around 12 o'clock position
                var angleRadians = -i * canvas.angle * Math.PI / 180 - ((root.angle % 180) ? Math.PI / 2 - root.angle * Math.PI / 360 : 0);
                ctx.lineTo(radius * Math.cos(angleRadians) + centerX, radius * Math.sin(angleRadians) + centerY);
            }
            ctx.closePath();
            ctx.stroke();
        }
    }

}
