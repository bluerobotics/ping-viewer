/**
 * Based over: https://gist.github.com/KeyMaster-/2bb5e20f824241f3caef
 * Tilman Schmidt [KeyMaster-] example
 */

// Set float precision
precision mediump float;

uniform sampler2D src;
uniform float angle;
varying vec2 coord;

void main() {
    vec2 sizeOverRadius = vec2(2.0, 2.0);
    float sampleOffset = 0.0;
    float polarFactor = 1.0;

    //Move position to the center
    vec2 relPos = coord - vec2(0.5 ,0.5);

    //Adjust for screen ratio
    relPos *= sizeOverRadius;

    //Normalised polar coordinates.
    //y: radius from center
    //x: angle
    vec2 polar;

    polar.y = sqrt(relPos.x * relPos.x + relPos.y * relPos.y);

    //Any radius over 1 would go beyond the source texture size, this simply outputs black for those fragments
    if(polar.y > 1.0){
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    polar.x = atan(relPos.y, relPos.x);

    //Fix glsl origin with src data
    polar.x += 3.1415/2.0;

    //Normalise from angle to 0-1 range
    polar.x /= 3.1415*2.0;
    polar.x = mod(polar.x, 1.0);

    //The xOffset fixes lines disappearing towards the center of the coordinate system
    //This happens because there's only a few pixels trying to display the whole width of the source image
    //so they 'miss' the lines. To fix this, we sample at the transformed position
    //and a bit to the left and right of it to catch anything we might miss.
    //Using 1 / radius gives us minimal offset far out from the circle,
    //and a wide offset for pixels close to the center
    float xOffset = 0.0;
    if(polar.y != 0.0){
        xOffset = 1.0 / polar.y;
    }

    //Adjusts for texture resolution
    xOffset *= sampleOffset;

    //This inverts the radius variable depending on the polarFactor
    polar.y = polar.y * polarFactor + (1.0 - polar.y) * (1.0 - polarFactor);

    //Sample at positions with a slight offset
    vec4 one = texture2D(src, vec2(polar.x - xOffset, polar.y));
    vec4 two = texture2D(src, polar);
    vec4 three = texture2D(src, vec2(polar.x + xOffset, polar.y));
    gl_FragColor = max(max(one, two), three);
}
