varying vec2 coord;
uniform float horizontalRatio;
uniform float verticalRatio;
uniform sampler2D src;

void main() {
    // Mod is used to wrap around the 0-1 scaled x axis
    vec2 shiftedCoord = mod(coord + vec2(horizontalRatio, 0), 1.0);
    shiftedCoord.y = shiftedCoord.y * verticalRatio;
    gl_FragColor = texture2D(src, shiftedCoord);
}
