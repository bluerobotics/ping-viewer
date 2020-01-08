varying highp vec2 coord;
uniform lowp float qt_Opacity;
void main() {
    float d = distance(coord, vec2(0.5, 0.5));
    if ( d > 0.5) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0) * qt_Opacity;
        return;
    }
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) * qt_Opacity;
}
