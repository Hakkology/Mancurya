varying float height;
uniform float time;
void main() {
    vec4 color = gl_Color;
    
    // Temel deniz rengi
    color.b += height * 2.0 + 0.1;
    color.g += height * 1.0;
    
    // YAKAMOZ (Moonlight reflection)
    // Dalga tepelerinde parıldama efekti
    float yakamoz = pow(max(0.0, height * 10.0), 3.0);
    yakamoz *= (sin(time * 5.0 + height * 50.0) * 0.5 + 0.5); // Pırıltı
    
    color.rgb += vec3(0.5, 0.7, 1.0) * yakamoz;
    
    gl_FragColor = color;
}
