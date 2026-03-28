uniform float time;
varying float height;
varying vec3 worldPos;
void main() {
    vec4 v = vec4(gl_Vertex);
    // Daha temiz, akışkan dalgalar
    float h = sin(v.x * 1.2 + time * 1.1) * 0.04;
    h += cos(v.z * 1.0 + time * 1.3) * 0.03;
    v.y += h;
    height = h;
    worldPos = v.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * v;
    gl_FrontColor = gl_Color;
}
