//Maurício Luiz Sobrinho <m.luizsobrinho@gmail.com>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

#define M_PI 3.14159265358979323846

//Controladores
static int quality = 64;
bool moving = 0;
float bla = 0.0;

//Centro da tela
int x_center = 320;
int y_center = 240;

//Posição e movimentação da aranha
float facing = 0;
float x_spider = 0.0;
float y_spider = 0.0;
float x_goal = 0.0;
float y_goal = 0.0;

//Tempo
double t1 = 0.0;
double t2 = 0.0;
double rotator = 0.0;

void mouse_test(GLint button, GLint action, GLint x, GLint y){
    //Calcula a nova direção da aranha
    if (button == GLUT_LEFT_BUTTON && action == GLUT_UP) {
        facing = (x_spider - x == 0) ? 90 : 90 - atan2(y_center - y, x_center - x) * 180 / M_PI;

        //Define as variáveis relacionadas à movimentação
        moving = 1;
        x_goal = x;
        y_goal = y;
        t1 = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

        glutPostRedisplay();
    }
}

/* GLUT callback Handlers */

static void resize(int width, int height) {
    const float ar = (float) width / (float) height;

    x_center = width/2;
    y_center = height/2;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}


void glSolidCircle(GLfloat x, GLfloat y, GLfloat radius) {
    GLfloat twicePi = 2.0f * M_PI;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for (int i=0; i <= quality; i++)
            glVertex2f(radius * cos(i*twicePi / quality) + x, radius * sin(i*twicePi / quality) + y);
    glEnd();
}

void glSpiderLeg(GLfloat x, GLfloat y, GLfloat radius) {
    GLfloat twicePi = 2.0f * M_PI;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for (int i=0; i <= quality; i++)
            glVertex2f(radius * cos(i*twicePi / quality) + x, radius * sin(i*twicePi / quality) + y);
    glEnd();
}

void glSolidSpider() {
    //Abdomen and Appendix
    glPushMatrix();
        glSolidCircle(0, 0, 1);
        glSolidCircle(0, -2.5, 1.5);
    glPopMatrix();

    rotator = (!moving) ? 0 : rotator + 5.0*(t2 - t1);
    double angle = sin(rotator) * 10.0;

    //Legs (i=0 -> Right Legs, i=1 ->Left Legs)
    for (int i=0, j=1; i<2; i++, j = -j) {
        //Foreleg
        glPushMatrix();
        glTranslatef(cos((i*0.75 + 0.125) * M_PI), sin((i*0.75 + 0.125) * M_PI), 0);
        glRotatef(-angle, 0, 0, 1);
        glBegin(GL_LINE_STRIP);
            glVertex2f(0, 0);
            glVertex2f(0.5*j, 2);
            glVertex2f(1.2* j, 3);
        glEnd();
        glPopMatrix();

        //Internal Legs
        for (float k = -1.0; k<2.0; k += 2.0) {
            glPushMatrix();
            glTranslatef(cos(i * M_PI), sin(i * M_PI), 0);
            glRotatef(k*angle*j, 0, 0, 1);
            glRotatef(12.5*k, 0, 0, 1);
            glBegin(GL_LINE_STRIP);
                glVertex2f(0, 0);
                glVertex2f(1*j, 0.4);
                glVertex2f(2*j, -1.5);
            glEnd();
            glPopMatrix();
        }

        //Hind Legs
        glPushMatrix();
        glTranslatef(-cos((i*0.75 + 0.125) * M_PI), sin((-i*0.75 - 0.125) * M_PI) , 0);
        glRotatef(angle, 0, 0, 1);
        glBegin(GL_LINE_STRIP);
                glVertex2f(0, 0);
                glVertex2f(-1*j, -0.5);
                glVertex2f(-1.5*j, -3);
        glEnd();
        glPopMatrix();
    }

}

static void display(void) {
    //Tempo
    t2 = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    //Limpa a tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1, 0, 0);

    glPushMatrix();

    //Prepara a rotação em direção ao clique
    glTranslatef(x_spider, y_spider, 0);
    glRotatef(facing, 0, 0, 1);
    glTranslatef(-x_spider, -y_spider, 0);

    //Prepara a movimentação em direção ao clique
    if (moving == 1) {
        double step = (t2 - t1) * 1.5;

        //Finaliza a movimentação ou dá um passo
        if (0 && x_spider + step * cos(M_PI * (facing+90)/180) > x_goal/10) {
            glColor3d(0,1,0); //TESTE
            x_spider = x_goal;
            y_spider = y_goal;
            moving = 0;
        }
        else {
            x_spider += step * cos(M_PI * (facing+90)/180);
            y_spider += step * sin(M_PI * (facing+90)/180);
        }
    }

    //Movimenta a aranha
    glTranslatef(x_spider, y_spider, -15);

    //Desenha a aranha
    glSolidSpider();

    t1 = t2;
    glPopMatrix();
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y) {
    switch (key) {
        case 27 :
        case 'q':
            exit(0);
            break;

        case 'm':
            break;

        case '+':
            if (quality < 2097152) quality *= 2;
            break;

        case '-':
            if (quality >= 6) quality /= 2;
            break;
    }

    glutPostRedisplay();
}

static void idle(void) {
    glutPostRedisplay();
}

/* Program entry point */

int main(int argc, char *argv[]) {
    //Início
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    //Inicia a tela
    glutCreateWindow("GLUT Shapes");

    //Funções
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glutMouseFunc(mouse_test);

    //Cor de fundo
    glClearColor(0,0,0,0);

    glutMainLoop();

    return EXIT_SUCCESS;
}

