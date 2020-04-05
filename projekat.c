#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>

//dimenzije prozora
static int window_width, window_height;

//deklaracije callback funkcija
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);

void drawCar();
void drawTree();

int main(int argc, char **argv) {
    //inicijalizuje se GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //kreira se prozor
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 50);
    glutCreateWindow(argv[0]);

    //registruju se callback funkcije
    glutDisplayFunc(on_display);
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);

    //obavlja se OpenGL inicijalizacija
    glClearColor(0, 0.6, 0, 0);
    glEnable(GL_DEPTH_TEST);
    
    //program ulazi u glavnu petlju
    glutMainLoop();

    return 0;
}

static void on_display(void) {
    //brise se prethodni sadrzaj prozora
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //podesava se viewport
    glViewport(0, 0, window_width, window_height);
    
    //podesava se projekcija
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(90, window_width/(float)window_height, 1, 6);
    glFrustum(-1, 1, -1, 1, 1, 10);
    
    //podesava se tacka pogleda
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            1, 3, 3,
            0, 0, 0,
            0, 1, 0
        );
    
    glTranslatef(-1, 0, 0);
    drawCar();
    glTranslatef(1, 0, 0);
    
    glTranslatef(1, 0, 0);
    drawTree();
    
    //nova slika se salje na ekran
    glutSwapBuffers();
}

static void on_keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        //izlazi se iz programa
        exit(0);
        break;
    }
}

static void on_reshape(int width, int height) {
    //pamte se sirina i visina prozora
    window_width  = width;
    window_height = height;
}

void drawCar() {
    glPushMatrix();
        glColor3f(1, 0, 0);
        glTranslatef(0, 0, 0);
        glScalef(2, 0.2-0.12, 1);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(1, 0, 0);
        glTranslatef(0, 0.21, 0);
        glScalef(2, 0.2+0.14, 1);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.6, 0, 0);
        glTranslatef(0.24, 0.57, 0);
        glScalef(1, 0.5-0.12, 0.7-0.05);
        glutSolidCube(1);
    glPopMatrix();
    
    //stakla
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0.1, 0.57, 0);
        glScalef(1-0.41, 0.5-0.25, 0.7-0.04);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0.1+0.48, 0.57, 0);
        glScalef(1-0.41-0.4, 0.5-0.25, 0.7-0.04);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0.1+0.14, 0.57, 0);
        glScalef(1-0.41+0.42, 0.5-0.25, 0.7-0.04-0.08);
        glutSolidCube(1);
    glPopMatrix();
    
    //tockovi
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0-0.64, 0.21-0.22, 0-0.01+0.42);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0-0.64, 0.21-0.22, 0+0.01-0.42);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0+0.64, 0.21-0.22, 0+0.01-0.42);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0+0.64, 0.21-0.22, 0-0.01+0.42);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.5);
        glTranslatef(0.64, -0.01, 0.42);
        glScalef(2-1.859999, 0.2-0.06, 1-0.78);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.5);
        glTranslatef(-0.64, -0.01, 0.42);
        glScalef(2-1.859999, 0.2-0.06, 1-0.78);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.5);
        glTranslatef(0.64, -0.01, -0.42);
        glScalef(2-1.859999, 0.2-0.06, 1-0.78);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.5);
        glTranslatef(-0.64, -0.01, -0.42);
        glScalef(2-1.859999, 0.2-0.06, 1-0.78);
        glutSolidCube(1);
    glPopMatrix(); 
}

void drawTree(){
    glPushMatrix();
        glColor3f(0, 0.35, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.5, 2, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.35, 0.2, 0.1);
        glTranslatef(0, -0.25, 0);
        glScalef(0.4, 0.5, 0.4);
        glutSolidCube(1);
    glPopMatrix();
}
