#include <GL/gl.h>
#include <GL/glut.h>
#include <Mancurya/Fleet.hpp>
#include <algorithm>
#include <cmath>

namespace Mancurya {

void Fleet::init(bool isFrenchSide, float baseX) {
  ships.clear();
  projectiles.clear();

  // Group 1: Standard ships (8 total) - Extreme Scatter & Height Randomization
  for (int i = 0; i < 8; i++) {
    float xVar = (rand() % 100 / 100.0f) - 0.5f; // Wider x-scatter
    float yVar =
        (rand() % 100 / 500.0f) - 0.1f; // Increased y-variance (+/- 0.1)
    float zVar = (rand() % 100 / 200.0f) - 0.25f;
    float sVar = 0.85f + (rand() % 30 / 100.0f);
    float z = -1.5f + i * 0.45f + zVar;
    ships.push_back({baseX + xVar, -0.38f + yVar, z, baseX, sVar, false, true});
  }

  // Group 2: Large ships / Background ships (10 total)
  float secondBaseX = isFrenchSide ? 3.5f : -3.5f;
  for (int i = 0; i < 10; i++) {
    float z = -4.5f + i * 1.2f + (rand() % 100 / 100.0f);
    float xVar = (rand() % 100 / 100.0f);
    float sVar = 0.9f + (rand() % 40 / 100.0f); // Random scale 0.9 to 1.3
    ships.push_back({isFrenchSide ? (secondBaseX + xVar) : (secondBaseX - xVar),
                     -0.38f, z, secondBaseX, sVar, false, false});
  }

  // Group 3: Distant ships (6 total)
  float thirdBaseX = isFrenchSide ? 5.5f : -5.5f;
  for (int i = 0; i < 6; i++) {
    float z = -6.0f + i * 2.5f + (rand() % 100 / 50.0f);
    float xVar = (rand() % 100 / 50.0f);
    float sVar = 1.0f + (rand() % 50 / 100.0f); // Random scale 1.0 to 1.5
    ships.push_back({isFrenchSide ? (thirdBaseX + xVar) : (thirdBaseX - xVar),
                     -0.38f, z, thirdBaseX, sVar, false, false});
  }
}

void Fleet::update(float storyTimer, std::vector<Explosion> &explosions,
                   std::vector<Ship> &targetShips, bool isMancurian) {
  const float shipSpeed = isMancurian ? 1.5f : 1.2f;
  for (auto &ship : ships) {
    ship.x = ship.baseX + sin(storyTimer * shipSpeed + ship.z) * 0.05f;
  }

  for (auto &bomb : projectiles) {
    if (bomb.isExploded)
      continue;

    bomb.vy += GRAVITY;
    bomb.x += (isMancurian ? bomb.vx : -bomb.vx);
    bomb.y += bomb.vy;
    bomb.z += bomb.vz;

    for (auto &targetShip : targetShips) {
      if (bomb.y < targetShip.y + 0.06f &&
          std::abs(bomb.x - targetShip.x) < 0.2f &&
          std::abs(bomb.z - targetShip.z) < 0.2f) {
        bomb.isExploded = true;
        targetShip.isBurning = true;
        explosions.push_back({bomb.x, bomb.y, bomb.z, 30});
        break;
      }
    }

    if (bomb.y < -0.5f && !bomb.isExploded) {
      bomb.isExploded = true;
      explosions.push_back({bomb.x, -0.5f, bomb.z, 20});
    }
  }

  projectiles.erase(
      std::remove_if(projectiles.begin(), projectiles.end(),
                     [](const Projectile &p) { return p.isExploded; }),
      projectiles.end());
}

void Fleet::fire(std::vector<Ship> &targetShips, bool isMancurian) {
  if (targetShips.empty())
    return;
  for (auto &ship : ships) {
    if (ship.isShooting && rand() % 10 < 3) {
      const auto &targetShip = targetShips[rand() % targetShips.size()];
      float dx = (isMancurian ? targetShip.x - ship.x : ship.x - targetShip.x);
      float dz = targetShip.z - ship.z;
      float vy = 0.08f + (rand() % 10) / 400.0f;
      float timeToTarget = -2.0f * vy / GRAVITY;
      projectiles.push_back({ship.x, ship.y + 0.1f, ship.z, dx / timeToTarget,
                             vy, dz / timeToTarget, false});
    }
  }
}

void Fleet::draw(bool isFrench) {
  for (auto &ship : ships) {
    glPushMatrix();
    glTranslatef(ship.x, ship.y, ship.z);
    glScalef(ship.scale, ship.scale, ship.scale); // Individual ship scaling
    drawShip(0, 0, 0.2f, 0.12f, isFrench ? 0.1f : 0.4f, isFrench ? 0.1f : 0.2f,
             isFrench ? 0.5f : 0.1f, isFrench);

    if (ship.isBurning) {
      glDisable(GL_LIGHTING);
      glColor4f(1.0f, 0.3f, 0.0f, 0.8f);
      glutSolidSphere(0.05, 8, 8);
      glEnable(GL_LIGHTING);
    }
    glPopMatrix();
  }

  if (!ships.empty()) {
    glPushMatrix();
    glTranslatef(ships[0].x, ships[0].y, ships[0].z);
    glScalef(ships[0].scale, ships[0].scale, ships[0].scale);
    if (isFrench) {
      drawBaguetteDepot(0, 0.15f, ships[0].isBurning);
    } else {
      drawHorse(-0.1f, 0.1f);
      drawCroissant(0.05f, 0.1f);
    }
    glPopMatrix();
  }

  for (auto &bomb : projectiles) {
    glPushMatrix();
    glTranslatef(bomb.x, bomb.y, bomb.z);
    if (isFrench)
      glColor3f(0.8f, 0.6f, 0.3f);
    else
      glColor3f(0.3f, 0.2f, 0.1f);
    glutSolidCube(0.04);
    glPopMatrix();
  }
}

void Fleet::drawShip(float x, float y, float w, float h, float r, float g,
                     float b, bool isFrench) {
  glPushMatrix();
  glTranslatef(x, y, 0);

  // WOOD COLOR FOR HULL
  glColor3f(0.35f, 0.22f, 0.12f);

  float bh = h / 2.0f; // half height
  float bd = w / 4.0f; // beam (half depth)

  // CUSTOM HULL PROFILE (Organic Ship Shape)
  // Points: P0(Bow), P1(Mid-Right), P2(Back-Right), P3(Stern), P4(Back-Left),
  // P5(Mid-Left)
  float px[] = {w * 0.65f,  w * 0.25f,  -w * 0.35f,
                -w * 0.55f, -w * 0.35f, w * 0.25f};
  float pz[] = {0.0f, bd, bd * 0.8f, 0.0f, -bd * 0.8f, -bd};

  // 1. TOP DECK
  glBegin(GL_POLYGON);
  glNormal3f(0, 1, 0);
  for (int i = 0; i < 6; i++)
    glVertex3f(px[i], bh, pz[i]);
  glEnd();

  // 2. BOTTOM HULL (Slightly narrower/tapered)
  glBegin(GL_POLYGON);
  glNormal3f(0, -1, 0);
  for (int i = 0; i < 6; i++)
    glVertex3f(px[i] * 0.95f, -bh, pz[i] * 0.7f);
  glEnd();

  // 3. HULL SIDES
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= 6; i++) {
    int idx = i % 6;
    // Simple normal calc (approximate)
    glNormal3f(px[idx], 0, pz[idx]);
    glVertex3f(px[idx], bh, pz[idx]);
    glVertex3f(px[idx] * 0.95f, -bh, pz[idx] * 0.7f);
  }
  glEnd();

  // 4. DECK CABIN (Kamara) - Lighter Wood
  glColor3f(0.45f, 0.30f, 0.18f);
  glPushMatrix();
  glTranslatef(-w * 0.15f, bh + 0.01f, 0);
  glScalef(w * 0.3f, h * 0.6f, w * 0.35f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 5. MAST (Direk)
  glColor3f(0.2f, 0.1f, 0.05f);
  glPushMatrix();
  glTranslatef(0.1f, bh + 0.15f, 0);
  glScalef(0.015f, 0.45f, 0.015f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 6. SAILS / TENTS (Team Colors)
  if (isFrench) {
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(0.1f, bh + 0.20f, 0);
    glRotatef(90, 0, 1, 0);
    glScalef(0.20f, 0.32f, 0.01f);
    glutSolidCube(1.0);
    glPopMatrix();
  } else {
    glColor3f(0.7f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.1f, bh + 0.18f, 0);
    glRotatef(110, 0, 1, 0);
    glScalef(0.15f, 0.35f, 0.15f);
    glutSolidCone(1.0, 1.0, 4, 1);
    glPopMatrix();
  }
  glPopMatrix();
}

void Fleet::drawHorse(float x, float y) {
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

void Fleet::drawCroissant(float x, float y) {
  glPushMatrix();
  glTranslatef(x, y, 0.1f);
  glColor3f(0.9f, 0.7f, 0.2f);
  glutSolidSphere(0.04f, 8, 8);
  glPopMatrix();
}

void Fleet::drawBaguetteDepot(float x, float y, bool isBurning) {
  glPushMatrix();
  glTranslatef(x, y, 0);
  glColor3f(0.6f, 0.4f, 0.2f);
  glPushMatrix();
  glScalef(0.2f, isBurning ? 0.05f : 0.1f, 0.1f);
  glutSolidCube(1.0f);
  glPopMatrix();
  glPopMatrix();
}

} // namespace Mancurya
