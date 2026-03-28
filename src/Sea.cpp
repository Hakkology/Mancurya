#include <Mancurya/Sea.hpp>
#include <GL/gl.h>

namespace Mancurya {

void Sea::init() {
    shader.load("../shaders/water.vert", "../shaders/water.frag");
}

void Sea::update() {
    waveOffset += 0.01f;
}

void Sea::draw() {
    shader.use();
    shader.setUniform1f("time", waveOffset * 10.0f);
    glBegin(GL_QUADS);
    for(float x=-10; x<=10; x+=0.25f) {
        for(float z=-10; z<=10; z+=0.25f) {
            glColor4f(0.0f, 0.2f, 0.45f, 0.9f);
            glVertex3f(x, -0.5f, z); 
            glVertex3f(x+0.25f, -0.5f, z);
            glVertex3f(x+0.25f, -0.5f, z+0.25f); 
            glVertex3f(x, -0.5f, z+0.25f);
        }
    }
    glEnd(); 
    Shader::stop();
}

}
