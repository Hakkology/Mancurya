varying float height;
varying vec3 worldPos;
uniform float time;
void main() {
    vec4 color = gl_Color;
    
    // Su tonları ve derinliği
    color.b += height * 2.0 + 0.1;
    color.g += height * 1.0;
    
    // LINEAR YAKAMOZ (Ay Yolu - Moon Path)
    // Ay merkeze (x=0) yakın olduğu için x ekseninde çizgisel bir aydınlanma
    // abs(worldPos.x) arttıkça aydınlanma düşer (Moon Path etkisi)
    float moonPath = exp(-abs(worldPos.x) * 2.5);
    
    // Sadece dalga tepelerinde (height > 0) parıltı (Masterpiece Tuning)
    float shimmer = pow(max(0.0, height * 15.0), 4.0);
    
    // Zaman bazlı pırıltı
    float flicker = (sin(time * 1.0 + worldPos.z * 3.0) * 0.5 + 0.5);
    
    // Çizgisel yansıma (Soft Cyan/White)
    color.rgb += vec3(0.5, 0.8, 1.0) * moonPath * shimmer * flicker * 0.7;
    
    gl_FragColor = color;
}
