#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

// --- Manuel Shader yükleme sistemi ---
// Not: Modern sistemlerde 'GLEW' veya 'GLAD' gibi kütüphaneler bu işi otomatik yapar.
// Ancak sistemde kütüphane olmadığı için, ekran kartı sürücüsünden bu fonksiyonların 
// adresini manuel olarak ("glXGetProcAddressARB" ile) çekiyoruz.

// APIENTRY: Bu bir 'Calling Convention' makrosudur.
// Fonksiyonların işlemci seviyesinde hangi sırada ve nasıl çağrılacağını (stack yönetimi vb.) belirtir.
// OpenGL'in ekran kartı sürücüsüyle 'aynı dili' konuşmasını sağlar, uyumsuzlukları önler.

typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);

PFNGLCREATESHADERPROC glCreateShader_ptr;
PFNGLSHADERSOURCEPROC glShaderSource_ptr;
PFNGLCOMPILESHADERPROC glCompileShader_ptr;
PFNGLCREATEPROGRAMPROC glCreateProgram_ptr;
PFNGLATTACHSHADERPROC glAttachShader_ptr;
PFNGLLINKPROGRAMPROC glLinkProgram_ptr;
PFNGLUSEPROGRAMPROC glUseProgram_ptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_ptr;
PFNGLUNIFORM1FPROC glUniform1f_ptr;

GLuint projectileShader;
GLuint waterShader;
GLint projTimeLoc;
GLint waterTimeLoc;

std::string readShaderFile(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint createShaderProgram(const char* vPath, const char* fPath) {
    if (!glCreateShader_ptr) return 0;

    std::string vSourceStr = readShaderFile(vPath);
    std::string fSourceStr = readShaderFile(fPath);
    const char* vSource = vSourceStr.c_str();
    const char* fSource = fSourceStr.c_str();

    GLuint vs = glCreateShader_ptr(GL_VERTEX_SHADER);
    glShaderSource_ptr(vs, 1, &vSource, NULL);
    glCompileShader_ptr(vs);

    GLuint fs = glCreateShader_ptr(GL_FRAGMENT_SHADER);
    glShaderSource_ptr(fs, 1, &fSource, NULL);
    glCompileShader_ptr(fs);

    GLuint program = glCreateProgram_ptr();
    glAttachShader_ptr(program, vs);
    glAttachShader_ptr(program, fs);
    glLinkProgram_ptr(program);
    return program;
}

void loadShaders() {
    glCreateShader_ptr = (PFNGLCREATESHADERPROC)glXGetProcAddressARB((const GLubyte*)"glCreateShader");
    glShaderSource_ptr = (PFNGLSHADERSOURCEPROC)glXGetProcAddressARB((const GLubyte*)"glShaderSource");
    glCompileShader_ptr = (PFNGLCOMPILESHADERPROC)glXGetProcAddressARB((const GLubyte*)"glCompileShader");
    glCreateProgram_ptr = (PFNGLCREATEPROGRAMPROC)glXGetProcAddressARB((const GLubyte*)"glCreateProgram");
    glAttachShader_ptr = (PFNGLATTACHSHADERPROC)glXGetProcAddressARB((const GLubyte*)"glAttachShader");
    glLinkProgram_ptr = (PFNGLLINKPROGRAMPROC)glXGetProcAddressARB((const GLubyte*)"glLinkProgram");
    glUseProgram_ptr = (PFNGLUSEPROGRAMPROC)glXGetProcAddressARB((const GLubyte*)"glUseProgram");
    glGetUniformLocation_ptr = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddressARB((const GLubyte*)"glGetUniformLocation");
    glUniform1f_ptr = (PFNGLUNIFORM1FPROC)glXGetProcAddressARB((const GLubyte*)"glUniform1f");

    if (!glCreateShader_ptr) return;

    projectileShader = createShaderProgram("../shaders/projectile.vert", "../shaders/projectile.frag");
    waterShader = createShaderProgram("../shaders/water.vert", "../shaders/water.frag");

    if (glGetUniformLocation_ptr) {
        projTimeLoc = glGetUniformLocation_ptr(projectileShader, "time");
        waterTimeLoc = glGetUniformLocation_ptr(waterShader, "time");
    }
}
// ----------------------

const float PI = 3.1415926535f;

struct Ship {
    float x, y, z;
    bool isBurning = false;
};

std::vector<Ship> mancurianFleet;
std::vector<Ship> frenchFleet;

struct Projectile {
  float x, y, z;
  float vx, vy, vz;
  bool isExploded = false;
};

std::vector<Projectile> mancurianBombs;
std::vector<Projectile> frenchBombs;

struct Explosion {
    float x, y, z;
    int timer;
};
std::vector<Explosion> activeExplosions;

float shipWidth = 0.2f, shipHeight = 0.12f;
float shipY = -0.4f;
float gravity = -0.003f;
bool isBaguetteDepotBurning = false;
float storyTimer = 0.0f;

void initFleets() {
    mancurianFleet.clear();
    frenchFleet.clear();
    for (int i = 0; i < 4; i++) {
        float zPos = -1.2f + i * 0.8f + (rand()%100/200.0f - 0.25f);
        float xOff = (rand()%100/500.0f - 0.1f);
        mancurianFleet.push_back({-1.5f + xOff, shipY, zPos});
        
        zPos = -1.2f + i * 0.8f + (rand()%100/200.0f - 0.25f);
        xOff = (rand()%100/500.0f - 0.1f);
        frenchFleet.push_back({1.5f + xOff, shipY, zPos});
    }
}

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

void drawShip(float x, float y, float w, float h, float r, float g, float b, bool isFrench) {
  glPushMatrix();
  glTranslatef(x, y, 0);
  glColor3f(r, g, b);
  
  // Gövde (Hull)
  glPushMatrix();
  glScalef(w, h, w/2);
  glutSolidCube(1.0);
  glPopMatrix();

  // Direklere daha derin renk
  glColor3f(0.3f, 0.2f, 0.1f);
  glPushMatrix();
  glTranslatef(0, h/2 + 0.1f, 0);
  glScalef(0.015f, 0.35f, 0.015f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Yelkenler (Sails)
  if (isFrench) {
    glColor4f(0.9f, 0.9f, 0.9f, 0.9f); // Beyaz yelken
    glPushMatrix();
    glTranslatef(0, h/2 + 0.15f, 0);
    glRotatef(90, 0, 1, 0);
    glScalef(0.18f, 0.25f, 0.01f);
    glutSolidCube(1.0);
    glPopMatrix();
  } else {
    glColor4f(0.7f, 0.1f, 0.1f, 0.9f); // Kırmızı Mançurya yelkeni
    glPushMatrix();
    glTranslatef(0, h/2 + 0.15f, 0);
    glRotatef(110, 0, 1, 0);
    glScalef(0.15f, 0.28f, 0.15f);
    glutSolidCone(1.0, 1.0, 4, 1);
    glPopMatrix();
  }

  glPopMatrix();
}

void drawBaguetteDepot(float x, float y) {
  glPushMatrix();
  glTranslatef(x, y, 0);
  glColor3f(0.6f, 0.4f, 0.2f);
  glPushMatrix();
  glScalef(0.2f, isBaguetteDepotBurning ? 0.05f : 0.1f, 0.1f);
  glutSolidCube(1.0f);
  glPopMatrix();
  
  if (isBaguetteDepotBurning) {
    glDisable(GL_LIGHTING);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef((rand() % 10 - 5) / 50.0f, 0.05f + (rand() % 10) / 100.0f, 0);
        drawCircle(0, 0, 0.03f, 8, 1.0f, 0.3f, 0.0f, 0.8f);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
  }
  glPopMatrix();
}

void drawBaguette(float x, float y, float vx, float vy) {
  float angle = atan2(vy, vx) * 180.0 / PI;
  glPushMatrix();
  glTranslatef(x, y, 0);
  glRotatef(angle, 0, 0, 1);
  glColor3f(0.8f, 0.6f, 0.3f);
  glScalef(0.12f, 0.03f, 0.03f);
  glutSolidCube(1.0f);
  glPopMatrix();
}

void drawCroissant(float x, float y) {
  glPushMatrix();
  glTranslatef(x, y, 0.1f);
  glColor3f(0.9f, 0.7f, 0.2f);
  glutSolidSphere(0.04f, 8, 8);
  glPopMatrix();
}

void drawHorse(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.1f);
    glColor3f(0.4f, 0.2f, 0.1f);
    glPushMatrix(); // Gövde
    glScalef(0.06f, 0.04f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix(); // Kafa
    glTranslatef(0.04f, 0.03f, 0);
    glScalef(0.03f, 0.03f, 0.03f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();
}

void drawSoySauceArrow(float x, float y, float vx, float vy) {
  float angle = atan2(vy, vx) * 180.0 / PI;
  glPushMatrix();
  glTranslatef(x, y, 0);
  glRotatef(angle, 0, 0, 1);
  
  glColor3f(0.3f, 0.15f, 0.05f);
  glPushMatrix(); // Gövde
  glScalef(0.1f, 0.01f, 0.01f);
  glutSolidCube(1.0f);
  glPopMatrix();
  
  glColor3f(0.8f, 0.1f, 0.0f);
  glPushMatrix(); // Uç
  glTranslatef(0.05f, 0, 0);
  glRotatef(90, 0, 1, 0);
  glutSolidCone(0.02, 0.04, 8, 1);
  glPopMatrix();
  
  glPopMatrix();
}

void renderText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
  glDisable(GL_LIGHTING); // Metin etkilenmesin
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glRasterPos2f(x, y);
  for (const char* c = text; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
  
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_LIGHTING);
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
  storyTimer += 0.016f;
  waveOffset += 0.01f;

  for (auto &b : mancurianBombs) {
    if (!b.isExploded) {
      b.vy += gravity; 
      b.x += b.vx;
      b.y += b.vy;
      b.z += b.vz;

      for (auto &ship : frenchFleet) {
          if (b.y < ship.y + shipHeight/2 && abs(b.x - ship.x) < 0.2f && abs(b.z - ship.z) < 0.2f) {
            b.isExploded = true;
            ship.isBurning = true;
            activeExplosions.push_back({b.x, b.y, b.z, 30});
          }
      }
      if (b.y < -0.5f) {
          b.isExploded = true;
          activeExplosions.push_back({b.x, -0.5f, b.z, 20});
      }
    }
  }

  for (auto &b : frenchBombs) {
    if (!b.isExploded) {
      b.vy += gravity;
      b.x -= b.vx; 
      b.y += b.vy;
      b.z += b.vz;

      for (auto &ship : mancurianFleet) {
          if (b.y < ship.y + shipHeight/2 && abs(b.x - ship.x) < 0.2f && abs(b.z - ship.z) < 0.2f) {
            b.isExploded = true;
            ship.isBurning = true;
            activeExplosions.push_back({b.x, b.y, b.z, 30});
          }
      }
      if (b.y < -0.5f) {
          b.isExploded = true;
          activeExplosions.push_back({b.x, -0.5f, b.z, 20});
      }
    }
  }

  for (int i = 0; i < activeExplosions.size(); i++) {
      activeExplosions[i].timer--;
  }
  activeExplosions.erase(std::remove_if(activeExplosions.begin(), activeExplosions.end(), 
      [](const Explosion& e){ return e.timer <= 0; }), activeExplosions.end());

  for (auto &p : particles) {
    p.x += p.vx; p.y += p.vy; p.life -= 0.02f;
  }
  particles.erase(std::remove_if(particles.begin(), particles.end(),
                                 [](const Particle &p) { return p.life <= 0; }),
                  particles.end());

  mancurianBombs.erase(std::remove_if(mancurianBombs.begin(), mancurianBombs.end(),
                                     [](const Projectile &b) { return b.isExploded; }),
                       mancurianBombs.end());
  frenchBombs.erase(std::remove_if(frenchBombs.begin(), frenchBombs.end(),
                                   [](const Projectile &b) { return b.isExploded; }),
                    frenchBombs.end());

  glutPostRedisplay();
  glutTimerFunc(16, update, 0);
}

void fireBomb(int value) {
  int fireInterval = 800; 

  for (const auto &s : mancurianFleet) {
      if (rand() % 10 < 3) {
          auto &target = frenchFleet[rand() % frenchFleet.size()];
          float dx = target.x - s.x;
          float dz = target.z - s.z;
          float vy = 0.08f + (rand()%10)/500.0f;
          float travelTime = -2.0f * vy / gravity;
          mancurianBombs.push_back({s.x, s.y + 0.1f, s.z, dx/travelTime, vy, dz/travelTime, false});
          addSmoke(s.x, s.y + 0.15f);
      }
  }

  for (const auto &s : frenchFleet) {
      if (rand() % 10 < 3) {
          auto &target = mancurianFleet[rand() % mancurianFleet.size()];
          float dx = s.x - target.x; 
          float dz = target.z - s.z;
          float vy = 0.08f + (rand()%10)/500.0f;
          float travelTime = -2.0f * vy / gravity;
          frenchBombs.push_back({s.x, s.y + 0.1f, s.z, dx/travelTime, vy, dz/travelTime, false});
          addSmoke(s.x, s.y + 0.15f);
      }
  }

  glutTimerFunc(fireInterval, fireBomb, 0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0.0f, 1.5f, 3.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glBegin(GL_QUADS);
  glColor3f(0.02f, 0.02f, 0.1f); glVertex2f(-1, 1);
  glColor3f(0.02f, 0.02f, 0.1f); glVertex2f(1, 1);
  glColor3f(0.3f, 0.1f, 0.1f); glVertex2f(1, -1);
  glColor3f(0.3f, 0.1f, 0.1f); glVertex2f(-1, -1);
  glEnd();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  glDisable(GL_LIGHTING);
  glBegin(GL_POINTS);
  for (const auto &s : stars) {
    glColor4f(1.0f, 1.0f, 1.0f, s.size / 2.0f);
    glVertex3f(s.x, s.y, -5.0f);
  }
  glEnd();

  glPushMatrix();
  glTranslatef(1.5f, 1.5f, -2.0f);
  glColor3f(0.9f, 0.9f, 0.8f);
  glutSolidSphere(0.2, 20, 20);
  glPopMatrix();
  glEnable(GL_LIGHTING);

  // Deniz (3D Yoğun Izgara + Shader)
  if (glUseProgram_ptr) {
      glUseProgram_ptr(waterShader);
      if (glUniform1f_ptr) glUniform1f_ptr(waterTimeLoc, waveOffset * 8.0f);
  }
  glPushMatrix();
  glBegin(GL_QUADS);
  for (float x = -6.0f; x <= 6.0f; x += 0.2f) {
      for (float z = -6.0f; z <= 6.0f; z += 0.2f) {
          glColor4f(0.0f, 0.25f, 0.45f, 0.85f);
          glVertex3f(x, -0.5f, z);
          glVertex3f(x + 0.2f, -0.5f, z);
          glVertex3f(x + 0.2f, -0.5f, z + 0.2f);
          glVertex3f(x, -0.5f, z + 0.2f);
      }
  }
  glEnd();
  glPopMatrix();
  if (glUseProgram_ptr) glUseProgram_ptr(0);

  // Filoları Çiz (Dikey/Z Düzeni + Detaylı Gemiler)
  for (auto &s : mancurianFleet) {
      glPushMatrix();
      glTranslatef(0, 0, s.z);
      drawShip(s.x, s.y, shipWidth, shipHeight, 0.4f, 0.2f, 0.1f, false);
      if (s.isBurning) {
          glDisable(GL_LIGHTING);
          drawCircle(s.x, s.y + 0.12f, 0.05f, 8, 1.0f, 0.3f, 0.0f, 0.8f);
          glEnable(GL_LIGHTING);
      }
      glPopMatrix();
  }
  if (!mancurianFleet.empty()) {
      glPushMatrix();
      glTranslatef(mancurianFleet[0].x, mancurianFleet[0].y, mancurianFleet[0].z);
      drawHorse(-0.1f, 0.1f);
      drawCroissant(0.05f, 0.1f);
      glPopMatrix();
  }
  
  for (auto &s : frenchFleet) {
      glPushMatrix();
      glTranslatef(0, 0, s.z);
      drawShip(s.x, s.y, shipWidth, shipHeight, 0.1f, 0.1f, 0.5f, true);
      if (s.isBurning) {
          glDisable(GL_LIGHTING);
          drawCircle(s.x, s.y + 0.12f, 0.05f, 8, 1.0f, 0.3f, 0.0f, 0.8f);
          glEnable(GL_LIGHTING);
      }
      glPopMatrix();
  }
  if (!frenchFleet.empty()) {
      glPushMatrix();
      glTranslatef(frenchFleet[0].x, frenchFleet[0].y, frenchFleet[0].z);
      drawBaguetteDepot(0, 0.15f);
      glPopMatrix();
  }

  // Projeler (3D Konumlandırma + Shader)
  if (glUseProgram_ptr) {
      glUseProgram_ptr(projectileShader);
      if (glUniform1f_ptr) glUniform1f_ptr(projTimeLoc, storyTimer);
  }
  for (const auto &b : mancurianBombs) {
      glPushMatrix();
      glTranslatef(b.x, b.y, b.z);
      drawSoySauceArrow(0, 0, b.vx, b.vy);
      glPopMatrix();
  }
  for (const auto &b : frenchBombs) {
      glPushMatrix();
      glTranslatef(b.x, b.y, b.z);
      drawBaguette(0, 0, b.vx, b.vy);
      glPopMatrix();
  }
  if (glUseProgram_ptr) glUseProgram_ptr(0);

  // Metin Katmanı (Hızlı Hikaye)
  renderText(-0.4f, 0.9f, "Operasyon: Kruvasan Atesi (Dikey Donanma)");
  if (storyTimer < 3.0f) renderText(-0.4f, 0.8f, "Bize her yer Mancurya!");
  else if (storyTimer < 6.0f) renderText(-0.5f, 0.8f, "Marsilya Dusuyor! Bagetler Tehlikede!");
  else if (storyTimer < 9.0f) renderText(-0.6f, 0.8f, "Mancurya Imparatoru: Guzel de, Marsilya neresi?");
  else if (storyTimer < 12.0f) renderText(-0.4f, 0.8f, "Atlar kruvasan bagimlisi oldu!");
  else renderText(-0.2f, 0.8f, "Pirus Zaferi!");

  glDisable(GL_LIGHTING);
  for (const auto &e : activeExplosions) {
      glPushMatrix();
      glTranslatef(e.x, e.y, e.z);
      drawExplosion(0, 0, e.timer);
      glPopMatrix();
  }
  glEnable(GL_LIGHTING);
  glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    GLfloat light_pos[] = { 1.0, 5.0, 5.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    loadShaders();
}

void initStars() {
  for (int i = 0; i < 200; i++) {
    stars.push_back({(rand() % 400 - 200) / 40.0f, (rand() % 400 - 100) / 40.0f, (rand() % 20 + 1) / 10.0f});
  }
}

int main(int argc, char **argv) {
  srand(time(0));
  initStars();
  initFleets();
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("3D Operation Croissant Fire");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000, fireBomb, 0);
  glutTimerFunc(16, update, 0);
  glutMainLoop();
  return 0;
}
