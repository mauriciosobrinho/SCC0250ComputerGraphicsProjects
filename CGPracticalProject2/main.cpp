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
bool invert = 0;
float bla = 0.0;
int looking = 3;

//Centro da tela
int x_center = 320;
int y_center = 240;

//Posição e movimentação da aranha
float facing = 0;
float x_spider = 0.0;
float y_spider = 0.0;
float z_spider = 0.0;
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
    glLoadIdentity();
}

void drawGrid(float size, float step) {
	float i;
	glColor3f(0, 1, 0);
	for(i = 0; i < size; i = i + step) {
		/** Translada para o ponto no eixo x para desenhar a linha paralela ao eixo z */
		glTranslatef(i, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, size);
		glEnd();
		glTranslatef(-i, 0.0, 0.0);

		/** Translada para o ponto no eixo z para desenhar a linha paralela ao eixo x */
		glTranslatef(0.0, 0.0, i);
		glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(size, 0.0, 0.0);
		glEnd();
		glTranslatef(0.0, 0.0, -i);
	}
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
        glutSolidSphere(1, quality, quality);
        glTranslatef(0, -2.5, 0);
        glutSolidSphere(1.5, quality, quality);
    glPopMatrix();

    rotator = (!moving) ? 0 : rotator + 5.0*(t2 - t1);
    double angle = sin(rotator) * 10.0;
    double m = (sin(rotator) > 0) ? sin(rotator) : 0;
    double n = (sin(rotator) < 0) ? -sin(rotator) : 0;
    double movx, movz;
    //angle = 0; //TEST LINE

    //Legs (i=0 ou j=1 -> Right Legs, i=1 ou j = -1 ->Left Legs)
    for (int i=0, j=1; i<2; i++, j = -j) {
        //Foreleg
        glPushMatrix();
        glTranslatef(cos((i*0.75 + 0.125) * M_PI), sin((i*0.75 + 0.125) * M_PI), 0);
        glRotatef(j*60 - angle, 0, 0, 1);
        glBegin(GL_LINE_STRIP);
            glVertex3f(0, 0, 0);

            //Articulação
            movx = (i*m + (!i)*n) * 0.05;
            movz = (i*m + (!i)*n) * 0.5;
            glVertex3f((0.5 + movx)*j, 0, 1.5 + movz);

            //Pata
            movx *= 2;
            movz *= 2;
            glVertex3f((1.2 + movx)*j, 0, -1.5 + movz);
        glEnd();
        glPopMatrix();

        //Internal Legs (k = -1 ou l=1-> Superior Internal Legs, k=1 ou l=0 -> Inferior Internal Legs)
        int l=1;
        for (float k = -1.0; l>=0; l--, k += 2.0) {
            glPushMatrix();
            glTranslatef(cos(i * M_PI), sin(i * M_PI), 0);
            glRotatef(k*angle, 0, 0, 1);
            glRotatef(30*k*j, 0, 0, 1);
            glBegin(GL_LINE_STRIP);
                glVertex3f(0, 0, 0);

                //Articulação
                movx = ((i^l)*n + !(i^l)*m) * 0.05;
                movz = ((i^l)*n + !(i^l)*m) * 0.5;
                glVertex3f((0.6 + movx)*j, 0, 1.5 + movz);

                //Pata
                movx *= 2;
                movz *= 2;
                glVertex3f((1.2 + movx)*j, 0, -1.5 + movz);
            glEnd();
            glPopMatrix();
        }

        //Hind Legs
        glPushMatrix();
        glTranslatef(-cos((i*0.75 + 0.125) * M_PI), sin((-i*0.75 - 0.125) * M_PI) , 0);
        glRotatef(45*j + angle, 0, 0, 1);
        glBegin(GL_LINE_STRIP);
                glVertex3f(0, 0, 0);

                //Articulação
                movx = (i*m + (!i)*n) * 0.05;
                movz = (i*m + (!i)*n) * 0.5;
                glVertex3f((-0.7 - movx)*j, 0, 1.5 + movz);

                //Pata
                movx *= 2;
                movz *= 2;
                glVertex3f((-1.2 - movx)*j, 0, -1.5 + movz);
        glEnd();
        glPopMatrix();
    }

}

static void display(void) {
    //Reposiciona a câmera
    glLoadIdentity();
    if      (looking == 1) gluLookAt(x_spider+10, y_spider   , z_spider   ,   x_spider, y_spider, z_spider,   0,0,1);
    else if (looking == 2) gluLookAt(x_spider   , y_spider+10, z_spider   ,   x_spider, y_spider, z_spider,   0,0,1);
    else if (looking == 3) gluLookAt(x_spider   , y_spider   , z_spider+15,   x_spider, y_spider, z_spider,   0,1,0);
    else if (looking == 4) gluLookAt(x_spider-3 , y_spider-2 , z_spider+10,   x_spider, y_spider, z_spider,   0,1,0);

    //Tempo
    t2 = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    //Limpa a tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Desenha grid
    glPushMatrix();
        glTranslatef(-50, 50, -1.5);
        glRotatef(90, 1, 0, 0);
        drawGrid(100, 1.0);
    glPopMatrix();

    glColor3d(1, 0, 0);
    glPushMatrix();

    //Prepara a rotação em direção ao clique
    glTranslatef(x_spider, y_spider, 0);
    glRotatef(facing, 0, 0, 1);
    glTranslatef(-x_spider, -y_spider, 0);

    //Prepara a movimentação em direção ao clique
    if (moving == 1) {
        double step = (t2 - t1) * 1.5;
        if (invert) step = -step;

        //Dá um passo
        x_spider += step * cos(M_PI * (facing+90)/180);
        y_spider += step * sin(M_PI * (facing+90)/180);
    }

    //Movimenta a aranha
    glTranslatef(x_spider, y_spider, 0);

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

        case '1':
            //Câmera em X
            looking = 1;
            break;

        case '2':
            //Câmera em Y
            looking = 2;
            break;

        case '3':
            //Câmera em Z
            looking = 3;
            break;

        case '4':
            //Câmera em posição "livre"
            looking = 4;
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

void arrowKeys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            if (moving && facing == 0) break;
            moving = 1;
            invert = 0;
            t1 = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        break;

        case GLUT_KEY_DOWN:
            if (moving && facing == 180) break;
            moving = 1;
            invert = 1;
            t1 = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        break;

        case GLUT_KEY_LEFT:
            facing++;
        break;

        case GLUT_KEY_RIGHT:
            facing--;
        break;
    }
}

void arrowKeysRelease(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            moving = 0;
        break;
    }
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
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //Inicia a tela
    glutCreateWindow("GLUT Shapes");

    //Funções
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(arrowKeys);
    glutSpecialUpFunc(arrowKeysRelease);
    glutIdleFunc(idle);

    //Cor de fundo
    glClearColor(0,0,0,0);

    //3d?
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glutMainLoop();

    return EXIT_SUCCESS;
}

