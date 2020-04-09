#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>

//dimenzije prozora
static int window_width, window_height;

float playerPosX = 2.5;
float playerPosZ = -5;
float playerLength = 0.1;
float lengthOfRoad = 1.5;
float playerStepForward  =  1.2;
float playerStepBackward = -1.2;

bool animation_ongoing = false;
int pauseStart = 1;

//brzina kojom se igrac krece
float move = 0.1;

//deklaracije callback funkcija
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);

void drawCar();
void drawTree();

bool canMoveThisWay(unsigned char key);

void setLighting();
void setMaterial(char *option);

void drawPlayer(float x, float z);
void drawRoad();

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
    glutTimerFunc(1, on_timer, 0);
    
    //obavlja se OpenGL inicijalizacija
    glClearColor(0, 0.6, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    //osvetljenje
    setLighting();
    
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
    gluPerspective(60, window_width/(float)window_height, 0.7, 10);
    
    //podesava se tacka pogleda
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            1.2, 0.5, 2+playerPosZ,
            1.2, 0,   1+playerPosZ,
            0,   1,   0
        );
    
    //iscrtavamo igraca
    drawPlayer((float)(playerPosX/2), playerPosZ);
    
    //iscrtavamo put
    drawRoad();
    
    //nova slika se salje na ekran
    glutSwapBuffers();
}

static void on_keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            //izlazi se iz programa
            exit(EXIT_SUCCESS);
        break;
        
        case 'g':
        case 'G':
            //pokrece se igrica
            if (!animation_ongoing)
                animation_ongoing = true;
        break;
        
        case 'a':
        case 'A':
            if (animation_ongoing) {
                //krecemo se na levu stranu ako mozemo
                if (canMoveThisWay('a'))
                    playerPosX += playerStepBackward;
            }
        break;
        
        case 'd':
        case 'D':
            if (animation_ongoing) {
                //krecemo se na desnu stranu ako mozemo
                if (canMoveThisWay('d'))
                    playerPosX += playerStepForward;
            }
        break;
        
        case 'r':
        case 'R':
            animation_ongoing = true;
            playerPosX = 2.5;
            playerPosZ = -5;
            move = 0.1;
            sleep(1);
        break;
        
        //ako neparan broj puta kliknemo na p ili P, igra se pauzira
        //ako paran broj puta kliknemo na p ili P, vracamo se u igru
        case 'p':
        case 'P':
            if (pauseStart % 2 == 1)
                animation_ongoing = false;
            else
                animation_ongoing = true;
            pauseStart++;
        break;
    }
}

bool canMoveThisWay(unsigned char key) {
    float playerCurrentPosition = playerPosX;
    
    if (key == 'a' && playerCurrentPosition < 1.8)
        return false;
    
    if (key == 'd' && playerCurrentPosition > 3.2)
        return false;
    
    return true;
}

static void on_reshape(int width, int height) {
    //pamte se sirina i visina prozora
    window_width = width;
    window_height = height;
}

static void on_timer(int value) {
    //kretanje igraca po putu
    if (animation_ongoing) {
        
        //unapred
        playerPosZ -= move;
        
        //ubrzanje nakon nekog vremena
        if ((int)playerPosZ % 50 == 0)
            move += 0.01;
        
        //nakon nekog vremena ce biti kraj igre
        //ako se pre toga ne pogine, kao pobeda
        if ((int)playerPosZ == -300)
            animation_ongoing = false;
    }
    //forsira se ponovno iscrtavanje prozora
    glutPostRedisplay();
    
    //ponovo se postavlja tajmer
    glutTimerFunc(1, on_timer, 0);
}

void setLighting() {
    //odredjujemo vektore
    GLfloat position [] = {10, 10, 10, 1};
    GLfloat ambient  [] = {0.4, 0.4, 0.4, 1};
    GLfloat diffuse  [] = {1, 1, 1, 1};
    GLfloat specular [] = {1, 1, 1, 1};
    
    //inicijalizujemo osobine osvetljenja
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

void setMaterial(char *option) {
    //put
    if (strcmp("road", option)==0) {
        //odredjujemo vektore
        GLfloat specular [] = {0, 0, 0, 0};
        GLfloat ambient  [] = {0.305, 0.431, 0.411, 1};
        GLfloat diffuse  [] = {0.301, 0.301, 0.301, 1};
        int shininess = 100;
        
        //inicijalizujemo osobine materijala
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    //linije na putu
    else if (strcmp("lines", option)==0) {
        //odredjujemo vektore
        GLfloat specular [] = {0, 0, 0, 0};
        GLfloat ambient  [] = {0.205, 0.231, 0.211, 1};
        GLfloat diffuse  [] = {0.201, 0.201, 0.201, 1};
        int shininess = 100;
        
        //inicijalizujemo osobine materijala
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    //igrac
    else if (strcmp("player", option)==0) {
        //odredjujemo vektore
        GLfloat specular [] = {1, 0, 0, 0};
        GLfloat ambient  [] = {1, 0, 0, 1};
        GLfloat diffuse  [] = {1, 0, 0, 1};
        int shininess = 100;
        
        //inicijalizujemo osobine materijala
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);	
    }
}

void drawPlayer(float x, float z) {
    //za sada loptica
    setMaterial("player");
    glPushMatrix();
        glTranslatef(x, -0.5, z);
        glutSolidSphere(playerLength, 10, 10);
    glPopMatrix();
}

void drawRoad(void) {
    //z osa
    /*glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, -100);
    glEnd();*/
    
    for (float i=-0.5; i < 0.5; i+=0.5) {
        setMaterial("road");
        glPushMatrix();
            glTranslatef((lengthOfRoad + i - 0.05), -0.75, playerPosZ);
            glScalef(lengthOfRoad, 0.3, 500);
            glutSolidCube(1);
        glPopMatrix();
        
        //iscrtavamo isprekidane linije po putu
        for (int j=10; (float)j > -500; j--) {
            if (j%2 == 0) {
                setMaterial("lines");
                glPushMatrix();
                    glTranslatef((lengthOfRoad + i), -0.75, j);
                    glScalef(0.05, 0.32, 1);
                    glutSolidCube(1);
                glPopMatrix();
            }
            else {
                setMaterial("road");
                glPushMatrix();
                    glTranslatef((lengthOfRoad + i), -0.75, j);
                    glScalef(0.05, 0.32, 1);
                    glutSolidCube(1);
                glPopMatrix();
            }
        }
    }
}
