#include <Mancurya/FleetManager.hpp>
#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace Mancurya {

void FleetManager::init() {
    projectileShader.load("../shaders/projectile.vert", "../shaders/projectile.frag");
    for (int i=0; i<8; i++) {
        float z = -1.5f + i*0.45f + (rand()%100/400.0f - 0.12f);
        mancurian.push_back({-1.6f + (rand()%100/500.0f), -0.38f, z, -1.6f, false, true});
        french.push_back({1.6f + (rand()%100/500.0f), -0.38f, z, 1.6f, false, true});
    }
    for (int i=0; i<4; i++) {
        float z = -2.5f + i*1.5f + (rand()%100/200.0f);
        mancurian.push_back({-3.5f - (rand()%100/100.0f), -0.38f, z, -3.5f, false, false});
        french.push_back({3.5f + (rand()%100/100.0f), -0.38f, z, 3.5f, false, false});
    }
}

void FleetManager::update(float storyTimer) {
    for (auto &ship : mancurian) {
        ship.x = ship.baseX + sin(storyTimer * 1.5f + ship.z) * 0.05f;
    }
    for (auto &ship : french) {
        ship.x = ship.baseX + sin(storyTimer * 1.2f + ship.z) * 0.05f;
    }

    auto processBombs = [&](std::vector<Projectile>& bombs, std::vector<Ship>& targetFleet, bool isMancurian) {
        for (auto &bomb : bombs) {
            if (bomb.isExploded) {
                continue;
            }
            bomb.vy += GRAVITY;
            bomb.x += (isMancurian ? bomb.vx : -bomb.vx);
            bomb.y += bomb.vy;
            bomb.z += bomb.vz;

            for (auto &ship : targetFleet) {
                if (bomb.y < ship.y + 0.06f && abs(bomb.x - ship.x) < 0.2f && abs(bomb.z - ship.z) < 0.2f) {
                    bomb.isExploded = true;
                    ship.isBurning = true;
                    explosions.push_back({bomb.x, bomb.y, bomb.z, 30});
                }
            }
            if (bomb.y < -0.5f) {
                bomb.isExploded = true;
                explosions.push_back({bomb.x, -0.5f, bomb.z, 20});
            }
        }
    };
    processBombs(mancurianBombs, french, true);
    processBombs(frenchBombs, mancurian, false);
    
    for (auto &explosion : explosions) {
        explosion.timer--;
    }
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.timer <= 0; }), explosions.end());
    mancurianBombs.erase(std::remove_if(mancurianBombs.begin(), mancurianBombs.end(), [](const Projectile& bomb) { return bomb.isExploded; }), mancurianBombs.end());
    frenchBombs.erase(std::remove_if(frenchBombs.begin(), frenchBombs.end(), [](const Projectile& bomb) { return bomb.isExploded; }), frenchBombs.end());
}

void FleetManager::fire() {
    auto fireSide = [&](std::vector<Ship>& sourceFleet, std::vector<Ship>& targetFleet, std::vector<Projectile>& bombs, bool isMancurian) {
        for (auto &ship : sourceFleet) {
            if (ship.isShooting && rand() % 10 < 3) {
                auto &targetShip = targetFleet[rand() % 8];
                float dx = (isMancurian ? targetShip.x - ship.x : ship.x - targetShip.x);
                float dz = targetShip.z - ship.z;
                float vy = 0.08f + (rand() % 10) / 400.0f;
                float tt = -2.0f * vy / GRAVITY;
                bombs.push_back({ship.x, ship.y + 0.1f, ship.z, dx / tt, vy, dz / tt, false});
            }
        }
    };
    fireSide(mancurian, french, mancurianBombs, true);
    fireSide(french, mancurian, frenchBombs, false);
}

void FleetManager::draw(float storyTimer) {
    auto drawFleet = [&](std::vector<Ship>& fleet, bool isFrench) {
        for (auto &ship : fleet) {
            glPushMatrix();
            glTranslatef(ship.x, ship.y, ship.z);
            drawShip(0, 0, 0.2f, 0.12f, isFrench ? 0.1f : 0.4f, isFrench ? 0.1f : 0.2f, isFrench ? 0.5f : 0.1f, isFrench);
            if (ship.isBurning) {
                glDisable(GL_LIGHTING);
                glColor4f(1, 0.3f, 0, 0.8f);
                glutSolidSphere(0.05, 8, 8);
                glEnable(GL_LIGHTING);
            }
            glPopMatrix();
        }
    };
    drawFleet(mancurian, false);
    drawFleet(french, true);
    if (!mancurian.empty()) {
        glPushMatrix();
        glTranslatef(mancurian[0].x, mancurian[0].y, mancurian[0].z);
        drawHorse(-0.1f, 0.1f);
        drawCroissant(0.05f, 0.1f);
        glPopMatrix();
    }
    if (!french.empty()) {
        glPushMatrix();
        glTranslatef(french[0].x, french[0].y, french[0].z);
        drawBaguetteDepot(0, 0.15f, french[0].isBurning);
        glPopMatrix();
    }

    projectileShader.use();
    projectileShader.setUniform1f("time", storyTimer);
    for (auto &bomb : mancurianBombs) {
        glPushMatrix();
        glTranslatef(bomb.x, bomb.y, bomb.z);
        glColor3f(0.3f, 0.2f, 0.1f);
        glutSolidCube(0.04);
        glPopMatrix();
    }
    for (auto &bomb : frenchBombs) {
        glPushMatrix();
        glTranslatef(bomb.x, bomb.y, bomb.z);
        glColor3f(0.8f, 0.6f, 0.3f);
        glutSolidCube(0.04);
        glPopMatrix();
    }
    Shader::stop();

    glDisable(GL_LIGHTING);
    for (auto &explosion : explosions) {
        glPushMatrix();
        glTranslatef(explosion.x, explosion.y, explosion.z);
        glColor3f(1, 0.5f, 0);
        glutSolidSphere(0.1f * (1.0f - explosion.timer / 30.0f), 10, 10);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

void FleetManager::drawShip(float x, float y, float w, float h, float r, float g, float b, bool isFrench) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(r, g, b);

    glPushMatrix();
    glScalef(w, h, w / 2);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.3f, 0.2f, 0.1f);
    glPushMatrix();
    glTranslatef(0, h / 2 + 0.1f, 0);
    glScalef(0.015f, 0.35f, 0.015f);
    glutSolidCube(1.0);
    glPopMatrix();

    if (isFrench) {
        glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix();
        glTranslatef(0, h / 2 + 0.15f, 0);
        glRotatef(90, 0, 1, 0);
        glScalef(0.18f, 0.25f, 0.01f);
        glutSolidCube(1.0);
        glPopMatrix();
    } else {
        glColor3f(0.7f, 0.1f, 0.1f);
        glPushMatrix();
        glTranslatef(0, h / 2 + 0.15f, 0);
        glRotatef(110, 0, 1, 0);
        glScalef(0.15f, 0.28f, 0.15f);
        glutSolidCone(1.0, 1.0, 4, 1);
        glPopMatrix();
    }

    glPopMatrix();
}

void FleetManager::drawHorse(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.1f);
    glColor3f(0.4f, 0.2f, 0.1f);

    glPushMatrix();
    glScalef(0.06f, 0.04f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.04f, 0.03f, 0);
    glScalef(0.03f, 0.03f, 0.03f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

void FleetManager::drawCroissant(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.1f);
    glColor3f(0.9f, 0.7f, 0.2f);
    glutSolidSphere(0.04f, 8, 8);
    glPopMatrix();
}

void FleetManager::drawBaguetteDepot(float x, float y, bool isBurning) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(0.6f, 0.4f, 0.2f);

    glPushMatrix();
    glScalef(0.2f, isBurning ? 0.05f : 0.1f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

}
