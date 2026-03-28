#include <Mancurya/Sea.hpp>
#include <GL/gl.h>

namespace Mancurya {

void Sea::init() {
    shader.load("shaders/water.vert", "shaders/water.frag");
}

void Sea::update() {
    waveOffset += 0.01f;
}

void Sea::draw() {
    shader.use();
    shader.setUniform1f("time", waveOffset * 10.0f);
    
    // Performance Optimized: GL_TRIANGLE_STRIP for efficient GPU rendering
    float step = 0.1f; 
    for (float x = -10.0f; x < 10.0f; x += step) {
        glBegin(GL_TRIANGLE_STRIP);
        for (float z = -10.0f; z <= 10.0f; z += step) {
            glColor4f(0.0f, 0.2f, 0.45f, 0.9f);
            glVertex3f(x, -0.5f, z);
            glVertex3f(x + step, -0.5f, z);
        }
        glEnd();
    }
    
    Shader::stop();
}

}
