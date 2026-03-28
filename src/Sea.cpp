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
    
    // Even higher resolution grid for ultra-smooth waves and yakamoz (0.1f step)
    float step = 0.1f; 
    glBegin(GL_QUADS);
    for (float x = -10.0f; x < 10.0f; x += step) {
        for (float z = -10.0f; z < 10.0f; z += step) {
            glColor4f(0.0f, 0.2f, 0.45f, 0.9f);
            glVertex3f(x, -0.5f, z);
            glVertex3f(x + step, -0.5f, z);
            glVertex3f(x + step, -0.5f, z + step);
            glVertex3f(x, -0.5f, z + step);
        }
    }
    glEnd();
    
    Shader::stop();
}

}
