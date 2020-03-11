#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>

//deklaracije callback funkcija
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);

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
