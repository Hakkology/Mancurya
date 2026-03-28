#include <GL/glut.h>
#include <Mancurya/Environment.hpp>
#include <cmath>
#include <cstdlib>

namespace Mancurya {

void Environment::init() {
  stars.clear();
  for (int i = 0; i < 200; i++) {
    // NDC coordinates (-1 to 1) for the sky layer
    stars.push_back({
        (float)(rand() % 2000 - 1000) / 1000.0f,
        (float)(rand() % 1000 - 100) / 1000.0f, // Upper half of screen
        (float)(rand() % 20 + 1) / 10.0f
    });
  }
}

void Environment::draw(float storyTimer) {
  glDisable(GL_LIGHTING);

  // Screen Space Layer for Sky & Stars
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glDisable(GL_DEPTH_TEST);
  
  // Sky Background (Darker Night)
  glBegin(GL_QUADS);
    glColor3f(0.002f, 0.002f, 0.01f); glVertex2f(-1, 1); glVertex2f(1, 1);
    glColor3f(0.01f, 0.01f, 0.02f); glVertex2f(1, -1); glVertex2f(-1, -1);
  glEnd();

  // Stars
  glBegin(GL_POINTS);
  for (auto &s : stars) {
    glColor4f(1.0f, 1.0f, 1.0f, s.size / 2.0f);
    glVertex2f(s.x, s.y);
  }
  glEnd();

  // Moon
  glPushMatrix();
  glTranslatef(0.0f, 0.7f, 0.0f);
  drawCircle(0, 0, 0.05f, 20, 0.9f, 0.9f, 0.8f);
  glPopMatrix();

  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_LIGHTING);
}

void Environment::drawCircle(float cx, float cy, float r, int segs, float red,
                             float grn, float blu) {
  glColor3f(red, grn, blu);
  glBegin(GL_POLYGON);
  for (int i = 0; i < segs; i++) {
    float t = 2.0f * 3.14159f * i / segs;
    glVertex2f(cx + r * cos(t), cy + r * sin(t));
  }
  glEnd();
}

} // namespace Mancurya
