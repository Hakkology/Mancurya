varying float height;
varying vec3 worldPos;
uniform float time;
uniform vec3 explosionPos[8];
uniform float explosionGlow[8];

void main() {
    vec4 color = gl_Color;
    
    // Su tonları ve derinliği
    color.b += height * 2.0 + 0.1;
    color.g += height * 1.0;
    
    float moonPath = exp(-abs(worldPos.x) * 1.5);
    
    // Increased shimmer intensity on wave peaks
    float shimmer = pow(max(0.0, height * 20.0 + 0.1), 3.0); 
    
    // Smooth flicker for water sparkle
    float flicker = (sin(time * 1.5 + worldPos.z * 4.0) * 0.5 + 0.5);
    
    // Brilliant Cyan/White reflection
    color.rgb += vec3(0.6, 0.9, 1.0) * moonPath * shimmer * flicker * 0.9;
    
    // Dynamic Explosion Glows (Boosted)
    vec3 glowColor = vec3(0.0);
    for(int i=0; i<8; i++) {
        if(explosionGlow[i] > 0.0) {
            float dist = distance(worldPos, explosionPos[i]);
            // Wider range and higher intensity
            float glow = exp(-dist * 1.2) * explosionGlow[i] * 2.5;
            glowColor += vec3(1.0, 0.5, 0.2) * glow;
        }
    }
    color.rgb += glowColor;

    gl_FragColor = color;
}
