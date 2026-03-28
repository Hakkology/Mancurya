varying float height;
varying vec3 worldPos;
uniform float time;
uniform vec3 explosionPos[8];
uniform float explosionGlow[8];
uniform vec3 shipPos[64];
uniform float shipScale[64];
uniform float storyTimer;
uniform int shipCount;

void main() {
    vec4 color = gl_Color;
    
    // Su tonları ve derinliği
    color.b += height * 2.0 + 0.1;
    color.g += height * 1.0;
    
    float moonPath = exp(-abs(worldPos.x) * 1.5);
    float shimmer = pow(max(0.0, height * 20.0 + 0.1), 3.0); 
    float flicker = (sin(time * 1.5 + worldPos.z * 4.0) * 0.5 + 0.5);
    
    // Brilliant Cyan/White reflection
    color.rgb += vec3(0.6, 0.9, 1.0) * moonPath * shimmer * flicker * 0.9;
    
    // Dynamic Explosion Glows (Boosted)
    vec3 glowColor = vec3(0.0);
    for(int i=0; i<8; i++) {
        if(explosionGlow[i] > 0.0) {
            float dist = distance(worldPos, explosionPos[i]);
            float glow = exp(-dist * 1.2) * explosionGlow[i] * 2.5;
            glowColor += vec3(1.0, 0.5, 0.2) * glow;
        }
    }
    color.rgb += glowColor;

    // Shader-Based Shadows (Soft 'Shapeless' Blob - Gaussian-like)
    float shadowMask = 0.0;
    for(int i=0; i<shipCount; i++) {
        vec2 diff = worldPos.xz - shipPos[i].xz;
        
        // Normalized Relative Coordinates (Sleek Narrow width)
        // Add subtle wave-based jitter to shadow edges for 'pro-grade' realism
        float jitter = sin(worldPos.x * 3.0 + storyTimer * 2.0) * 0.03;
        float dx = abs(diff.x + jitter) / max(0.01, shipScale[i] * 0.22);
        float dz = (diff.y + shipScale[i] * 0.15) / max(0.01, shipScale[i] * 2.2);
        
        // Circular/Ellipse distance for 'shapeless' blob look
        float dist = length(vec2(dx, dz)); 
        
        // Base Soft Mask
        float s = smoothstep(1.5, 0.0, dist);
        
        // Depth Decay: Shadow gets weaker as it stretches away from hull
        s *= (1.0 - smoothstep(0.0, 2.8, dz)); 
        
        // Asymmetric Fading: Fades out behind the ship (dz < 0) 
        // while remaining soft under the hull
        s *= smoothstep(-0.25, 0.1, dz); 
        
        shadowMask = max(shadowMask, s * 0.99); 
    }
    // Deep black atmosphere
    color.rgb = mix(color.rgb, vec3(0.0), shadowMask);

    gl_FragColor = color;
}
