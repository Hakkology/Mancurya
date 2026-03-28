// 1696: Mancurya-Marsilya Artillery Battle (HD Update)
#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

const float PI = 3.1415926535f;

struct Projectile {
  float x, y, vx, vy, speed, angle;
  bool isExploded = false;
  float timeAlive = 0.0f;
};

std::vector<Projectile> mancurianBombs;
std::vector<Projectile> frenchBombs;
float mancurianShipX = -0.8f, frenchShipX = 0.8f; // Gemileri ayırdım
float shipY = -0.4f, shipWidth = 0.2f, shipHeight = 0.15f;
float gravity = -0.005f;
int explosionTimer = 0;
float explosionX, explosionY;
bool isBaguetteDepotBurning = false;
float storyTimer = 0.0f;

struct Star {
  float x, y, size;
};
std::vector<Star> stars;

struct Particle {
  float x, y, vx, vy, life;
  float r, g, b;
};
std::vector<Particle> particles;
float waveOffset = 0.0f;

void drawCircle(float cx, float cy, float r, int num_segments, float red,
                float green, float blue, float alpha = 1.0f) {
  glColor4f(red, green, blue, alpha);
  glBegin(GL_POLYGON);
  for (int i = 0; i < num_segments; i++) {
    float theta = 2.0f * PI * float(i) / float(num_segments);
    glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
  }
  glEnd();
}

void drawShip(float x, float y, float w, float h, float r, float g, float b) {
  glColor3f(r, g, b);
  glRectf(x - w / 2, y - h / 2, x + w / 2, y + h / 2);

  // Basit bir direk ekleyelim
  glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
  glRectf(x - 0.01f, y + h / 2, x + 0.01f, y + h / 2 + 0.1f);
}

void drawBaguette(float x, float y, float vx, float vy) {
  float angle = atan2(vy, vx) * 180.0 / PI;
  glPushMatrix();
  glTranslatef(x, y, 0);
  glRotatef(angle, 0, 0, 1);
  // Baget gövdesi
  glColor3f(0.8f, 0.6f, 0.3f);
  glRectf(-0.06f, -0.015f, 0.06f, 0.015f);
  // Çizikler
  glColor3f(0.6f, 0.4f, 0.2f);
  glBegin(GL_LINES);
  for(float i=-0.04f; i<=0.04f; i+=0.02f) {
      glVertex2f(i, -0.01f); glVertex2f(i+0.01f, 0.01f);
  }
  glEnd();
  glPopMatrix();
}

void drawCroissant(float x, float y) {
  drawCircle(x, y, 0.03f, 8, 0.9f, 0.7f, 0.2f); // Basit bir kruvasan şekli
  drawCircle(x+0.02f, y, 0.02f, 8, 0.9f, 0.7f, 0.2f);
  drawCircle(x-0.02f, y, 0.02f, 8, 0.9f, 0.7f, 0.2f);
}

void drawHorse(float x, float y) {
    // Bağımlı at (Basit şekil)
    glColor3f(0.4f, 0.2f, 0.1f);
    glRectf(x-0.03f, y-0.02f, x+0.03f, y+0.02f); // Gövde
    glRectf(x+0.02f, y+0.02f, x+0.05f, y+0.05f); // Boyun/Kafa
    // Ayaklar
    glBegin(GL_LINES);
    glVertex2f(x-0.02f, y-0.02f); glVertex2f(x-0.02f, y-0.04f);
    glVertex2f(x+0.02f, y-0.02f); glVertex2f(x+0.02f, y-0.04f);
    glEnd();
}

void drawBaguetteDepot(float x, float y) {
  // Ekmek deposu (Kahverengi kutu)
  glColor3f(0.6f, 0.4f, 0.2f);
  glRectf(x - 0.1f, y - 0.05f * (isBaguetteDepotBurning ? 0.5f : 1.0f), x + 0.1f, y + 0.05f);
  
  if (isBaguetteDepotBurning) {
    // Alev efekti (Kırmızı-Sarı)
    for (int i = 0; i < 5; i++) {
        drawCircle(x + (rand() % 10 - 5) / 50.0f, y + 0.05f + (rand() % 10) / 100.0f, 
                   0.03f, 8, 1.0f, 0.3f, 0.0f, 0.8f);
    }
  }
}

void drawSoySauceArrow(float x, float y, float vx, float vy) {
  float angle = atan2(vy, vx) * 180.0 / PI;
  glPushMatrix();
  glTranslatef(x, y, 0);
  glRotatef(angle, 0, 0, 1);
  
  // Ok gövdesi (Koyu Soya Sosu rengi)
  glColor3f(0.3f, 0.15f, 0.05f);
  glBegin(GL_LINES);
  glVertex2f(-0.04f, 0);
  glVertex2f(0.04f, 0);
  glEnd();
  
  // Ok ucu (Acılı kırmızı)
  glColor3f(0.8f, 0.1f, 0.0f);
  glBegin(GL_TRIANGLES);
  glVertex2f(0.04f, 0.01f);
  glVertex2f(0.04f, -0.01f);
  glVertex2f(0.06f, 0);
  glEnd();
  
  glPopMatrix();
}

void renderText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
  glRasterPos2f(x, y);
  for (const char* c = text; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

void drawExplosion(float x, float y, int timer) {
  float maxRadius = 0.2f;
  float alpha = (float)timer / 30.0f;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Katkısal (Additive) Karıştırma - BLOOM!
  for(int i=0; i<3; i++) {
    drawCircle(x, y, maxRadius * (1.0f - (float)timer / 30.0f) * (1.0f - i*0.2f), 20, 
               1.0f, 0.4f + i*0.2f, 0.0f, alpha * (1.0f - i*0.2f));
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void addSmoke(float x, float y) {
  for (int i = 0; i < 5; i++) {
    particles.push_back({x, y, (rand() % 10 - 5) / 500.0f,
                         (rand() % 10) / 300.0f, 1.0f, 0.5f, 0.5f, 0.5f});
  }
}

void update(int value) {
  // Mançurya Bombalarını Güncelle
  for (int i = 0; i < mancurianBombs.size(); i++) {
    if (!mancurianBombs[i].isExploded) {
      mancurianBombs[i].vx =
          mancurianBombs[i].speed * cos(mancurianBombs[i].angle * PI / 180.0f);
      mancurianBombs[i].vy += gravity; // Yerçekimi
      mancurianBombs[i].x += mancurianBombs[i].vx;
      mancurianBombs[i].y += mancurianBombs[i].vy;

      // Çarpışma kontrolü (Fransız amiralleri/baget depoları)
      if (mancurianBombs[i].y < shipY + shipHeight / 2 &&
          abs(mancurianBombs[i].x - frenchShipX) < shipWidth / 2) {
        mancurianBombs[i].isExploded = true;
        isBaguetteDepotBurning = true;
        explosionX = mancurianBombs[i].x;
        explosionY = mancurianBombs[i].y;
        explosionTimer = 30;
      }

      // Suya düşme kontrolü
      if (mancurianBombs[i].y < -0.5f) {
        mancurianBombs[i].isExploded = true;
        explosionX = mancurianBombs[i].x;
        explosionY = -0.5f;
        explosionTimer = 20; // Daha kısa su sıçraması
      }
    }
  }

  // Fransız Bombalarını Güncelle
  for (int i = 0; i < frenchBombs.size(); i++) {
    if (!frenchBombs[i].isExploded) {
      frenchBombs[i].vx = frenchBombs[i].speed *
                          cos((180 - frenchBombs[i].angle) * PI / 180.0f);
      frenchBombs[i].vy += gravity;
      frenchBombs[i].x += frenchBombs[i].vx;
      frenchBombs[i].y += frenchBombs[i].vy;

      if (frenchBombs[i].y < shipY + shipHeight / 2 &&
          abs(frenchBombs[i].x - mancurianShipX) < shipWidth / 2) {
        frenchBombs[i].isExploded = true;
        explosionX = frenchBombs[i].x;
        explosionY = frenchBombs[i].y;
        explosionTimer = 30;
      }

      if (frenchBombs[i].y < -0.5f) {
        frenchBombs[i].isExploded = true;
        explosionX = frenchBombs[i].x;
        explosionY = -0.5f;
        explosionTimer = 20;
      }
    }
  }

  if (explosionTimer > 0)
    explosionTimer--;

  // Parçacıkları güncelle
  for (auto &p : particles) {
    p.x += p.vx;
    p.y += p.vy;
    p.life -= 0.02f;
  }
  particles.erase(std::remove_if(particles.begin(), particles.end(),
                                 [](const Particle &p) { return p.life <= 0; }),
                  particles.end());

  waveOffset += 0.05f;
  storyTimer += 0.016f;

  // Yıldızları titret (twinkle)
  for (auto &s : stars) {
    if (rand() % 10 == 0) s.size = (rand() % 20 + 1) / 10.0f;
  }

  // Temizlik: Patlamış bombaları listeden sil
  mancurianBombs.erase(
      std::remove_if(mancurianBombs.begin(), mancurianBombs.end(),
                     [](const Projectile &b) { return b.isExploded; }),
      mancurianBombs.end());
  frenchBombs.erase(std::remove_if(frenchBombs.begin(), frenchBombs.end(),
                                   [](const Projectile &b) { return b.isExploded; }),
                    frenchBombs.end());

  glutPostRedisplay();
  glutTimerFunc(16, update, 0); // 60 FPS
}

void fireBomb(int value) {
  int fireInterval = 500; // 0.5 saniyede bir

  // Mançurya Bombası: Hedefe doğru daha parabolik atış
  float angle = 60.0f + (rand() % 10 - 5);
  float speed = 0.07f + (rand() % 10) / 1000.0f;
  mancurianBombs.push_back({mancurianShipX, shipY + 0.1f, 0, speed, speed, angle});
  addSmoke(mancurianShipX, shipY + 0.15f);

  // Fransız Bombası: Hedefe doğru daha parabolik atış
  angle = 60.0f + (rand() % 10 - 5);
  speed = 0.07f + (rand() % 10) / 1000.0f;
  frenchBombs.push_back({frenchShipX, shipY + 0.1f, 0, speed, speed, angle});
  addSmoke(frenchShipX, shipY + 0.15f);

  glutTimerFunc(fireInterval, fireBomb, 0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Arka Plan: Gradient Gökyüzü (Realism!)
  glBegin(GL_QUADS);
  glColor3f(0.02f, 0.02f, 0.1f); glVertex2f(-1, 1);  // Gece Mavisi
  glColor3f(0.02f, 0.02f, 0.1f); glVertex2f(1, 1);
  glColor3f(0.3f, 0.1f, 0.1f); glVertex2f(1, -0.5f); // Gün Batımı / Dumanlı ufk
  glColor3f(0.3f, 0.1f, 0.1f); glVertex2f(-1, -0.5f);
  glEnd();

  // Arka Plan: Yıldızlar
  glPointSize(1.0f);
  glBegin(GL_POINTS);
  for (const auto &s : stars) {
    glColor4f(1.0f, 1.0f, 1.0f, s.size / 2.0f);
    glVertex2f(s.x, s.y);
  }
  glEnd();

  // Arka Plan: Ay
  drawCircle(0.7f, 0.7f, 0.1f, 30, 0.9f, 0.9f, 0.8f);
  drawCircle(0.66f, 0.72f, 0.09f, 30, 0.1f, 0.05f, 0.1f);

  // Deniz (Yansımalar İçin Alan)
  glBegin(GL_QUAD_STRIP);
  for (float x = -1.0f; x <= 1.05f; x += 0.05f) {
    float wave = 0.02f * sinf(x * 10.0f + waveOffset);
    glColor4f(0.0f, 0.15f, 0.4f, 1.0f);
    glVertex2f(x, -0.5f + wave);
    glColor4f(0.0f, 0.05f, 0.2f, 1.0f);
    glVertex2f(x, -1.0f);
  }
  glEnd();

  // YANSIMALAR (Realism ++ )
  glPushMatrix();
  glTranslatef(0, -1.0f, 0); // Su yüzeyine göre aynalama
  glScalef(1.0f, -0.5f, 1.0f); // Ters çevir ve basıklaştır
  
  // Gemilerin Yansıması
  drawShip(mancurianShipX, shipY, shipWidth, shipHeight, 0.4f, 0.2f, 0.1f);
  drawShip(frenchShipX, shipY, shipWidth, shipHeight, 0.1f, 0.1f, 0.5f);
  drawBaguetteDepot(frenchShipX, shipY + 0.15f);
  
  glPopMatrix();

  // Duman Parçacıkları (Real Glow)
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  for (const auto &p : particles) {
    drawCircle(p.x, p.y, 0.015f, 6, p.r, p.g, p.b, p.life * 0.5f);
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Gemiler & Depolar & Atlar
  drawShip(mancurianShipX, shipY, shipWidth, shipHeight, 0.4f, 0.2f, 0.1f);
  drawHorse(mancurianShipX - 0.05f, shipY + 0.1f);
  drawCroissant(mancurianShipX + 0.05f, shipY + 0.1f);
  
  drawBaguetteDepot(frenchShipX, shipY + 0.15f);
  drawShip(frenchShipX, shipY, shipWidth, shipHeight, 0.1f, 0.1f, 0.5f);

  // Projeler
  for (const auto &b : mancurianBombs)
    if (!b.isExploded)
      drawSoySauceArrow(b.x, b.y, b.vx, b.vy);
  for (const auto &b : frenchBombs)
    if (!b.isExploded)
      drawBaguette(b.x, b.y, b.vx, b.vy);

  // Metin Katmanı
  glColor3f(1.0f, 1.0f, 0.0f);
  if (storyTimer < 5.0f) renderText(-0.4f, 0.9f, "Operasyon: Kruvasan Atesi");
  else if (storyTimer < 10.0f) renderText(-0.4f, 0.9f, "Bize her yer Mancurya!");
  else if (storyTimer < 15.0f) {
      glColor3f(1.0f, 0.5f, 0.0f);
      renderText(-0.5f, 0.9f, "Mancurya Imparatoru: Guzel de, Marsilya neresi?");
  } else if (storyTimer < 20.0f) {
      glColor3f(1.0f, 0.8f, 0.0f);
      renderText(-0.4f, 0.9f, "Atlar kruvasan bagimlisi oldu!");
  } else {
      glColor3f(1.0f, 0.0f, 0.0f);
      renderText(-0.2f, 0.9f, "Pirus Zaferi!");
  }

  // Patlama
  if (explosionTimer > 0) {
    drawExplosion(explosionX, explosionY, explosionTimer);
  }

  glutSwapBuffers();
}

void initStars() {
  for (int i = 0; i < 100; i++) {
    stars.push_back({(rand() % 200 - 100) / 100.0f,
                     (rand() % 150 - 50) / 100.0f, (rand() % 20 + 1) / 10.0f});
  }
}

int main(int argc, char **argv) {
  srand(time(0));
  initStars();
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1280, 720); // Daha büyük ekran
  glutCreateWindow("1696 Artillery Battle: Mancurya vs Marsilya (HD Update)");
  glutDisplayFunc(display);
  glutTimerFunc(1000, fireBomb, 0); // Başlangıç gecikmesi
  glutTimerFunc(16, update, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glutMainLoop();
  return 0;
}