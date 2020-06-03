#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include"image.h"
#include<time.h>

//imena fajlova sa teksturama
#define FILENAME0 "grass.bmp"
#define FILENAME1 "sky.bmp"

//identifikatori tekstura
static GLuint names[2];

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

//niz prepreka za svaku traku
int leftWalls[500];
int midWalls[500];
int rightWalls[500];

//niz dijamanata za svaku traku
int leftCoins[500];
int midCoins[500];
int rightCoins[500];

//indikatori za postavljanje
int indWalls = 0;
int indCoins = 0;

//indikatori da su dijamanti pokupljeni
int leftCoinsInd[500];
int midCoinsInd[500];
int rightCoinsInd[500];

int score = 0;

//indikator za poraz
int indGameOver = 0;

//deklaracije callback funkcija
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);

bool canMoveThisWay(unsigned char key);

void setLighting();
void setMaterial(char *option);

void drawPlayer(float x, float z);
void drawRoad();
void drawCar();
void drawTree();
void drawGrass(); 

void setWalls();
void drawWalls();
void drawBear();
int is_crashed();

void setCoins();
void drawCoins();
int is_coin_collected();

void writeScore();
void end_game();

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
    
    //objekat koji predstavlja teksturu ucitanu iz fajla
    Image *image;
    
    //ukljucuju se teksture
    glEnable(GL_TEXTURE_2D);
    
    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);
    
    //inicijalizuje se objekat koji ce sadrzati teksture ucitane iz fajla
    image = image_init(0, 0);
    
    //generisu se identifikatori tekstura
    glGenTextures(2, names);
    
    //kreira se prva tekstura
    image_read(image, FILENAME0);
    
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    
    //kreira se druga tekstura
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    
    //iskljucujemo aktivnu teksturu
    glBindTexture(GL_TEXTURE_2D, 0);

    //unistava se objekat za citanje tekstura iz fajla
    image_done(image);
    
    //postavljamo "sito" kako bismo koristili rand za generisanje
    srand(time(NULL));
    
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
    
    //postavljamo pozadinu
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, names[1]);

        glBegin(GL_POLYGON);
            glTexCoord2f(0, 0);
            glVertex3f(-12, -17.5, playerPosZ);
            
            glTexCoord2f(1, 0);
            glVertex3f(14, -17.5, playerPosZ);
            
            glTexCoord2f(1, 3);
            glVertex3f(14, 12, playerPosZ-12);
            
            glTexCoord2f(0, 3);
            glVertex3f(-12, 12, playerPosZ-12);
        glEnd();
        
        glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    //iscrtavamo igraca, put, travu
    drawPlayer((float)(playerPosX/2), playerPosZ);
    drawRoad();
    drawGrass();
    
    //prepreke
    drawWalls();
    if (is_crashed()) {
        animation_ongoing = false;
        indGameOver = 1;
        glutDisplayFunc(end_game);
        glutPostRedisplay();
    }
    
    //dijamante
    drawCoins();
    if (is_coin_collected()) {
        score++;
        //printf("%d\n", score);
    }
    writeScore();
    
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
        
        //restart dugme
        case 'r':
        case 'R':
            animation_ongoing = true;
            playerPosX = 2.5;
            playerPosZ = -5;
            move = 0.1;
            sleep(1);
            score = 0;
            indCoins = 0;
            glutDisplayFunc(on_display);
            indGameOver = 0;
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
            move += 0.0015;
        
        //nakon nekog vremena ce biti kraj igre
        //ako se pre toga ne pogine, kao pobeda
        if ((int)playerPosZ == -450) {
            animation_ongoing = false;
            glutDisplayFunc(end_game);
            glutPostRedisplay();
        }
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
    GLfloat diffuse  [] = {0.8, 0.8, 0.8, 1};
    GLfloat specular [] = {1, 1, 1, 1};
    
    //inicijalizujemo osobine osvetljenja
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
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
    //trava
    else if (strcmp("grass", option)==0) {
        //odredjujemo vektore
        GLfloat specular [] = {0, 0, 0, 0};
        GLfloat ambient  [] = {0.1, 0.87, 0.1, 0.8};
        GLfloat diffuse  [] = {0.1, 0.87, 0.1, 1};
        int shininess = 100;
        
        //inicijalizujemo osobine materijala
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    //prepreke
    else if (strcmp("wall", option)==0) {
        //odredjujemo vektore
        GLfloat specular [] = {0, 0, 0, 0};
        GLfloat ambient  [] = {0.334, 0.111, 0.552, 1};
        GLfloat diffuse  [] = {0.552, 0.111, 0.552, 1};
        int shininess = 100;
        
        //inicijalizujemo osobine materijala
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    //dijamanti
    else if (strcmp("coin", option)==0) {
        //odredjujemo vektore
        GLfloat specular [] = {0, 0, 0, 0};
        GLfloat ambient  [] = {0.809, 0.252, 0.2, 1};
        GLfloat diffuse  [] = {0.809, 0.452, 0.2, 1};
        int shininess = 100;
        
        //inicijalizujemo osobine materijala
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
}

void drawGrass(void) {
    setMaterial("grass");
    glPushMatrix();
        glTranslatef(-0.55, -0.75, playerPosZ);
        glScalef(lengthOfRoad, 0.3, 500);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(2.95, -0.75, playerPosZ);
        glScalef(lengthOfRoad, 0.3, 500);
        glutSolidCube(1);
    glPopMatrix();
    
    //jelkice
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    for (int j=10; (float)j > -500; j--) {
        if (j < playerPosZ+2 && j > playerPosZ-12) {
            glPushMatrix();
                glTranslatef(2.95, -0.5, j);
                glScalef(lengthOfRoad/4, lengthOfRoad/4, lengthOfRoad/4);
                drawTree();
            glPopMatrix();
    
            glPushMatrix();
                glTranslatef(-0.45, -0.5, j);
                glScalef(lengthOfRoad/4, lengthOfRoad/4, lengthOfRoad/4);
                drawTree();
            glPopMatrix();
        }
    }
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
}

void drawRoad() {
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
                glEnable(GL_COLOR_MATERIAL);
                glPushMatrix();
                    glColor3f(0.5, 0.5, 0.5);
                    glTranslatef((lengthOfRoad + i), -0.75, j);
                    glScalef(0.05, 0.32, 1);
                    glutSolidCube(1);
                glPopMatrix();
            }
        }
        glDisable(GL_COLOR_MATERIAL);
    }
}

void drawPlayer(float x, float z) {
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
        glTranslatef(x, -0.5, z);
        glRotatef(90, 0, 1, 0);
        glScalef(lengthOfRoad/4, lengthOfRoad/4, lengthOfRoad/4);
        drawCar();
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
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
        glRotatef(playerPosZ*50, 0, 0, 1);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0-0.64, 0.21-0.22, 0+0.01-0.42);
        glRotatef(playerPosZ*50, 0, 0, 1);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0+0.64, 0.21-0.22, 0+0.01-0.42);
        glRotatef(playerPosZ*50, 0, 0, 1);
        glScalef(2+0.01-1.669999, 0.2+0.14+0.01-0.02, 1-0.7-0.07);
        glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(0+0.64, 0.21-0.22, 0-0.01+0.42);
        glRotatef(playerPosZ*50, 0, 0, 1);
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

void drawTree() {
    glPushMatrix();
        glColor3f(0, 0.4, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.5, 2, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.5, 0.2, 0.1);
        glTranslatef(0, -0.25, 0);
        glScalef(0.4, 0.5, 0.4);
        glutSolidCube(1);
    glPopMatrix();
}

void setWalls() {    
    for (int i=0; i<500; i++) {
        leftWalls[i] = 0;
        midWalls[i]  = 0;
        rightWalls[i]= 0;
    }
    
    for (int i=0, j=0, k=0, l=20; l > -400; l--) {
        //prva traka
        if (((rand() % 1000) / 100) < 3 && l%7 == 0) {
            leftWalls[i] = l-23;
            i++;
        }
        //srednja traka
        if (((rand() % 1000) / 100) < 3 && l%7 == 0) {
            midWalls[j] = l-31;
            j++;
        }
        //treca traka
        if (((rand() % 1000) / 100) < 3 && l%7 == 0) {
            rightWalls[k] = l-27;
            k++;
        }
    }
}

void drawBear() {
    float telo = 0.7;
    glPushMatrix();
        glColor3f(0.5, 0.2, 0.1);
        glTranslatef(0, telo/3, 0);
        glScalef(1.1, 0.4, 0.9);
        glutSolidSphere(telo, 100, 100);
    glPopMatrix();
    
    float glava = 0.8 * telo;
    glPushMatrix();
        glColor3f(0.8, 0.4, 0.2);
        glTranslatef(0, telo, 0);
        glScalef(1, 0.5, 1);
        glutSolidSphere(glava, 100, 100);
    glPopMatrix();
    
    float uvo = 0.25 * telo;
    glPushMatrix();
        glColor3f(0.5, 0.2, 0.1);
        glTranslatef(-0.35, telo+glava-uvo, 0);
        glutSolidSphere(uvo, 100, 100);
    glPopMatrix();
    glPushMatrix();
        glColor3f(0.5, 0.2, 0.1);
        glTranslatef(0.35, telo+glava-uvo, 0);	
        glutSolidSphere(uvo, 100, 100);
    glPopMatrix(); 
}

void drawWalls() {
    for (int i=0; i<500; i++) {
        if (indWalls == 0) {
            setWalls();
            indWalls = 1;
        }
        
        if (leftWalls[i] < playerPosZ+2 && leftWalls[i] > playerPosZ-12) {
            glDisable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glPushMatrix();
                glTranslatef(0.6, -0.6, leftWalls[i]);
                glScalef(lengthOfRoad/4, lengthOfRoad/4, lengthOfRoad/4);
                drawBear();
            glPopMatrix();
            glDisable(GL_COLOR_MATERIAL);
            glEnable(GL_LIGHTING);
        }
        
        if (midWalls[i] < playerPosZ+2 && midWalls[i] > playerPosZ-12) {
            glDisable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glPushMatrix();
                glTranslatef(1.25, -0.6, midWalls[i]);
                glScalef(lengthOfRoad/4, lengthOfRoad/4, lengthOfRoad/4);
                drawBear();
            glPopMatrix();
            glDisable(GL_COLOR_MATERIAL);
            glEnable(GL_LIGHTING);
        }
        
        if (rightWalls[i] < playerPosZ+2 && rightWalls[i] > playerPosZ-12) {
            glDisable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glPushMatrix();
                glTranslatef(1.85, -0.6, rightWalls[i]);
                glScalef(lengthOfRoad/4, lengthOfRoad/4, lengthOfRoad/4);
                drawBear();
            glPopMatrix();
            glDisable(GL_COLOR_MATERIAL);
            glEnable(GL_LIGHTING);
        }
    }
}

int is_crashed() {
    for (int i=0; i<500; i++) {

        if (playerPosX < 2.4) {
            if (leftWalls[i] != 0)
                if (playerPosZ <= leftWalls[i]+1 && playerPosZ >= leftWalls[i]-0.4)
                    return 1;
        }
        else if (playerPosX == 2.5) {
            if (midWalls[i] != 0) 
                if (playerPosZ <= midWalls[i]+1 && playerPosZ >= midWalls[i]-0.4)
                    return 1;
        }
        else {
            if (rightWalls[i] != 0) 
                if (playerPosZ <= rightWalls[i]+1 && playerPosZ >= rightWalls[i]-0.4)
                    return 1;
        }
    }
    return 0;
}

void setCoins() {
    for (int i=0; i<500; i++) {
        leftCoins[i] = 0;
        midCoins[i]  = 0;
        rightCoins[i]= 0;
        
        leftCoinsInd[i] = 0;
        midCoinsInd[i]  = 0;
        rightCoinsInd[i]= 0;
    }
    
    for (int i=0, j=0, k=0, l=20; l > -400; l--) {
        //prva traka
        if (((rand() % 1000) / 100) < 7 && l%7 == 0) {
            leftCoins[i] = l-23;
            i++;
        }
        //srednja traka
        if (((rand() % 1000) / 100) < 7 && l%7 == 0) {
            midCoins[j] = l-31;
            j++;
        }
        //treca traka
        if (((rand() % 1000) / 100) < 7 && l%7 == 0) {
            rightCoins[k] = l-27;
            k++;
        }
    }
}

void drawCoins() {
    for (int i=0; i<500; i++) {
        if (indCoins == 0) {
            setCoins();
            indCoins = 1;
        }
                              //vise se ne iscrtava na istoj poziciji
        if (leftCoins[i] != 0 && leftCoinsInd[i] != 1) {
            setMaterial("coin");
            glPushMatrix();
                glTranslatef(0.63, -0.5, leftCoins[i]);
                glRotatef(playerPosZ*50, 0, 1, 0);
                glScalef(playerLength*1.2, playerLength*1.2, playerLength*1.2);
                glutSolidIcosahedron();
            glPopMatrix();
        }
        if (midCoins[i] != 0 && midCoinsInd[i] != 1) {
            setMaterial("coin");
            glPushMatrix();
                glTranslatef(1.25, -0.5, midCoins[i]);
                glRotatef(playerPosZ*50, 0, 1, 0);
                glScalef(playerLength*1.2, playerLength*1.2, playerLength*1.2);
                glutSolidIcosahedron();
            glPopMatrix();
        }
        if (rightCoins[i] != 0 && rightCoinsInd[i] != 1) {
            setMaterial("coin");
            glPushMatrix();
                glTranslatef(1.85, -0.5, rightCoins[i]);
                glRotatef(playerPosZ*50, 0, 1, 0);
                glScalef(playerLength*1.2, playerLength*1.2, playerLength*1.2);
                glutSolidIcosahedron();
            glPopMatrix();
        }
    }
}

int is_coin_collected() {
    for (int i=0; i<500; i++) {
        
        if (playerPosX < 2.4) {   //i ne moze biti ponovo pokupljen
            if (leftCoins[i] != 0 && leftCoinsInd[i] != 1)
                if (playerPosZ <= leftCoins[i]+0.3 && playerPosZ >= leftCoins[i]-0.2) {
                    leftCoinsInd[i] = 1; //ako je dijamant jednom pokupljen
                    return 1;
                }
        }
        else if (playerPosX == 2.5) {
            if (midCoins[i] != 0 && midCoinsInd[i] != 1) 
                if (playerPosZ <= midCoins[i]+0.3 && playerPosZ >= midCoins[i]-0.2) {
                    midCoinsInd[i] = 1;
                    return 1;
                }
        }
        else {
            if (rightCoins[i] != 0 && rightCoinsInd[i] != 1) 
                if (playerPosZ <= rightCoins[i]+0.3 && playerPosZ >= rightCoins[i]-0.2) {
                    rightCoinsInd[i] = 1;
                    return 1;
                }
        }
    }
    return 0;
}

void writeScore() {
    setMaterial("wall");
    glRasterPos3f(-0.7, 0.5, playerPosZ-2);
    
    char score_display[50] = "Score: ";
    char score_value[50];
    
    sprintf(score_value, " %d ", score);
    strcat(score_display, score_value);
    int len = (int)strlen(score_display);
    for (int i=0; i<len; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score_display[i]);
}

void end_game() {
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
            1.2, 0.4, 2+playerPosZ,
            1.2, 0,   1+playerPosZ,
            0,   1,   0
        );
    
    //postavljamo pozadinu
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, names[1]);

        glBegin(GL_POLYGON);
            glTexCoord2f(0, 0);
            glVertex3f(-10, -17.5, playerPosZ);
            
            glTexCoord2f(1, 0);
            glVertex3f(14, -17.5, playerPosZ);
            
            glTexCoord2f(1, 3);
            glVertex3f(14, 12, playerPosZ-12);
            
            glTexCoord2f(0, 3);
            glVertex3f(-10, 12, playerPosZ-12);
        glEnd();
        
        glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    //ispisujemo poruku na ekran
    char message[50];
    setMaterial("wall");
    
    if (indGameOver) {
        glRasterPos3f(-0.12, 0.1, playerPosZ-2);
        sprintf(message, "GAME OVER"); 
        for (int i=0; i<9; i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }
    else {
        glRasterPos3f(-0.38, 0.1, playerPosZ-2);
        sprintf(message, "CONGRATULATIONS!!!"); 
        for (int i=0; i<18; i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }
    
    glRasterPos3f(-0.13, -0.1, playerPosZ-2);
    sprintf(message, "Your score is %d", score);  
    int len = (int)strlen(message);
    for (int i=0; i<len; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    
    glRasterPos3f(-0.4, -0.3, playerPosZ-2);
    sprintf(message, "Press r/R to start a new game");  
    for (int i=0; i<30; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    
    glutSwapBuffers();
}
