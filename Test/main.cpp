#include <iostream>
#include <stdlib.h>
#include <math.h>

#include <GL/glut.h>
#include "imageloader.h"

const double PI = atan(1) * 4;

GLUquadricObj* quadratic;   //Needed to draw cylinders with glu
GLuint _textureId;          //The OpenGL id of the texture

int milliseconds = 20;      //Time between each screen update
float maxAngle = 50;        //Maximum value for angle
float maxIncrement = 6.5;   //Maximum value for angle increments
float angle = -maxAngle;    //The left sphere will start suspended with the largest valid angle
bool clockwise = false;     //and it will move counter-clockwise

//Spheres
int spheres = 5;            //Number of spheres in total
int movingSpheres = 1;      //Number of spheres that will be in motion
float sphereDiameter = 1.0; //Diameter of each sphere
float sphereCube = 0.125;   //Size of the cube embedded on each sphere

//Base                      //Size for wooden base
float baseX = 7.5;
float baseY = 0.8;
float baseZ = 5.5;
float baseDistance = 1.0;   //Distance between spheres and base when angle is 0

//Pipes
float pipeRadius = 0.125;   //Radius of each cylinder
GLint slices = 32;          //Number of subdivisions around the z axis to draw each cylinder
GLint stacks = 32;          //Number of subdivisions along the z axis to draw each cylinder

float pipeX = 6.5;          //Rectangular box size
float pipeY = 5;            //formed by the tubes
float pipeZ = 4.5;          //(including the diameter of each tube)

//The length of the wire is calculated based on the size of the tubes,
//the diameter of the spheres and the distance between the spheres and the base
float wireLength = pipeY - pipeRadius - baseDistance - sphereDiameter - sphereCube / 2;

//Camera
float cameraX = 0.0;
float cameraY = 0.5;
float cameraZ = 15.0;
float cameraRotationX = 6.0;
float cameraRotationY = 40.0;
float cameraRotationZ = 0.0;

//Axes
bool drawAxes = true;

//Menus
static int menuMain, menuTotalSpheres, menuInMovement;

GLuint loadTexture(Image* image) {
	GLuint textureId;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

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

void positionCamera() {
	glTranslatef(-cameraX, -cameraY, -cameraZ);
	glRotatef(cameraRotationX, 1.0, 0.0, 0.0);
	glRotatef(cameraRotationY, 0.0, 1.0, 0.0);
	glRotatef(cameraRotationZ, 0.0, 0.0, 1.0);
}

double toDeg(double radian) {
	return radian * 180 / PI;
}

double toRad(double degree) {
	return degree * PI / 180;
}

int mediana(int n) {
	//Find the center position for a sequence of numbers
	if (n % 2 == 0) {
		return n / 2;
	}
	else {
		return (n + 1) / 2;
	}
}

void traceAxes(void) {
	//Axis X - Red
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-500.0, 0.0, 0.0);
	glVertex3f(500.0, 0.0, 0.0);
	glEnd();

	//Axis Y - Green
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -500.0, 0.0);
	glVertex3f(0.0, 500.0, 0.0);
	glEnd();

	//Axis Z - Blue
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, -500.0);
	glVertex3f(0.0, 0.0, 500.0);
	glEnd();
}

void drawBase() {
	glPushMatrix();
	glTranslatef(0.0f, -(sphereDiameter / 2 + baseDistance + baseY / 2), 0.0f); //Geometric Center

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	//Upper Face
	glNormal3f(0.0, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);   //Bottom Left
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);    //Bottom Right
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);   //Top Right
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);  //Top Left
	//Underside
	glNormal3f(0.0, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, -baseY / 2, -baseZ / 2);
	//Left Face
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);
	//Right face
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);
	//Front Face
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);
	//Upper side
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawPipes() {
	glPushMatrix();
	glTranslatef(0.0f, pipeY / 2 - sphereDiameter / 2 - baseDistance, 0.0f); //Geometric Center

	glColor3f(0.69f, 0.69f, 0.69f);

	//Lighting
	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 300.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//Superior Front
	glPushMatrix();
	glTranslatef(-pipeX / 2, pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeX, slices, stacks);
	glPopMatrix();

	//Upper Rear
	glPushMatrix();
	glTranslatef(-pipeX / 2, pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeX, slices, stacks);
	glPopMatrix();

	//Left Forward
	glPushMatrix();
	glTranslatef(-(pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, slices, stacks);
	glPopMatrix();

	//Left Rear
	glPushMatrix();
	glTranslatef(-(pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, slices, stacks);
	glPopMatrix();

	//Right Forward
	glPushMatrix();
	glTranslatef((pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, slices, stacks);
	glPopMatrix();

	//Rear Right
	glPushMatrix();
	glTranslatef((pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, slices, stacks);
	glPopMatrix();

	glPopMatrix();
}

void drawSphere(float angle) {
	glPushMatrix();

	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -wireLength, 0.0f);

	glColor3f(0.8, 0.8, 0.8);

	//Lighting
	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 300.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//Draw cube
	glutSolidCube(sphereCube);

	//Draw sphere
	glPushMatrix();
	glTranslatef(0.0f, -(sphereDiameter / 2), 0.0f);
	glutSolidSphere(sphereDiameter / 2, 20, 20);
	glPopMatrix();

	glRotatef(-angle, 0.0f, 0.0f, 1.0f);

	//Draw wires
	float distX = sin(toRad(angle)) * wireLength;
	float distY = cos(toRad(angle)) * wireLength;
	float distZ = pipeZ / 2 - pipeRadius;

	glColor3f(0.72f, 0.54f, 0.0f);
	glBegin(GL_LINES);
	//Towards back pipe
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-distX, distY, -distZ);
	//To head pipe
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-distX, distY, distZ);
	glEnd();

	glPopMatrix();
}

void drawSpheres() {
	glPushMatrix();
	glTranslatef(0.0f, pipeY - pipeRadius - baseDistance - sphereDiameter / 2, 0.0f); //Pipe height

	for (int i = 1; i <= spheres; i++) {
		glPushMatrix();
		glTranslatef(-spheres / 2.0f - sphereDiameter / 2.0f + i * sphereDiameter, 0.0f, 0.0f); //Center in X

		if (i <= movingSpheres && angle < 0) {
			drawSphere(angle);
		}
		else if (i > spheres - movingSpheres && angle > 0) {
			drawSphere(angle);
		}
		else {
			drawSphere(0);
		}

		glPopMatrix();
	}

	glPopMatrix();
}

void drawScene() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Black background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Lighting
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPos[] = { -cameraX + pipeX, -cameraY + pipeY, -cameraZ + pipeZ, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	positionCamera();

	if (drawAxes)
		traceAxes();

	drawBase();
	drawPipes();
	drawSpheres();

	glutSwapBuffers();
}

void update(int value) {
	float incremento = maxIncrement - abs(angle) / maxAngle * maxIncrement * 0.85;

	if (clockwise and angle <= -maxAngle) {
		clockwise = false;
	}
	else if (!clockwise and angle >= maxAngle) {
		clockwise = true;
	}

	if (clockwise)
		angle -= incremento;
	else
		angle += incremento;

	glutPostRedisplay();
	glutTimerFunc(milliseconds, update, 0);
}

void handleSpecialKeys(int key, int x, int y) {
	int inc = 2.0;

	switch (key) {
		case GLUT_KEY_RIGHT:
			cameraRotationY += inc;
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			cameraRotationY -= inc;
			glutPostRedisplay();
			break;
		case GLUT_KEY_UP:
			cameraRotationX -= inc;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			cameraRotationX += inc;
			glutPostRedisplay();
			break;
	}
}

void handleResize(int w, int h) {
	//Projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);

	//ViewPort
	glViewport(0, 0, w, h);
}

void handleMenuMain(int m) {
	switch (m) {
		case 0:
			exit(0);
		case 1:
			drawAxes = !drawAxes;
	}
}

void handleMenuTotalSpheres(int m) {
	spheres = m - 10;
	glutPostRedisplay();
}

void handleMenuInMotion(int m) {
	movingSpheres = m - 20;
	glutPostRedisplay();
}

void generateMenu() {
	menuTotalSpheres = glutCreateMenu(handleMenuTotalSpheres);

	glutAddMenuEntry("1", 11);
	glutAddMenuEntry("2", 12);
	glutAddMenuEntry("3", 13);
	glutAddMenuEntry("4", 14);
	glutAddMenuEntry("5", 15);
	glutAddMenuEntry("6", 16);
	glutAddMenuEntry("7", 17);

	menuInMovement = glutCreateMenu(handleMenuInMotion);

	glutAddMenuEntry("1", 21);
	glutAddMenuEntry("2", 22);
	glutAddMenuEntry("3", 23);
	glutAddMenuEntry("4", 24);
	glutAddMenuEntry("5", 25);
	glutAddMenuEntry("6", 26);
	glutAddMenuEntry("7", 27);

	menuMain = glutCreateMenu(handleMenuMain);

	glutAddSubMenu("Total spheres", menuTotalSpheres);
	glutAddSubMenu("Total in movement", menuInMovement);
	glutAddMenuEntry("Show Axes", 1);
	glutAddMenuEntry("Leave", 0);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);

	glutCreateWindow("Newton's cradle");
	initRendering();
	generateMenu();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);
	glutSpecialFunc(handleSpecialKeys);
	glutTimerFunc(milliseconds, update, 0);

	glutMainLoop();
	return 0;
}
