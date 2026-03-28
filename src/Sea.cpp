#include <Mancurya/Sea.hpp>
#include <GL/gl.h>
#include <cstdio>
#include <vector>

namespace Mancurya {

void Sea::init() {
    shader.load("shaders/water.vert", "shaders/water.frag");
}

void Sea::update() {
    waveOffset += 0.01f;
}

void Sea::draw(const std::vector<Explosion>& explosions) {
    shader.use();
    shader.setUniform1f("time", waveOffset * 10.0f);
    
    // Pass Explosion Data to Shader
    for (int i = 0; i < 8; i++) {
        char posName[32], glowName[32];
        snprintf(posName, sizeof(posName), "explosionPos[%d]", i);
        snprintf(glowName, sizeof(glowName), "explosionGlow[%d]", i);
        
        if (i < (int)explosions.size()) {
            GLint ploc = glGetUniformLocation(shader.program, posName);
            if (ploc != -1) glUniform3f(ploc, explosions[i].x, explosions[i].y, explosions[i].z);
            
            GLint gloc = glGetUniformLocation(shader.program, glowName);
            if (gloc != -1) glUniform1f(gloc, (float)explosions[i].timer / 30.0f);
        } else {
            GLint gloc = glGetUniformLocation(shader.program, glowName);
            if (gloc != -1) glUniform1f(gloc, 0.0f);
        }
    }
    
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
