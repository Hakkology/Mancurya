varying float height;
varying vec3 worldPos;
uniform float time;
void main() {
    vec4 color = gl_Color;
    
    // Su tonları ve derinliği
    color.b += height * 2.0 + 0.1;
    color.g += height * 1.0;
    
    float moonPath = exp(-abs(worldPos.x) * 1.5); // Widened from 2.5
    
    // Increased shimmer intensity on wave peaks
    float shimmer = pow(max(0.0, height * 20.0 + 0.1), 3.0); 
    
    // Smooth flicker for water sparkle
    float flicker = (sin(time * 1.5 + worldPos.z * 4.0) * 0.5 + 0.5);
    
    // Brilliant Cyan/White reflection
    color.rgb += vec3(0.6, 0.9, 1.0) * moonPath * shimmer * flicker * 0.9;
    
    gl_FragColor = color;
}
