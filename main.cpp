#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "imageloader.h"

GLUquadricObj* gobj;
GLuint woodTextureId;

const float SPHERE_D = 1.0;
const float SPHERE_CUBE_SIZE = 0.125;

const float BASE_X = 7.5;
const float BASE_Y = 0.75;
const float BASE_Z = 5.5;
const float BASE_DISTANCE = 1.0;

const float TUBE_X = 6.5;
const float TUBE_Y = 5.0;
const float TUBE_Z = 4.5;
const float TUBE_RADIUS = 0.125;

const float CAMERA_X = 0.0;
const float CAMERA_Y = 0.5;
const float CAMERA_Z = 15.0;

const float WIRE_L = TUBE_Y - TUBE_RADIUS - BASE_DISTANCE - SPHERE_D - SPHERE_CUBE_SIZE / 2;

float cameraRotationX = 5.0;
float cameraRotationY = 45.0;
float cameraRotationZ = 0.0;

float angle = 45.0;                      // Угол отклонения
float freq = 1000 / 50;                  // Частота смены кадров (млс)
float A = WIRE_L;                        // Амплитуда колебаний, максимальное отклонение груза (м)
float T = 2 * M_PI * sqrt(WIRE_L / 9.8); // Период колебаний (с)
float v = 1 / T;                         // Частота колебаний (Гц)
float omega = 2 * M_PI * v;              // Циклическая частота колебаний (рад/с)
float fi0 = M_PI / 4;                    // Начальная фаза колебания (рад)
float t = 0;                             // Время (c)

GLuint loadTexture(Image* image) {
	GLuint woodTextureId;

	glGenTextures(1, &woodTextureId);
	glBindTexture(GL_TEXTURE_2D, woodTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	return woodTextureId;
}

void drawSphere(float angle) {
	glPushMatrix();

	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -WIRE_L, 0.0f);

	glColor3f(0.675f, 0.675f, 0.750f);

	glutSolidCube(SPHERE_CUBE_SIZE);

	glPushMatrix();

	glTranslatef(0.0f, -(SPHERE_D / 2), 0.0f);
	glutSolidSphere(SPHERE_D / 2, 100, 100);

	glPopMatrix();

	glRotatef(-angle, 0.0f, 0.0f, 1.0f);

	glPopMatrix();
}

void drawWire(float angle) {
	glPushMatrix();

	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -WIRE_L, 0.0f);
	glRotatef(-angle, 0.0f, 0.0f, 1.0f);

	float x = sin(angle * M_PI / 180) * WIRE_L;
	float y = cos(angle * M_PI / 180) * WIRE_L;
	float z = TUBE_Z / 2 - TUBE_RADIUS;

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, y, -z);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, y, z);

	glEnd();

	glPopMatrix();
}

void drawTubes() {
	glPushMatrix();
	glTranslatef(0.0f, TUBE_Y / 2 - SPHERE_D / 2 - BASE_DISTANCE, 0.0f);

	glColor3f(0.675f, 0.675f, 0.750f);

	//Top Front
	glPushMatrix();
	glTranslatef(-TUBE_X / 2 + TUBE_RADIUS, TUBE_Y / 2 - TUBE_RADIUS, TUBE_Z / 2 - TUBE_RADIUS);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(gobj, TUBE_RADIUS, TUBE_RADIUS, TUBE_X - TUBE_RADIUS * 2.0, 50, 50);
	glPopMatrix();

	//Top Rear
	glPushMatrix();
	glTranslatef(-TUBE_X / 2 + TUBE_RADIUS, TUBE_Y / 2 - TUBE_RADIUS, -(TUBE_Z / 2 - TUBE_RADIUS));
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(gobj, TUBE_RADIUS, TUBE_RADIUS, TUBE_X - TUBE_RADIUS * 2.0, 50, 50);
	glPopMatrix();

	//Left Front
	glPushMatrix();
	glTranslatef(-(TUBE_X / 2 - TUBE_RADIUS), TUBE_Y / 2 - TUBE_RADIUS, TUBE_Z / 2 - TUBE_RADIUS);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, TUBE_RADIUS, TUBE_RADIUS, GLdouble(TUBE_Y) - TUBE_RADIUS, 50, 50);
	glutSolidSphere(TUBE_RADIUS, 50, 50);
	glPopMatrix();

	//Left Rear
	glPushMatrix();
	glTranslatef(-(TUBE_X / 2 - TUBE_RADIUS), TUBE_Y / 2 - TUBE_RADIUS, -(TUBE_Z / 2 - TUBE_RADIUS));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, TUBE_RADIUS, TUBE_RADIUS, GLdouble(TUBE_Y) - TUBE_RADIUS, 50, 50);
	glutSolidSphere(TUBE_RADIUS, 50, 50);
	glPopMatrix();

	//Right Front
	glPushMatrix();
	glTranslatef((TUBE_X / 2 - TUBE_RADIUS), TUBE_Y / 2 - TUBE_RADIUS, TUBE_Z / 2 - TUBE_RADIUS);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, TUBE_RADIUS, TUBE_RADIUS, GLdouble(TUBE_Y) - TUBE_RADIUS, 50, 50);
	glutSolidSphere(TUBE_RADIUS, 50, 50);
	glPopMatrix();

	//Right Rear
	glPushMatrix();
	glTranslatef((TUBE_X / 2 - TUBE_RADIUS), TUBE_Y / 2 - TUBE_RADIUS, -(TUBE_Z / 2 - TUBE_RADIUS));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, TUBE_RADIUS, TUBE_RADIUS, GLdouble(TUBE_Y) - TUBE_RADIUS, 50, 50);
	glutSolidSphere(TUBE_RADIUS, 50, 50);
	glPopMatrix();

	glPopMatrix();
}

void drawBase() {
	glPushMatrix();
	glTranslatef(0.0f, -(SPHERE_D / 2 + BASE_DISTANCE + BASE_Y / 2), 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

	//Up
	glNormal3f(0.0, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BASE_X / 2, BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BASE_X / 2, BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BASE_X / 2, BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BASE_X / 2, BASE_Y / 2, -BASE_Z / 2);

	//Bottom
	glNormal3f(0.0, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BASE_X / 2, -BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BASE_X / 2, -BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BASE_X / 2, -BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BASE_X / 2, -BASE_Y / 2, -BASE_Z / 2);

	//Left
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BASE_X / 2, -BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-BASE_X / 2, -BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-BASE_X / 2, BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BASE_X / 2, BASE_Y / 2, -BASE_Z / 2);

	//Right
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(BASE_X / 2, -BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BASE_X / 2, -BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BASE_X / 2, BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(BASE_X / 2, BASE_Y / 2, -BASE_Z / 2);

	//Front
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BASE_X / 2, -BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BASE_X / 2, -BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BASE_X / 2, BASE_Y / 2, BASE_Z / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BASE_X / 2, BASE_Y / 2, BASE_Z / 2);

	//Rear
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BASE_X / 2, -BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BASE_X / 2, -BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BASE_X / 2, BASE_Y / 2, -BASE_Z / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BASE_X / 2, BASE_Y / 2, -BASE_Z / 2);

	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawAxios() {
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-800.0, 0.0, 0.0);
	glVertex3f(800.0, 0.0, 0.0);
	glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -800.0, 0.0);
	glVertex3f(0.0, 800.0, 0.0);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, -800.0);
	glVertex3f(0.0, 0.0, 800.0);
	glEnd();
}

void displayFunc() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-CAMERA_X, -CAMERA_Y, -CAMERA_Z);
	glRotatef(cameraRotationX, 1.0, 0.0, 0.0);
	glRotatef(cameraRotationY, 0.0, 1.0, 0.0);
	glRotatef(cameraRotationZ, 0.0, 0.0, 1.0);

	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	GLfloat ambientMaterial[] = { 0.25, 0.25, 0.25, 0.25 };
	GLfloat diffuseMaterial[] = { 0.25, 0.25, 0.25, 0.25 };
	GLfloat specularMaterial[] = { 0.25, 0.25, 0.25, 0.25 };
	GLfloat shininessMaterial[] = { 25.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininessMaterial);

	drawAxios();
	drawBase();
	drawTubes();

	//glPushMatrix();

	//glTranslatef(0.0f, TUBE_Y - TUBE_RADIUS - BASE_DISTANCE - SPHERE_D / 2.0f, 0.0f);

	//for (int i = 1; i <= 5; i++) {
	//	glPushMatrix();

	//	glTranslatef(-5 / 2.0f - SPHERE_D / 2.0f + i * SPHERE_D, 0.0f, 0.0f);

	//	if (i == 1 && angle < 0 || i == 5 && angle > 0) {
	//		drawWire(angle);
	//		drawSphere(angle);
	//	}
	//	else {
	//		drawWire(0);
	//		drawSphere(0);
	//	}

	//	glPopMatrix();
	//}

	//glPopMatrix();

	glutSwapBuffers();
}

void reshapeFunc(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
	glViewport(0, 0, w, h);
}

void specialFunc(int key, int x, int y) {
	int increment = 2.0;

	switch (key) {
	case GLUT_KEY_RIGHT:
		cameraRotationY += increment;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		cameraRotationY -= increment;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		cameraRotationX -= increment;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		cameraRotationX += increment;
		glutPostRedisplay();
		break;
	}
}

void timerFunc(int value) {
	float x = A * cos(omega * t + fi0);                                            // Координата, смещение груза от положения равновесия 
	float c = sqrt(WIRE_L * WIRE_L + x * x);                               // Пользуясь теоремой косинусов
	float cosA = (x * x + c * c - WIRE_L * WIRE_L) / (2 * x * WIRE_L);
	float angleA = cosA * (180 / M_PI);

	angle = angleA;
	t += freq / 1000;

	glutPostRedisplay();
	glutTimerFunc(freq, timerFunc, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);

	glutCreateWindow("Newton's cradle");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	gobj = gluNewQuadric();

	gluQuadricNormals(gobj, GLU_SMOOTH);
	gluQuadricTexture(gobj, GL_TRUE);

	woodTextureId = loadTexture(loadBMP("wood.bmp"));

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutSpecialFunc(specialFunc);
	glutTimerFunc(freq, timerFunc, 0);

	glutMainLoop();

	return 0;
}