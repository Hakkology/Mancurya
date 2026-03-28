#include <Mancurya/FleetManager.hpp>
#include <GL/glut.h>
#include <algorithm>

namespace Mancurya {

void FleetManager::init() {
    projectileShader.load("shaders/projectile.vert", "shaders/projectile.frag");
    
    // Grouped initialization logic
    mancurianFleet.init(false, -1.6f);
    frenchFleet.init(true, 1.6f);
    
    explosions.clear();
}

void FleetManager::update(float storyTimer) {
    mancurianFleet.update(storyTimer, explosions, frenchFleet.ships, true);
    frenchFleet.update(storyTimer, explosions, mancurianFleet.ships, false);

    for (auto &explosion : explosions) {
        explosion.timer--;
    }
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), 
        [](const Explosion& e) { return e.timer <= 0; }), explosions.end());
}

void FleetManager::fire() {
    mancurianFleet.fire(frenchFleet.ships, true);
    frenchFleet.fire(mancurianFleet.ships, false);
}

void FleetManager::draw(float storyTimer) {
    mancurianFleet.draw(false, storyTimer);
    frenchFleet.draw(true, storyTimer);

    projectileShader.use();
    projectileShader.setUniform1f("time", storyTimer);
    Shader::stop();

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glow
    for (auto &e : explosions) {
        glPushMatrix();
        glTranslatef(e.x, e.y, e.z);
        float r = 0.3f * (1.0f - (float)e.timer / 30.0f);
        glColor4f(1.0f, 0.6f, 0.2f, (float)e.timer / 30.0f);
        glutSolidSphere(r, 12, 12);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

}
