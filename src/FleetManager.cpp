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
    mancurianFleet.draw(false);
    frenchFleet.draw(true);

    projectileShader.use();
    projectileShader.setUniform1f("time", storyTimer);
    Shader::stop();

    glDisable(GL_LIGHTING);
    for (auto &e : explosions) {
        glPushMatrix();
        glTranslatef(e.x, e.y, e.z);
        glColor3f(1.0f, 0.5f, 0.0f);
        glutSolidSphere(0.1f * (1.0f - e.timer / 30.0f), 10, 10);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

}
