#include <iostream>
#include <stdlib.h>
#include <math.h>

#include <GL/glut.h>
#include "imageloader.h"

const double PI = atan(1) * 4;

GLUquadricObj* quadratic;   //Necesario para dibujar cilindros con glu
GLuint _textureId;          //The OpenGL id of the texture
int milisegundos = 20;      //Tiempo entre cada actualización de pantalla
float angulomax = 50;       //Valor máximo para el ángulo
float incrementomax = 6.5;  //Valor máximo para los incrementos de ángulo
float angulo = -angulomax;  //La esfera izquierda iniciará suspendida con el mayor ángulo válido
bool clockwise = false;     //y se trasladará en sentido counter-clockwise

//Esferas
int esferas = 5;            //Cantidad de esferas en total
int semueven = 1;           //Cantidad de esferas que estarán en movimiento
float esf_diam = 1.0;       //Diámetro de cada esfera
float cuboamarrar = 0.125;   //Tamaño del cubo incrustado sobre cada esfera

//Base                      //Tamaño para la base de madera
float base_tamX = 7.5;
float base_tamY = 0.8;
float base_tamZ = 5.5;

float dist_esf_base = 1.0;  //Distancia entre las esferas y la base cuando el ángulo es 0

//Tubos
float tub_radio = 0.125;    //Radio de cada cilindro
GLint slices = 32;          //Cantidad de subdivisiones alrededor del eje z para dibujar cada cilindro
GLint stacks = 32;          //Cantidad de subdivisiones a lo largo del eje z para dibujar cada cilindro

float tub_tamX = 6.5;       // Tamaño de la cajeta rectangular 
float tub_tamY = 5;      // formada por los tubos (incluyendo 
float tub_tamZ = 4.5;       // el diámetro de cada tubo)

//El largo del hilo se calcula en base al tamaño de los tubos, 
// el diámetro de las esferas y la distancia entre las esferas y la base
float largo_hilo = tub_tamY - tub_radio - dist_esf_base - esf_diam - cuboamarrar / 2;

//Camara
float camposx = 0.0;
float camposy = 0.5;
float camposz = 15.0;
float camrotx = 6.0;
float camroty = 40.0;
float camrotz = 0.0;

//Ejes
bool dibujarEjes = false;

//Menus
static int menuPrincipal, menuTotalEsf, menuEnMovimiento;

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width, image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);

	Image* image = loadBMP("madera.bmp");
	_textureId = loadTexture(image);
	delete image;
}

void posicionarCamara() {
	glTranslatef(-camposx, -camposy, -camposz);
	glRotatef(camrotx, 1.0, 0.0, 0.0);
	glRotatef(camroty, 0.0, 1.0, 0.0);
	glRotatef(camrotz, 0.0, 0.0, 1.0);
}

double toDeg(double radian) {
	return radian * 180 / PI;
}

double toRad(double degree) {
	return degree * PI / 180;
}

int mediana(int n) {
	//Encontrar la posición central para una secuencia de n números
	if (n % 2 == 0)
		return n / 2;
	else
		return (n + 1) / 2;
}

void trazarEjes(void) {
	//Eje X - Rojo
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-500.0, 0.0, 0.0);
	glVertex3f(500.0, 0.0, 0.0);
	glEnd();

	//Eje Y - Verde
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -500.0, 0.0);
	glVertex3f(0.0, 500.0, 0.0);
	glEnd();

	//Eje Z - Azul
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, -500.0);
	glVertex3f(0.0, 0.0, 500.0);
	glEnd();
}

void dibujarBase() {
	glPushMatrix();
	glTranslatef(0.0f, -(esf_diam / 2 + dist_esf_base + base_tamY / 2), 0.0f); //Centro Geométrico de la Base

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	//Cara Superior
	glNormal3f(0.0, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_tamX / 2, base_tamY / 2, base_tamZ / 2); //Bottom Left of Texture & Plane
	glTexCoord2f(1.0f, 0.0f); glVertex3f(base_tamX / 2, base_tamY / 2, base_tamZ / 2); // Bottom Right of Texture & Plane
	glTexCoord2f(1.0f, 1.0f); glVertex3f(base_tamX / 2, base_tamY / 2, -base_tamZ / 2); // Top Right of Texture & Plane
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_tamX / 2, base_tamY / 2, -base_tamZ / 2); // Top Left of Texture & Plane
	//Cara Inferior
	glNormal3f(0.0, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_tamX / 2, -base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(base_tamX / 2, -base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(base_tamX / 2, -base_tamY / 2, -base_tamZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_tamX / 2, -base_tamY / 2, -base_tamZ / 2);
	//Cara Izq
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_tamX / 2, -base_tamY / 2, -base_tamZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-base_tamX / 2, -base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-base_tamX / 2, base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_tamX / 2, base_tamY / 2, -base_tamZ / 2);
	//Cara Der
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(base_tamX / 2, -base_tamY / 2, -base_tamZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(base_tamX / 2, -base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(base_tamX / 2, base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(base_tamX / 2, base_tamY / 2, -base_tamZ / 2);
	//Cara Frontal
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_tamX / 2, -base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(base_tamX / 2, -base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(base_tamX / 2, base_tamY / 2, base_tamZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_tamX / 2, base_tamY / 2, base_tamZ / 2);
	//Cara Posterior
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_tamX / 2, -base_tamY / 2, -base_tamZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(base_tamX / 2, -base_tamY / 2, -base_tamZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(base_tamX / 2, base_tamY / 2, -base_tamZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_tamX / 2, base_tamY / 2, -base_tamZ / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void dibujarTubos() {
	glPushMatrix();
	glTranslatef(0.0f, tub_tamY / 2 - esf_diam / 2 - dist_esf_base, 0.0f); //Centro Geométrico

	glColor3f(0.69f, 0.69f, 0.69f);

	//Iluminación
	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 300.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//Superior Delantero
	glPushMatrix();
	glTranslatef(-tub_tamX / 2, tub_tamY / 2 - tub_radio, tub_tamZ / 2 - tub_radio);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadratic, tub_radio, tub_radio, tub_tamX, slices, stacks);
	glPopMatrix();

	//Superior Posterior
	glPushMatrix();
	glTranslatef(-tub_tamX / 2, tub_tamY / 2 - tub_radio, -(tub_tamZ / 2 - tub_radio));
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadratic, tub_radio, tub_radio, tub_tamX, slices, stacks);
	glPopMatrix();

	//Izquierdo Delantero
	glPushMatrix();
	glTranslatef(-(tub_tamX / 2 - tub_radio), tub_tamY / 2 - tub_radio, tub_tamZ / 2 - tub_radio);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, tub_radio, tub_radio, tub_tamY - tub_radio, slices, stacks);
	glPopMatrix();

	//Izquierdo Posterior
	glPushMatrix();
	glTranslatef(-(tub_tamX / 2 - tub_radio), tub_tamY / 2 - tub_radio, -(tub_tamZ / 2 - tub_radio));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, tub_radio, tub_radio, tub_tamY - tub_radio, slices, stacks);
	glPopMatrix();

	//Derecho Delantero
	glPushMatrix();
	glTranslatef((tub_tamX / 2 - tub_radio), tub_tamY / 2 - tub_radio, tub_tamZ / 2 - tub_radio);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, tub_radio, tub_radio, tub_tamY - tub_radio, slices, stacks);
	glPopMatrix();

	//Derecho Posterior
	glPushMatrix();
	glTranslatef((tub_tamX / 2 - tub_radio), tub_tamY / 2 - tub_radio, -(tub_tamZ / 2 - tub_radio));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, tub_radio, tub_radio, tub_tamY - tub_radio, slices, stacks);
	glPopMatrix();

	glPopMatrix();
}

void esferaAmarrada(float angulo) {
	glPushMatrix();

	glRotatef(angulo, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -largo_hilo, 0.0f);

	glColor3f(0.8, 0.8, 0.8);

	//Iluminación
	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 300.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//Dibujar Cubo Para Amarrar
	glutSolidCube(cuboamarrar);

	//Dibujar Esfera
	glPushMatrix();
	glTranslatef(0.0f, -(esf_diam / 2), 0.0f);
	glutSolidSphere(esf_diam / 2, 20, 20);
	glPopMatrix();

	glRotatef(-angulo, 0.0f, 0.0f, 1.0f);

	//Dibujar hilos
	float distX = sin(toRad(angulo)) * largo_hilo;
	float distY = cos(toRad(angulo)) * largo_hilo;
	float distZ = tub_tamZ / 2 - tub_radio;

	glColor3f(0.72f, 0.54f, 0.0f);
	glBegin(GL_LINES);
	//Hacia tubo posterior
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-distX, distY, -distZ);
	//Hacia tubo frontal
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-distX, distY, distZ);
	glEnd();

	glPopMatrix();
}

void dibujarEsferas() {
	glPushMatrix();
	glTranslatef(0.0f, tub_tamY - tub_radio - dist_esf_base - esf_diam / 2, 0.0f); //Altura de los tubos

	for (int i = 1; i <= esferas; i++) {
		glPushMatrix();
		glTranslatef(-esferas / 2.0f - esf_diam / 2.0f + i * esf_diam, 0.0f, 0.0f); //Centro en X
		if (i <= semueven && angulo < 0)
			esferaAmarrada(angulo);
		else if (i > esferas - semueven && angulo > 0)
			esferaAmarrada(angulo);
		else
			esferaAmarrada(0);
		glPopMatrix();
	}

	glPopMatrix();
}

void drawScene() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //fondo negro
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/** Iluminación **/
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPos[] = { -camposx + tub_tamX, -camposy + tub_tamY, -camposz + tub_tamZ, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	/** Viewing **/
	posicionarCamara();

	/** Modeling **/
	if (dibujarEjes)
		trazarEjes();
	dibujarBase();
	dibujarTubos();
	dibujarEsferas();

	glutSwapBuffers();
}

void update(int value) {
	float incremento = incrementomax - abs(angulo) / angulomax * incrementomax * 0.85;

	if (clockwise and angulo <= -angulomax) {
		clockwise = false;
	}
	else if (!clockwise and angulo >= angulomax) {
		clockwise = true;
	}

	if (clockwise)
		angulo -= incremento;
	else
		angulo += incremento;

	glutPostRedisplay();
	glutTimerFunc(milisegundos, update, 0);
}

void handleSpecialKeys(int key, int x, int y) {
	int inc = 2.0;
	switch (key) {
	case GLUT_KEY_RIGHT:
		camroty += inc;
		glutPostRedisplay();
		break;

	case GLUT_KEY_LEFT:
		camroty -= inc;
		glutPostRedisplay();
		break;

	case GLUT_KEY_UP:
		camrotx -= inc;
		glutPostRedisplay();
		break;

	case GLUT_KEY_DOWN:
		camrotx += inc;
		glutPostRedisplay();
		break;
	}
}

void handleResize(int w, int h) {
	/** Projection **/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);

	/** ViewPort **/
	glViewport(0, 0, w, h);
}

void handleMenuPrincipal(int m) {
	switch (m) {
	case 0:
		exit(0);
	case 1:
		dibujarEjes = !dibujarEjes;
	}
}

void handleMenuTotalEsf(int m) {
	esferas = m - 10;
	glutPostRedisplay();
}

void handleMenuEnMovimiento(int m) {
	semueven = m - 20;
	glutPostRedisplay();
}

void prepararMenu() {
	menuTotalEsf = glutCreateMenu(handleMenuTotalEsf);
	glutAddMenuEntry("1", 11);
	glutAddMenuEntry("2", 12);
	glutAddMenuEntry("3", 13);
	glutAddMenuEntry("4", 14);
	glutAddMenuEntry("5", 15);
	glutAddMenuEntry("6", 16);
	glutAddMenuEntry("7", 17);

	menuEnMovimiento = glutCreateMenu(handleMenuEnMovimiento);
	glutAddMenuEntry("1", 21);
	glutAddMenuEntry("2", 22);
	glutAddMenuEntry("3", 23);
	glutAddMenuEntry("4", 24);
	glutAddMenuEntry("5", 25);
	glutAddMenuEntry("6", 26);
	glutAddMenuEntry("7", 27);

	menuPrincipal = glutCreateMenu(handleMenuPrincipal);
	glutAddSubMenu("Total de Esferas", menuTotalEsf);
	glutAddSubMenu("En Movimiento", menuEnMovimiento);
	glutAddMenuEntry("Mostrar Ejes", 1);
	glutAddMenuEntry("Salir", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);

	glutCreateWindow("Péndulo de Newton");
	initRendering();
	prepararMenu();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);
	glutSpecialFunc(handleSpecialKeys);
	glutTimerFunc(milisegundos, update, 0);

	glutMainLoop();
	return 0;
}
