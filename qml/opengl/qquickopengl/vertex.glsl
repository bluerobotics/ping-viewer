uniform highp mat4 qt_Matrix;

attribute highp vec4 qt_Vertex;
attribute highp vec2 qt_MultiTexCoord0;

varying highp vec2 coord;

void main() {
    gl_Position = qt_Matrix * qt_Vertex;
    coord = qt_MultiTexCoord0;// + vec2(1.0, 1.0)) / 2.0;
}
