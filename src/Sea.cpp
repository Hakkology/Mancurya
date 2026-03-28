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

void Sea::draw(const std::vector<Explosion>& explosions, const std::vector<Ship>& ships, float storyTimer) {
    shader.use();
    shader.setUniform1f("time", storyTimer);
    
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

    // Pass Ship Data for Shadows (Initializing all 64 slots for stability)
    int sCount = (int)std::min((size_t)64, ships.size());
    shader.setUniform1i("shipCount", sCount);
    shader.setUniform1f("storyTimer", storyTimer);
    for (int i = 0; i < 64; i++) {
        char posName[32], scaleName[32];
        snprintf(posName, sizeof(posName), "shipPos[%d]", i);
        snprintf(scaleName, sizeof(scaleName), "shipScale[%d]", i);
        
        if (i < sCount) {
            shader.setUniform3f(posName, ships[i].x, ships[i].y, ships[i].z);
            shader.setUniform1f(scaleName, ships[i].scale);
        } else {
            shader.setUniform3f(posName, 0.0f, -100.0f, 0.0f);
            shader.setUniform1f(scaleName, 1.0f);
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
