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
    for (auto &s : mancurian) s.x = s.baseX + sin(storyTimer * 1.5f + s.z) * 0.05f;
    for (auto &s : french) s.x = s.baseX + sin(storyTimer * 1.2f + s.z) * 0.05f;

    auto processBombs = [&](std::vector<Projectile>& bs, std::vector<Ship>& targetF, bool isM) {
        for (auto &b : bs) {
            if (b.isExploded) continue;
            b.vy += GRAVITY; b.x += (isM?b.vx:-b.vx); b.y += b.vy; b.z += b.vz;
            for (auto &s : targetF) {
                if (b.y < s.y + 0.06f && abs(b.x - s.x) < 0.2f && abs(b.z - s.z) < 0.2f) {
                    b.isExploded = true; s.isBurning = true; explosions.push_back({b.x, b.y, b.z, 30});
                }
            }
            if (b.y < -0.5f) { b.isExploded = true; explosions.push_back({b.x, -0.5f, b.z, 20}); }
        }
    };
    processBombs(mBombs, french, true); processBombs(fBombs, mancurian, false);
    
    for (auto &e : explosions) e.timer--;
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& e){ return e.timer <= 0; }), explosions.end());
    mBombs.erase(std::remove_if(mBombs.begin(), mBombs.end(), [](const Projectile& b){ return b.isExploded; }), mBombs.end());
    fBombs.erase(std::remove_if(fBombs.begin(), fBombs.end(), [](const Projectile& b){ return b.isExploded; }), fBombs.end());
}

void FleetManager::fire() {
    auto fireSide = [&](std::vector<Ship>& side, std::vector<Ship>& targets, std::vector<Projectile>& bs, bool isM) {
        for (auto &s : side) {
            if (s.isShooting && rand()%10 < 3) {
                auto &t = targets[rand()%8];
                float dx = (isM ? t.x-s.x : s.x-t.x), dz = t.z-s.z, vy = 0.08f + (rand()%10)/400.0f, tt = -2.0f*vy/GRAVITY;
                bs.push_back({s.x, s.y+0.1f, s.z, dx/tt, vy, dz/tt, false});
            }
        }
    };
    fireSide(mancurian, french, mBombs, true); fireSide(french, mancurian, fBombs, false);
}

void FleetManager::draw(float storyTimer) {
    auto drawFleet = [&](std::vector<Ship>& f, bool isF) {
        for (auto &s : f) {
            glPushMatrix(); glTranslatef(s.x, s.y, s.z);
            drawShip(0, 0, 0.2f, 0.12f, isF?0.1f:0.4f, isF?0.1f:0.2f, isF?0.5f:0.1f, isF);
            if (s.isBurning) { glDisable(GL_LIGHTING); glColor4f(1, 0.3f, 0, 0.8f); glutSolidSphere(0.05, 8, 8); glEnable(GL_LIGHTING); }
            glPopMatrix();
        }
    };
    drawFleet(mancurian, false); drawFleet(french, true);
    if (!mancurian.empty()) { glPushMatrix(); glTranslatef(mancurian[0].x, mancurian[0].y, mancurian[0].z); drawHorse(-0.1f, 0.1f); drawCroissant(0.05f, 0.1f); glPopMatrix(); }
    if (!french.empty()) { glPushMatrix(); glTranslatef(french[0].x, french[0].y, french[0].z); drawBaguetteDepot(0, 0.15f, french[0].isBurning); glPopMatrix(); }

    projectileShader.use(); projectileShader.setUniform1f("time", storyTimer);
    for (auto &b : mBombs) { glPushMatrix(); glTranslatef(b.x, b.y, b.z); glColor3f(0.3f, 0.2f, 0.1f); glutSolidCube(0.04); glPopMatrix(); }
    for (auto &b : fBombs) { glPushMatrix(); glTranslatef(b.x, b.y, b.z); glColor3f(0.8f, 0.6f, 0.3f); glutSolidCube(0.04); glPopMatrix(); }
    Shader::stop();

    glDisable(GL_LIGHTING);
    for (auto &e : explosions) { glPushMatrix(); glTranslatef(e.x, e.y, e.z); glColor3f(1, 0.5f, 0); glutSolidSphere(0.1f * (1.0f - e.timer/30.0f), 10, 10); glPopMatrix(); }
    glEnable(GL_LIGHTING);
}

void FleetManager::drawShip(float x, float y, float w, float h, float r, float g, float b, bool isF) {
    glPushMatrix(); glTranslatef(x, y, 0); glColor3f(r, g, b);
    glPushMatrix(); glScalef(w, h, w/2); glutSolidCube(1.0); glPopMatrix();
    glColor3f(0.3f, 0.2f, 0.1f); glPushMatrix(); glTranslatef(0, h/2 + 0.1f, 0); glScalef(0.015f, 0.35f, 0.015f); glutSolidCube(1.0); glPopMatrix();
    if (isF) { glColor3f(0.9f, 0.9f, 0.9f); glPushMatrix(); glTranslatef(0, h/2 + 0.15f, 0); glRotatef(90, 0, 1, 0); glScalef(0.18f, 0.25f, 0.01f); glutSolidCube(1.0); glPopMatrix(); }
    else { glColor3f(0.7f, 0.1f, 0.1f); glPushMatrix(); glTranslatef(0, h/2 + 0.15f, 0); glRotatef(110, 0, 1, 0); glScalef(0.15f, 0.28f, 0.15f); glutSolidCone(1.0, 1.0, 4, 1); glPopMatrix(); }
    glPopMatrix();
}

void FleetManager::drawHorse(float x, float y) {
    glPushMatrix(); glTranslatef(x, y, 0.1f); glColor3f(0.4f, 0.2f, 0.1f);
    glPushMatrix(); glScalef(0.06f, 0.04f, 0.04f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.04f, 0.03f, 0); glScalef(0.03f, 0.03f, 0.03f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void FleetManager::drawCroissant(float x, float y) {
    glPushMatrix(); glTranslatef(x, y, 0.1f); glColor3f(0.9f, 0.7f, 0.2f); glutSolidSphere(0.04f, 8, 8); glPopMatrix();
}

void FleetManager::drawBaguetteDepot(float x, float y, bool isBurning) {
    glPushMatrix(); glTranslatef(x, y, 0); glColor3f(0.6f, 0.4f, 0.2f);
    glPushMatrix(); glScalef(0.2f, isBurning ? 0.05f : 0.1f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

}
