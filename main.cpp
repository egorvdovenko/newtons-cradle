#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "imageloader.h"

GLUquadricObj* gobj;
GLuint woodTextureId;

float angle = -50;
float maxAngle = 50;        
float maxIncrement = 6.5;   
bool clockwise = false;

float sphereDiameter = 1.0;
float sphereCube = 0.125;

float baseX = 7.5;
float baseY = 0.8;
float baseZ = 5.5;
float baseDistance = 1.0;

float pipeRadius = 0.125;
float pipeX = 6.5;
float pipeY = 5.0;
float pipeZ = 4.5;

float wireLength = pipeY - pipeRadius - baseDistance - sphereDiameter - sphereCube / 2;

float cameraX = 0.0;
float cameraY = 0.5;
float cameraZ = 15.0;
float cameraRotationX = 6.0;
float cameraRotationY = 40.0;
float cameraRotationZ = 0.0;

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
	glTranslatef(0.0f, -wireLength, 0.0f);

	glColor3f(0.675f, 0.675f, 0.750f);

	GLfloat ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 300.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glutSolidCube(sphereCube);

	glPushMatrix();

	glTranslatef(0.0f, -(sphereDiameter / 2), 0.0f);
	glutSolidSphere(sphereDiameter / 2, 20, 20);

	glPopMatrix();

	glRotatef(-angle, 0.0f, 0.0f, 1.0f);

	float x = sin(angle * M_PI / 180) * wireLength;
	float y = cos(angle * M_PI / 180) * wireLength;
	float z = pipeZ / 2 - pipeRadius;

	glColor3f(0.72f, 0.54f, 0.0f);
	glBegin(GL_LINES);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, y, -z);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, y, z);
	glEnd();

	glPopMatrix();
}

void drawSpheres() {
	glPushMatrix();
	glTranslatef(0.0f, pipeY - pipeRadius - baseDistance - sphereDiameter / 2.0f, 0.0f);

	for (int i = 1; i <= 5; i++) {
		glPushMatrix();
		glTranslatef(-5 / 2.0f - sphereDiameter / 2.0f + i * sphereDiameter, 0.0f, 0.0f);

		if (i == 1 && angle < 0 || i == 5 && angle > 0) {
			drawSphere(angle);
		}
		else {
			drawSphere(0);
		}

		glPopMatrix();
	}

	glPopMatrix();
}

void drawPipes() {
	glPushMatrix();
	glTranslatef(0.0f, pipeY / 2 - sphereDiameter / 2 - baseDistance, 0.0f);

	glColor3f(0.675f, 0.675f, 0.750f);

	GLfloat ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 300.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	//Top Front
	glPushMatrix();
	glTranslatef(-pipeX / 2, pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(gobj, pipeRadius, pipeRadius, pipeX, 32, 32);
	glPopMatrix();

	//Top Rear
	glPushMatrix();
	glTranslatef(-pipeX / 2, pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(gobj, pipeRadius, pipeRadius, pipeX, 32, 32);
	glPopMatrix();

	//Left Front
	glPushMatrix();
	glTranslatef(-(pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, pipeRadius, pipeRadius, GLfloat(pipeY - pipeRadius), 32, 32);
	glPopMatrix();

	//Left Rear
	glPushMatrix();
	glTranslatef(-(pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, pipeRadius, pipeRadius, GLfloat(pipeY - pipeRadius), 32, 32);
	glPopMatrix();

	//Right Front
	glPushMatrix();
	glTranslatef((pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, pipeRadius, pipeRadius, GLfloat(pipeY - pipeRadius), 32, 32);
	glPopMatrix();

	//Right Rear
	glPushMatrix();
	glTranslatef((pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gobj, pipeRadius, pipeRadius, GLfloat(pipeY - pipeRadius), 32, 32);
	glPopMatrix();

	glPopMatrix();
}

void drawBase() {
	glPushMatrix();
	glTranslatef(0.0f, -(sphereDiameter / 2 + baseDistance + baseY / 2), 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

	//Up
	glNormal3f(0.0, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);

	//Bottom
	glNormal3f(0.0, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, -baseY / 2, -baseZ / 2);

	//Left
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);

	//Right
	glNormal3f(-1.0, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);

	//Front
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);

	//Rear
	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, -baseY / 2, -baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void displayFunc() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glTranslatef(-cameraX, -cameraY, -cameraZ);
	glRotatef(cameraRotationX, 1.0, 0.0, 0.0);
	glRotatef(cameraRotationY, 0.0, 1.0, 0.0);
	glRotatef(cameraRotationZ, 0.0, 0.0, 1.0);

	drawBase();
	drawPipes();
	drawSpheres();

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
	float increment = maxIncrement - (abs(angle) / maxAngle * maxIncrement * 0.85);

	if (clockwise) {
		if (angle <= -maxAngle) {
			clockwise = false;
		}
	}
	else {
		if (angle >= maxAngle) {
			clockwise = true;
		}
	}

	clockwise
		? angle -= increment
		: angle += increment;

	glutPostRedisplay();
	glutTimerFunc(20, timerFunc, 0);
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
	glutTimerFunc(20, timerFunc, 0);

	glutMainLoop();

	return 0;
}