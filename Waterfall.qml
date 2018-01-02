import QtQuick 2.0

Item {
    id: root

    Canvas {
        id: graph
        anchors.fill : parent
        anchors.left: parent.right
        anchors.top: parent.top
        property var graphType: 'thermal'

        function dot(x, y, r, color1, color2) {
            var ctx = getContext("2d")
            ctx.beginPath()
            var rad = ctx.createRadialGradient(x+r, y+r, 1, x+r, y+r, r)
            rad.addColorStop(0, color1)
            rad.addColorStop(1, color2)
            ctx.fillStyle = rad
            ctx.fillRect(x, y, 2*r, 2*r)
        }

        function draw(points)
        {
            // Set some values
            var scaleW = 50
            var scaleH = 200
            var stepH = graph.canvasSize.height/scaleH
            var radius = Math.pow(stepH*stepH, 0.5)
            var stepW = radius/2 < 1 ? 1 : radius/2
            var posX = width-stepW

            // Move everything to left
            var ctx = graph.getContext("2d")
            ctx.fillStyle = 'transparent'
            var img = ctx.getImageData(0, 0, graph.canvasSize.width, graph.canvasSize.height)
            ctx.drawImage(img, -stepW, 0, graph.canvasSize.width, graph.canvasSize.height)

            // Draw new column
            for(var j=0; j<scaleH; j++) {
                var perc = points == undefined ? Math.random()/*0*/ : points[j]
                var posY = j*stepH
                //TODO: test this approach with:
                //  Append a series of 200 addColorStop and draw a rectangle
                var minimum = 0
                var maximum = 1
                var ratio = 2 * (perc-minimum) / (maximum - minimum)
                var b = Math.max(0, (1 - ratio))
                var r = Math.max(0, (ratio - 1))
                var g = 1 - b - r
                dot(posX, posY, stepW, Qt.rgba(r, g, b, 1), Qt.rgba(r, g, b, 0))
            }
        }

        Timer {
            id: testTimer
            interval: 100; running: true; repeat: true
            onTriggered: {
                graph.draw()
            }
        }
    }
}