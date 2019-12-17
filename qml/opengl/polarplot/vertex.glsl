attribute vec4 vertices;
varying vec2 coords;
void main() {
    gl_Position = vertices;
    coords = vertices.xy;
}
