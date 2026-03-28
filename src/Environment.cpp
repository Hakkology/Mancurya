#include <GL/glut.h>
#include <Mancurya/Environment.hpp>
#include <cmath>
#include <cstdlib>

namespace Mancurya {

void Environment::init() {
  for (int i = 0; i < 200; i++) {
    stars.push_back({(rand() % 400 - 200) / 40.0f, (rand() % 400 - 100) / 40.0f,
                     (rand() % 20 + 1) / 10.0f});
  }
}

void Environment::draw(float storyTimer) {
  glDisable(GL_LIGHTING);

  // Sky Background & Stars & Moon (Screen Space Layer)
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glBegin(GL_QUADS);
  glColor3f(0.01f, 0.01f, 0.05f);
  glVertex2f(-1, 1);
  glVertex2f(1, 1);
  glColor3f(0.05f, 0.02f, 0.02f);
  glVertex2f(1, -1);
  glVertex2f(-1, -1);
  glEnd();
  glBegin(GL_POINTS);
  for (auto &s : stars) {
    glColor4f(1.0f, 1.0f, 1.0f, s.size / 2.0f);
    glVertex2f(s.x, s.y);
  }
  glEnd();
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
    float t = 2.0f * PI * i / segs;
    glVertex2f(cx + r * cos(t), cy + r * sin(t));
  }
  glEnd();
}

} // namespace Mancurya
