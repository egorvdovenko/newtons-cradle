#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "imageloader.h"

const double PI = atan(1) * 4;

GLUquadricObj* quadratic;  //Needed to draw cylinders with glu
GLuint textureId;          //The OpenGL id of the texture

int millis = 20;            //Time between each screen update
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
GLint pipeSlices = 32;          //Number of subdivisions around the z axis to draw each cylinder
GLint pipeStacks = 32;          //Number of subdivisions along the z axis to draw each cylinder
float pipeRadius = 0.125;   //Radius of each cylinder
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

GLuint loadTexture(Image* image) {
	GLuint textureId;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	return textureId;
}

void drawSphere(float angle) {
	glPushMatrix();

	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -wireLength, 0.0f);

	glColor3f(0.8, 0.8, 0.8);

	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 300.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glutSolidCube(sphereCube);

	glPushMatrix();
	glTranslatef(0.0f, -(sphereDiameter / 2), 0.0f);
	glutSolidSphere(sphereDiameter / 2, 20, 20);
	glPopMatrix();

	glRotatef(-angle, 0.0f, 0.0f, 1.0f);

	float distX = sin(angle * PI / 180) * wireLength;
	float distY = cos(angle * PI / 180) * wireLength;
	float distZ = pipeZ / 2 - pipeRadius;

	glColor3f(0.72f, 0.54f, 0.0f);
	glBegin(GL_LINES);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-distX, distY, -distZ);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-distX, distY, distZ);
	glEnd();

	glPopMatrix();
}

void drawSpheres() {
	glPushMatrix();
	glTranslatef(0.0f, pipeY - pipeRadius - baseDistance - sphereDiameter / 2, 0.0f);

	for (int i = 1; i <= spheres; i++) {
		glPushMatrix();
		glTranslatef(-spheres / 2.0f - sphereDiameter / 2.0f + i * sphereDiameter, 0.0f, 0.0f);

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

void drawPipes() {
	glPushMatrix();
	glTranslatef(0.0f, pipeY / 2 - sphereDiameter / 2 - baseDistance, 0.0f);

	glColor3f(0.69f, 0.69f, 0.69f);

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
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeX, pipeSlices, pipeStacks);
	glPopMatrix();

	//Upper Rear
	glPushMatrix();
	glTranslatef(-pipeX / 2, pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeX, pipeSlices, pipeStacks);
	glPopMatrix();

	//Left Forward
	glPushMatrix();
	glTranslatef(-(pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, pipeSlices, pipeStacks);
	glPopMatrix();

	//Left Rear
	glPushMatrix();
	glTranslatef(-(pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, pipeSlices, pipeStacks);
	glPopMatrix();

	//Right Forward
	glPushMatrix();
	glTranslatef((pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, pipeZ / 2 - pipeRadius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, pipeSlices, pipeStacks);
	glPopMatrix();

	//Rear Right
	glPushMatrix();
	glTranslatef((pipeX / 2 - pipeRadius), pipeY / 2 - pipeRadius, -(pipeZ / 2 - pipeRadius));
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, pipeRadius, pipeRadius, pipeY - pipeRadius, pipeSlices, pipeStacks);
	glPopMatrix();

	glPopMatrix();
}

void drawBase() {
	glPushMatrix();
	glTranslatef(0.0f, -(sphereDiameter / 2 + baseDistance + baseY / 2), 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

	//Upper
	glNormal3f(0.0, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(baseX / 2, baseY / 2, baseZ / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(baseX / 2, baseY / 2, -baseZ / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-baseX / 2, baseY / 2, -baseZ / 2);

	//Under
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
	
	//Upper
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
	GLfloat lightPos[] = { -cameraX + pipeX, -cameraY + pipeY, -cameraZ + pipeZ, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
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
	glutTimerFunc(millis, timerFunc, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);

	glutCreateWindow("Newton's cradle");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	quadratic = gluNewQuadric();

	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);

	Image* image = loadBMP("wood.bmp");
	textureId = loadTexture(image);
	delete image;

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutSpecialFunc(specialFunc);
	glutTimerFunc(millis, timerFunc, 0);

	glutMainLoop();
	return 0;
}