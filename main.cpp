#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"
#include <math.h>

using namespace std;

GLfloat pos_x = 10, pos_z = 10, pos_y = 50; //Position in x and z-axis of the sphere.

GLint winWidth = 500, winHeight = 500; // Initial display-window size.

GLUquadric *texture; //Used for binding a texture to the sphere.
GLuint sphereTexture; //Used for binding a texture to the sphere.

//Function for loading the bitmap of the image to create the texture.
int LoadBitmap(const char *filename)
{
	FILE * file;
	char temp;
	long i;

	BITMAPINFOHEADER infoheader;
	unsigned char *infoheader_data;

	GLuint num_texture;

	if ((file = fopen(filename, "rb")) == NULL) return (-1); // Open the file for reading

	fseek(file, 18, SEEK_CUR);  /* start reading width & height */
	fread(&infoheader.biWidth, sizeof(int), 1, file);

	fread(&infoheader.biHeight, sizeof(int), 1, file);

	fread(&infoheader.biPlanes, sizeof(short int), 1, file);
	if (infoheader.biPlanes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, infoheader.biPlanes);
		return 0;
	}

	// read the bpp
	fread(&infoheader.biBitCount, sizeof(unsigned short int), 1, file);
	if (infoheader.biBitCount != 24) {
		printf("Bpp from %s is not 24: %d\n", filename, infoheader.biBitCount);
		return 0;
	}

	fseek(file, 24, SEEK_CUR);

	// read the data
	if (infoheader.biWidth < 0) {
		infoheader.biWidth = -infoheader.biWidth;
	}
	if (infoheader.biHeight < 0) {
		infoheader.biHeight = -infoheader.biHeight;
	}
	infoheader_data = (unsigned char *)malloc(infoheader.biWidth * infoheader.biHeight * 3);
	if (infoheader_data == NULL) {
		printf("Error allocating memory for color-corrected image data\n");
		return 0;
	}

	if ((i = fread(infoheader_data, infoheader.biWidth * infoheader.biHeight * 3, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}

	for (i = 0; i < (infoheader.biWidth * infoheader.biHeight * 3); i += 3) { // reverse all of the colors. (bgr -> rgb)
		temp = infoheader_data[i];
		infoheader_data[i] = infoheader_data[i + 2];
		infoheader_data[i + 2] = temp;
	}


	fclose(file); // Closes the file stream

	glGenTextures(1, &num_texture);
	glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

	// The next commands sets the texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //The minifying function

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Finally we define the 2d texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, infoheader.biWidth, infoheader.biHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, infoheader_data);

	// And create 2d mipmaps for the minifying function
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader.biWidth, infoheader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, infoheader_data);

	free(infoheader_data); // Free the memory we used to load the texture

	return (num_texture); // Returns the current texture OpenGL ID
}

void init(void)
{
	GLfloat x0 = 50, y0 = 50, z0 = 50; // Viewing-coordinate origin.
	GLfloat xref = 50.0, yref = 50.0, zref = 0.0; // Look-at point.
	GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0; // View-up vector.

	//Set coordinate limits for the clipping window
	GLfloat xwMin = -100.0, ywMin = -80.0, xwMax = 100.0, ywMax = 80.0;

	//Set positions for near and far clipping planes
	GLfloat dnear = 25.0, dfar = 250.0;

	glClearColor(.741, .624, .239, 0.0); //Set backgroud color to "gold"

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(x0, y0, z0, xref, yref, zref, Vx, Vy, Vz);

	glMatrixMode(GL_PROJECTION);
	glFrustum(xwMin, xwMax, ywMin, ywMax, dnear, dfar);

	//glEnable(GL_DEPTH_TEST); //Used for the sphere.
	texture = gluNewQuadric(); //Used for the sphere.
	gluQuadricTexture(texture, GL_TRUE); //Used for the sphere.
	sphereTexture = LoadBitmap("images/pattern.bmp"); //Used for the sphere.

	//////////////FOR THE LIGHTING
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	/////////////
}

//Method for drawing a row of hexagons in the background.
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

//Method to draw the sphere.
void sphere(void) {
	glColor3f(1, 1, 1);
	//glMatrixMode(GL_PROJECTION);
	//gluOrtho2D(0.0, 500, 0.0, 500);
	//GLfloat pos_y = 50;

	/*------------------Trying to bind texture to the sphere---------------*/	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sphereTexture);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	////rotate/translate entire scene with a,A,s,S,D,D and h,l,j,k,u,U keys
	//glRotatef(anglez, 0.0, 0.0, 1.0);
	//glRotatef(angley, 0.0, 1.0, 0.0);
	//glRotatef(anglex, 1.0, 0.0, 0.0);

	//draw textured sphere
	glPushMatrix();
	//glTranslatef(locX, locY, locZ);
	glTranslatef(pos_x, pos_y, pos_z);
	//glScalef(0.5, 0.5, 0.5);
	gluSphere(texture, 60, 36, 72);
	glTranslatef(pos_x, pos_y, pos_z);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	//glutSwapBuffers();
	/*---------------------------------------------------------------------*/

	//glTranslatef(pos_x,pos_y,pos_z);
	//glColor3f(.2, .3, .4); //Set color of the sphere
	//glutSolidSphere(50, 100, 100);
	//glTranslatef(-pos_x, -pos_y, -pos_z);
}

//Main drawing method.
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

	/////////////////////////////////LIGHTING
	glTranslatef(0.0f, 0.0f, -8.0f);

	//Add ambient light
	GLfloat ambientColor[] = { 0.9, 0.9, 0.9, 0.0 }; //Color (0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	//Add positioned light
	GLfloat diffuseLightColor0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLightColor0[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat lightPos0[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightColor0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//Add directed light
	GLfloat lightColor1[] = { 0.9f, 0.9f, 0.9f, 1.0f }; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = { -1.0f, 0.5f, -0.2f, 0.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	glTranslatef(0.0f, 0.0f, 8.0f);
	/////////////////////////////////

	glFlush();
	glutPostRedisplay();
}

//Method for managing window reshape.
void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);

	winWidth = newWidth;
	winHeight = newHeight;
}

//Method to control the sphere using the keyboard.
void specialkeys(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			pos_y = pos_y + .5;
			cout << y << endl;
			break;
		case GLUT_KEY_DOWN:
			pos_y = pos_y - .5;
			cout << y << endl;
			break;
		case GLUT_KEY_RIGHT:
			pos_x = pos_x + .5;
			cout << x << endl;
			break;
		case GLUT_KEY_LEFT:
			pos_x = pos_x - .5;
			cout << x << endl;
			break;
		case GLUT_KEY_F1:
			pos_z += .5;
			break;
		case GLUT_KEY_F2:
			pos_z = pos_z - .5;
		case GLUT_KEY_F3:
			glDisable(GL_LIGHT0);
			break;
		case GLUT_KEY_F4:
			glEnable(GL_LIGHT0);
			break;
		case GLUT_KEY_F5:
			glDisable(GL_LIGHT1);
			break;
		case GLUT_KEY_F6:
			glEnable(GL_LIGHT1);
			break;
	}
	glutPostRedisplay();
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