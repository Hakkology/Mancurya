#include <Mancurya/Fleet.hpp>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>
#include <algorithm>

namespace Mancurya {

void Fleet::init(bool isFrenchSide, float baseX) {
    ships.clear();
    projectiles.clear();
    
    // Group 1: Standard ships (8 total)
    for (int i=0; i<8; i++) {
        float z = -1.5f + i*0.45f + (rand()%100/400.0f - 0.12f);
        ships.push_back({baseX + (rand()%100/500.0f), -0.38f, z, baseX, false, true});
    }
    
    // Group 2: Large ships / Background ships (4 total)
    float secondBaseX = isFrenchSide ? 3.5f : -3.5f;
    for (int i=0; i<4; i++) {
        float z = -2.5f + i*1.5f + (rand()%100/200.0f);
        float xOffset = (rand()%100/100.0f);
        ships.push_back({isFrenchSide ? (secondBaseX + xOffset) : (secondBaseX - xOffset), -0.38f, z, secondBaseX, false, false});
    }
}

void Fleet::update(float storyTimer, std::vector<Explosion>& explosions, std::vector<Ship>& targetShips, bool isMancurian) {
    const float shipSpeed = isMancurian ? 1.5f : 1.2f;
    for (auto &ship : ships) {
        ship.x = ship.baseX + sin(storyTimer * shipSpeed + ship.z) * 0.05f;
    }

    for (auto &bomb : projectiles) {
        if (bomb.isExploded) continue;
        
        bomb.vy += GRAVITY;
        bomb.x += (isMancurian ? bomb.vx : -bomb.vx);
        bomb.y += bomb.vy;
        bomb.z += bomb.vz;

        // Collision logic optimized for clarity and performance
        for (auto &targetShip : targetShips) {
            if (bomb.y < targetShip.y + 0.06f && 
                std::abs(bomb.x - targetShip.x) < 0.2f && 
                std::abs(bomb.z - targetShip.z) < 0.2f) {
                bomb.isExploded = true;
                targetShip.isBurning = true;
                explosions.push_back({bomb.x, bomb.y, bomb.z, 30});
                break; // One bomb, one hit
            }
        }
        
        if (bomb.y < -0.5f && !bomb.isExploded) {
            bomb.isExploded = true;
            explosions.push_back({bomb.x, -0.5f, bomb.z, 20});
        }
    }

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), 
        [](const Projectile& p) { return p.isExploded; }), projectiles.end());
}

void Fleet::fire(std::vector<Ship>& targetShips, bool isMancurian) {
    if (targetShips.empty()) return;
    for (auto &ship : ships) {
        // Shooting logic dynamic to target fleet size
        if (ship.isShooting && rand() % 10 < 3) {
            const auto &targetShip = targetShips[rand() % targetShips.size()];
            float dx = (isMancurian ? targetShip.x - ship.x : ship.x - targetShip.x);
            float dz = targetShip.z - ship.z;
            float vy = 0.08f + (rand() % 10) / 400.0f;
            float timeToTarget = -2.0f * vy / GRAVITY;
            projectiles.push_back({ship.x, ship.y + 0.1f, ship.z, dx / timeToTarget, vy, dz / timeToTarget, false});
        }
    }
}

void Fleet::draw(bool isFrench) {
    for (auto &ship : ships) {
        glPushMatrix();
        glTranslatef(ship.x, ship.y, ship.z);
        drawShip(0, 0, 0.2f, 0.12f, 
                 isFrench ? 0.1f : 0.4f, 
                 isFrench ? 0.1f : 0.2f, 
                 isFrench ? 0.5f : 0.1f, 
                 isFrench);
        
        if (ship.isBurning) {
            glDisable(GL_LIGHTING);
            glColor4f(1.0f, 0.3f, 0.0f, 0.8f);
            glutSolidSphere(0.05, 8, 8);
            glEnable(GL_LIGHTING);
        }
        glPopMatrix();
    }

    // Special items for the "flagship" (first ship in list)
    if (!ships.empty()) {
        glPushMatrix();
        glTranslatef(ships[0].x, ships[0].y, ships[0].z);
        if (isFrench) {
            drawBaguetteDepot(0, 0.15f, ships[0].isBurning);
        } else {
            drawHorse(-0.1f, 0.1f);
            drawCroissant(0.05f, 0.1f);
        }
        glPopMatrix();
    }

    // Bomb drawing loop
    for (auto &bomb : projectiles) {
        glPushMatrix();
        glTranslatef(bomb.x, bomb.y, bomb.z);
        if (isFrench) glColor3f(0.8f, 0.6f, 0.3f);
        else glColor3f(0.3f, 0.2f, 0.1f);
        glutSolidCube(0.04);
        glPopMatrix();
    }
}

void Fleet::drawShip(float x, float y, float w, float h, float r, float g, float b, bool isFrench) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(r, g, b);
    glPushMatrix(); glScalef(w, h, w / 2); glutSolidCube(1.0); glPopMatrix();
    glColor3f(0.3f, 0.2f, 0.1f);
    glPushMatrix(); glTranslatef(0, h / 2 + 0.1f, 0); glScalef(0.015f, 0.35f, 0.015f); glutSolidCube(1.0); glPopMatrix();
    if (isFrench) {
        glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix(); glTranslatef(0, h / 2 + 0.15f, 0); glRotatef(90, 0, 1, 0); glScalef(0.18f, 0.25f, 0.01f); glutSolidCube(1.0); glPopMatrix();
    } else {
        glColor3f(0.7f, 0.1f, 0.1f);
        glPushMatrix(); glTranslatef(0, h / 2 + 0.15f, 0); glRotatef(110, 0, 1, 0); glScalef(0.15f, 0.28f, 0.15f); glutSolidCone(1.0, 1.0, 4, 1); glPopMatrix();
    }
    glPopMatrix();
}

void Fleet::drawHorse(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.1f);
    glColor3f(0.4f, 0.2f, 0.1f);
    glPushMatrix(); glScalef(0.06f, 0.04f, 0.04f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.04f, 0.03f, 0); glScalef(0.03f, 0.03f, 0.03f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void Fleet::drawCroissant(float x, float y) {
    glPushMatrix(); glTranslatef(x, y, 0.1f); glColor3f(0.9f, 0.7f, 0.2f); glutSolidSphere(0.04f, 8, 8); glPopMatrix();
}

void Fleet::drawBaguetteDepot(float x, float y, bool isBurning) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(0.6f, 0.4f, 0.2f);
    glPushMatrix(); glScalef(0.2f, isBurning ? 0.05f : 0.1f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

}
