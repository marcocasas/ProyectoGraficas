#include <iostream>
#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"

using namespace std;

GLfloat pos_x = 10, pos_z = 10; //Position in z-axis of the sphere.

GLint winWidth = 500, winHeight = 500; // Initial display-window size.

void init(void)
{
	GLfloat x0 = 50, y0 = 50, z0 = 50; // Viewing-coordinate origin.
	GLfloat xref = 50.0, yref = 50.0, zref = 0.0; // Look-at point.
	GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0; // View-up vector.

	/* Set coordinate limits for the clipping window: */
	GLfloat xwMin = -100.0, ywMin = -80.0, xwMax = 100.0, ywMax = 80.0;

	/* Set positions for near and far clipping planes: */
	GLfloat dnear = 25.0, dfar = 125.0;

	glClearColor(.741, .624, .239, 0.0); //Set backgroud color to "gold"

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(x0, y0, z0, xref, yref, zref, Vx, Vy, Vz);

	glMatrixMode(GL_PROJECTION);
	glFrustum(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
}

void hexagonLine(GLfloat xi, GLfloat yi, GLfloat zi) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (int i = 0; i < 12; i++) {
		if (i < 6) {

			//White contour
			glColor3f(1, 1, 1);
			glBegin(GL_POLYGON);
				glVertex3f(xi - 40 * i, yi + 2, zi);
				glVertex3f(xi + 2 + 13 - 40 * i, yi + 2 - 10, zi + 1);
				glVertex3f(xi + 2 + 13 - 40 * i, yi - 2 - 20 , zi + 1);
				glVertex3f(xi - 40 * i, yi - 2 - 30, zi);
			glEnd();
			
			//Black contour
			glColor3f(0, 0, 0);
			glBegin(GL_POLYGON);
				glVertex3f(xi - 40 * i, yi - 2 - 30, zi);
				glVertex3f(xi - 2 - 13 - 40 * i, yi - 2 - 20, zi - 1);
				glVertex3f(xi - 2 - 13 - 40 * i, yi + 2 - 10, zi - 1);
				glVertex3f(xi - 40 * i, yi + 2, zi);
			glEnd();

			//Hexagon "center"
			glColor3f(.443, .133, .522); // Set fill color to "purple".
			glBegin(GL_POLYGON);
				glVertex3f(xi - 40 * i, yi, zi);
				glVertex3f(xi + 13 - 40 * i, yi - 10, zi + 1);
				glVertex3f(xi + 13 - 40 * i, yi - 20, zi + 1);
				glVertex3f(xi - 40 * i, yi - 30, zi);
				glVertex3f(xi - 13 - 40 * i, yi - 20, zi - 1);
				glVertex3f(xi - 13 - 40 * i, yi - 10, zi - 1);
			glEnd();
		}
		else {
			//White contour
			glColor3f(1, 1, 1);
				glBegin(GL_POLYGON);
				glVertex3f(xi - 40 * i, yi + 2, zi);
				glVertex3f(xi + 2 + 13 - 40 * i, yi + 2 - 10, zi - 1);
				glVertex3f(xi + 2 + 13 - 40 * i, yi - 2 - 20, zi - 1);
				glVertex3f(xi - 40 * i, yi - 2 - 30, zi);
			glEnd();

			//Black contour
			glColor3f(0, 0, 0);
				glBegin(GL_POLYGON);
				glVertex3f(xi - 40 * i, yi - 2 - 30, zi);
				glVertex3f(xi - 2 - 13 - 40 * i, yi - 2 - 20, zi + 1);
				glVertex3f(xi - 2 - 13 - 40 * i, yi + 2 - 10, zi + 1);
				glVertex3f(xi - 40 * i, yi + 2, zi);
			glEnd();

			//Hexagon "center"
			glColor3f(.443, .133, .522); // Set fill color to "purple".
			glBegin(GL_POLYGON);
				glVertex3f(xi - 40 * i, yi, zi);
				glVertex3f(xi + 13 - 40 * i, yi - 10, zi - 1);
				glVertex3f(xi + 13 - 40 * i, yi - 20, zi - 1);
				glVertex3f(xi - 40 * i, yi - 30, zi);
				glVertex3f(xi - 13 - 40 * i, yi - 20, zi + 1);
				glVertex3f(xi - 13 - 40 * i, yi - 10, zi + 1);
			glEnd();
		}

	}
}

void sphere(void) {
	//glMatrixMode(GL_PROJECTION);
	//gluOrtho2D(0.0, 500, 0.0, 500);
	GLfloat pos_y = 50;
	glTranslatef(pos_x,pos_y,pos_z);
	glColor3f(.2, .3, .4); //Set color of the sphere
	glutSolidSphere(50, 100, 100);
	glTranslatef(-pos_x, -pos_y, -pos_z);
}

void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat xi = 250, zi = 0;
	int m = 1;

	//Drawing background pattern.
	for (GLfloat yi = -100; yi <= 700; yi = yi + 30) {
		if (m == 1)
			hexagonLine(xi, yi, zi);
		else
			hexagonLine(xi+20, yi, zi);

		m = m * -1;
	}

	sphere(); //Drawing the sphere.

	glFlush();
	glutPostRedisplay();
}

void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);

	winWidth = newWidth;
	winHeight = newHeight;
}

void specialkeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		pos_z = pos_z + .5;
		cout << y << endl;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		pos_z = pos_z - .5;
		cout << y << endl;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		pos_x = pos_x + .5;
		cout << x << endl;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		pos_x = pos_x - .5;
		cout << x << endl;
		glutPostRedisplay();
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Proyecto");

	init();
	glutDisplayFunc(draw);
	glutReshapeFunc(reshapeFcn);

	glutSpecialFunc(specialkeys); //Keyboard callback function to control the sphere.

	glutMainLoop();
	return 0;
}