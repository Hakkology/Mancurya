uniform float time;
void main() {
    vec4 v = vec4(gl_Vertex);
    v.y += sin(v.x * 2.0 + time) * 0.05;
    v.y += cos(v.z * 1.5 + time * 0.8) * 0.03;
    gl_Position = gl_ModelViewProjectionMatrix * v;
    gl_FrontColor = gl_Color;
}
