//#include "GL/glew.h"
//#include "GL/glut.h"
//
//#define PI 3.141592654
//
//void init(void) {
//	glClearColor(1.0, 1.0, 1.0, 0.0);
//	glMatrixMode(GL_PROJECTION);
//	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
//}
//
//void draw(void) {
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	
//	glColor3f(0.0, 0.4, 0.2);
//	glBegin(GL_POLYGON);
//		glVertex3f(100, 100, 0);
//		glVertex3f(115, 90, 0);
//		glVertex3f(115, 80, 0);
//		glVertex3f(100, 70, 0);
//		glVertex3f(85, 80, -1);
//		glVertex3f(85, 90, -1);
//	glEnd();
//
//	glFlush();
//}
//
//int main(int argc, char** argv) {
//	glutInit(&argc, argv);
//
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//	glutInitWindowPosition(50, 100);
//	glutInitWindowSize(500, 500);
//
//	glutCreateWindow("Proyecto Gráficas");
//
//	init();
//	glutDisplayFunc(draw);
//	glutMainLoop();
//
//	return 0;
//}

#include "GL/glut.h"

GLint winWidth = 500, winHeight = 500; // Initial display-window size.

GLfloat x0 = 50, y0 = 50, z0 = 50; // Viewing-coordinate origin.
GLfloat xref = 50.0, yref = 50.0, zref = 0.0; // Look-at point.
GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0; // View-up vector.

/* Set coordinate limits for the clipping window: */
GLfloat xwMin = -100.0, ywMin = -80.0, xwMax = 100.0, ywMax = 80.0;

/* Set positions for near and far clipping planes: */
GLfloat dnear = 25.0, dfar = 125.0;

void init(void)
{
	glClearColor(.741, .624, .239, 0.0);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(x0, y0, z0, xref, yref, zref, Vx, Vy, Vz);

	glMatrixMode(GL_PROJECTION);
	glFrustum(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
}

void hexagono(void) {
	GLfloat xi = 290, yi = 100, zi = 0;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (int i = 0; i < 12; i++) {
		glBegin(GL_POLYGON);
			glVertex3f(xi - 40*i, yi, zi);
			glVertex3f(xi+15 - 40*i, yi-10, zi+1);
			glVertex3f(xi+15 - 40 * i, yi-20, zi+1);
			glVertex3f(xi - 40*i, yi-30, zi);
			glVertex3f(xi-15 - 40*i, yi-20, zi-1);
			glVertex3f(xi-15 - 40*i, yi-10, zi-1);
		glEnd();
	}
}

void esfera(void) {

}

void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(.443, .133, .522); // Set fill color to green.

	hexagono();

	glFlush();
}

void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);

	winWidth = newWidth;
	winHeight = newHeight;
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
	glutMainLoop();

	return 0;
}