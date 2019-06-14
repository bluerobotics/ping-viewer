import QtQuick 2.12
import QtQuick.Shapes 1.12

Item {
    id: root
    width: 400
    height: width

    Shape {
        anchors.fill: parent
        vendorExtensionsEnabled: false

        // polar axis
        ShapePath {
            id: shapePathPolar
            fillColor: "transparent"
            strokeColor: "black"
            strokeWidth: 2
            strokeStyle: ShapePath.DashLine
            dashPattern: [4, 6]
            property var centerX: root.width/2
            property var centerY: root.height/2
            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: root.width/2
                radiusY: root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (3/4)*root.width/2
                radiusY: (3/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (2/4)*root.width/2
                radiusY: (2/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (1/4)*root.width/2
                radiusY: (1/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }
        }

        ShapePath {
            fillColor: "transparent"
            strokeColor: "white"
            strokeWidth: 2
            strokeStyle: ShapePath.DashLine
            dashOffset: 5
            dashPattern: [4, 6]
            property var centerX: root.width/2
            property var centerY: root.height/2
            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: root.width/2
                radiusY: root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (3/4)*root.width/2
                radiusY: (3/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (2/4)*root.width/2
                radiusY: (2/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (1/4)*root.width/2
                radiusY: (1/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }
        }
    }

    Canvas {
        anchors.fill: parent
        opacity: 0.8
        property var angle: 45

        onPaint: {
            var centerX = root.width/2
            var centerY = root.height/2
            var radius = root.height/2 - shapePathPolar.strokeWidth
            var ctx = getContext("2d")
            ctx.lineWidth = 1
            ctx.strokeStyle = "white"
            ctx.beginPath()

            for(var i=0; i*angle < 360; i++){
                ctx.moveTo(centerX, centerY);
                var angleRadians = i*angle*Math.PI/180;
                ctx.lineTo(radius*Math.cos(angleRadians) + centerX, radius*Math.sin(angleRadians) + centerY);
            }

            ctx.closePath()
            ctx.stroke()
        }
    }
}
