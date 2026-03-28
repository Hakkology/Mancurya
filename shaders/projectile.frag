uniform float time;
void main() {
    vec4 color = gl_Color;
    color.r += sin(time * 5.0) * 0.15;
    color.g += cos(time * 3.0) * 0.05;
    gl_FragColor = color;
}
