#include <GL/glew.h>
#include <Mancurya/Environment.hpp>
#include <Mancurya/Sea.hpp>
#include <Mancurya/FleetManager.hpp>
#include <GL/glut.h>
#include <ctime>
#include <algorithm>

using namespace Mancurya;

Environment env;
Sea sea;
FleetManager fleets;
float storyTimer = 0.0f;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 
    // Stable viewing angle for better fleet and environment visibility
    gluLookAt(0.0f, 1.2f, 7.0f, 0.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f);
    
    env.draw(storyTimer);
    sea.draw(fleets.explosions);
    fleets.draw(storyTimer);

    // Story UI
    const char* lines[] = {
        "Bize her yer Mancurya!", 
        "Marsilya Dusuyor! Bagetler Tehlikede!", 
        "Mancurya Imparatoru: Gidiyoz mu?", 
        "Atlar kruvasan bagimlisi oldu!", 
        "Pirus Zaferi!"
    };
    glDisable(GL_LIGHTING); 
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2f(-0.4f, 0.85f);
    int idx = ((int)(storyTimer / 3)) % 5;
    for (const char* c = lines[idx]; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

void update(int v) {
    storyTimer += 0.016f;
    sea.update();
    fleets.update(storyTimer);
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void fire(int v) {
    fleets.fire();
    glutTimerFunc(800, fire, 0);
}

void init() {
    glewInit();
    env.init();
    sea.init();
    fleets.init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lp[] = {0.0f, 2.0f, 3.0f, 1.0f};
    GLfloat ambient[] = { 0.1f, 0.1f, 0.15f, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.7f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lp);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    srand(time(0)); 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720); 
    glutCreateWindow("Mancurya 3D - Modular Architecture");
    init();
    glutDisplayFunc(display); 
    glutReshapeFunc([](int w, int h) {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (float)w / h, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
    });
    glutTimerFunc(1000, fire, 0);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
