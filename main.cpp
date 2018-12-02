#define _USE_MATH_DEFINES

#include <windows.h>
#include <stdio.h>
#include <iostream>
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include "GL/glew.h"
#include "GL/glut.h"
#include <time.h> 
#include <math.h>
#include <vector>
#include <array>

using namespace std;

float sweepResolution = 520;
float xmin = -5;
float xmax = 5;
float ymin = -10;
float ymax = 40;

float x_n;
float y_n;
float zn = 0;

int r = 1;

GLfloat pos_x = 0.5, pos_y = 0.5, pos_z = 5; //Position in x, y and z-axis of the sphere.
GLfloat anglex = 0;
GLfloat angley = 0;
GLfloat anglez = 0;
GLfloat angleVx = -90;
GLfloat angleVy = 0;
GLfloat angleVz = 0;
GLfloat transpSphere = 1.0;

GLint winWidth = 600, winHeight = 600; // Initial display-window size.

GLUquadric *texture; //Used for binding a texture to the sphere.
GLuint sphereTexture; //Used for binding a texture to the sphere.
GLuint vortexTexture; //Used for binding a texture to the sphere.

boolean appear = false; //For allowing appearance of a new object.

//Function for loading the bitmap of the image to create the texture.
int LoadBitmap(const char *filename)
{
	FILE * file;
	char temp;
	long i;

	BITMAPINFOHEADER infoheader;
	unsigned char *infoheader_data;

	GLuint num_texture;
#ifdef WIN32
	errno_t err;
	if ((err = fopen_s(&file, filename, "rb")) != 0) return (-1); // Open the file for reading
#else
	if ((file = fopen(filename, "rb")) == NULL) return (-1); // Open the file for reading

#endif
	fseek(file, 18, SEEK_CUR);  // start reading width & height
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

void init(void) {
	/*-----------------FOR THE LIGHTING--------------------------*/
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	/*-----------------------------------------------------------*/

	GLfloat x0 = 0, y0 = 0, z0 = 10; // Viewing-coordinate origin.
	GLfloat xref = 0, yref = 0, zref = 0; // Look-at point.
	GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0; // View-up vector.

	//Set coordinate limits for the clipping window
	GLfloat xwMin = -100.0, ywMin = -80.0, xwMax = 100.0, ywMax = 80.0;

	//Set positions for near and far clipping planes
	GLfloat dnear = 25.0, dfar = 250.0;

	glClearColor(.741, .624, .239, 0.0); //Set backgroud color to "gold"

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(x0, y0, z0, xref, yref, zref, Vx, Vy, Vz);

	//Working with a frustrum visualization volume.
	glMatrixMode(GL_PROJECTION);
	glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 20.0);

	texture = gluNewQuadric(); //Used for the sphere.
	gluQuadricTexture(texture, GL_TRUE); //Used for the sphere.
	gluQuadricNormals(texture, GLU_SMOOTH);
	sphereTexture = LoadBitmap("images/pattern_sphere.bmp"); //Used for the sphere.
	vortexTexture = LoadBitmap("images/pattern_vortex2.bmp"); //Used for the sphere.
}

float get_distance(float r) {
	return sqrt(pow(r, 2) - pow(r / 2, 2));
}

void draw_edge(float xc, float yc, float w, float z, float r) {
	float d = get_distance(r + w);

	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(xc, yc - (r + w), z);
	glVertex3f(xc + d, yc - (r + w) / 2, z);
	glVertex3f(xc + d, yc + (r + w) / 2, z);
	glVertex3f(xc, yc + (r + w), z);
	glEnd();

	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex3f(xc, yc - (r + w), z);
	glVertex3f(xc - d, yc - (r + w) / 2, z);
	glVertex3f(xc - d, yc + (r + w) / 2, z);
	glVertex3f(xc, yc + (r + w), z);
	glEnd();
}

void draw_hexagon(float xc, float yc, float z, float r, float w) {
	float d = get_distance(r);
	draw_edge(xc, yc, w, z, r);

	// Set fill color to "purple".
	glColor3ub(72, 27, 109);
	glBegin(GL_POLYGON);
	glVertex3f(xc, yc + r, z);
	glVertex3f(xc + d, yc + r / 2, z);
	glVertex3f(xc + d, yc - r / 2, z);
	glVertex3f(xc, yc - r, z);
	glVertex3f(xc - d, yc - r / 2, z);
	glVertex3f(xc - d, yc + r / 2, z);
	glEnd();

}

void hexagon_line(float xi, float yc, float r, float w, float inc) {
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); //Used for enabling color of polygons after applying lighting effects.
	glEnable(GL_COLOR_MATERIAL); //Used for enabling color of polygons after applying lighting effects.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (float xc = xi; xc <= -2 * xi; xc += inc) {
		draw_hexagon(xc, yc, -10, r, w);
	}
}

void sphere() {
	glColor3f(1, 1, 1);

	/*------------------Binding texture to the sphere---------------*/
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sphereTexture);


	//Next lines will draw the textured sphere
	glPushMatrix();
	//glRotatef(anglez, 0.0, 0.0, 1.0);
	//glRotatef(angley, pos_x, pos_y, pos_z);
	//glRotatef(anglex, 1.0, 1, 0);
	//
	glTranslatef(pos_x, pos_y, pos_z);
	glRotatef(anglex, 1, 0, 0);
	glRotatef(angley, 0, 1, 0);
	glRotatef(anglez, 0, 0, 1);
	gluSphere(texture, 0.8, 36, 72);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void vortex() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, vortexTexture);


	glColor3f(1.0, 1.0, 1.0);
	GLfloat angle = -5200 * M_PI / 180;
	GLfloat angle2 = -5000 * M_PI / 180;
	float h = 10;

	glPushMatrix();
	glTranslatef(0, 0, -20);
	glRotatef(-angle, 1.0f, 0.0f, 0.0f);
	gluCylinder(texture, 10, 1, h, 36, 72);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -20);
	glRotatef(angle, 1.0f, 0.0f, 0.0f);
	gluCylinder(texture, 10, 1, h, 36, 72);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

//Displaying menu options as text.
void print(float x, float y, float z, const char *string)
{
	glRasterPos2f(x, y); //Set position of text.
	int len = (int)strlen(string);

	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]); //Displaying character by character.
	}
}

//Main drawing function.
void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear display window.

	/*---------------------------------------Drawing background---------------------------------------------*/
	float r = 0.8, w = 0.1, x1 = -10, x2 = -9.0, inc = 2 * (r + w) + 0.08;
	int m = 1;
	float xi, ylim = 9;
	for (float yc = ylim; yc >= -2 * ylim; yc -= 0.9*inc) {
		xi = (m == 1) ? x1 : x2;
		hexagon_line(xi, yc, r, w, inc);
		m = m * (-1);
	}
	/*------------------------------------------------------------------------------------------------------*/

	/*--------------------Draw random sphere-------------------------------------*/

	if (appear) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4f(.5, .2, .3, transpSphere);
		glPushMatrix();
		glTranslatef(x_n, y_n, zn);
		//glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(1.2, 20, 20);
		glPopMatrix();
		glDisable(GL_BLEND);
	}
	glColor4f(1, 1, 1, 1.0);
	/*---------------------------------------------------------------------------*/

	vortex(); //Drawing vortex in the middle of the scene.

	/*-----------------------Lighting----------------------------------------------*/
	glTranslatef(0.0f, 0.0f, -8.0f);

	//Add ambient light
	GLfloat ambientColor[] = { 0.6, 0.6, 0.6, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	//Add positioned light
	GLfloat diffuseLightColor0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLightColor0[] = { 0.7f, 0.7f, 0.7f, 0.7f };

	GLfloat lightPos0[] = { 1.0f, 10.0f, 5.0f, 1.0f };
	GLfloat lightPos2[] = { 4.0f, -10.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);

	//Add directed light
	GLfloat lightColor1[] = { 0.9f, 0.9f, 0.9f, 1.0f }; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = { 1.0f, 1.0f, -0.5f, 0.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLightColor0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	glTranslatef(0.0f, 0.0f, 8.0f);
	/*---------------------------------------------------------------------------*/

	sphere(); // Drawing sphere

	/*---------------------Generate label with menu-------------------------*/
	glColor3f(0, 0, 0);
		glBegin(GL_POLYGON);
			glVertex3f(5, -5, 0);
			glVertex3f(-5, -5, 0);
			glVertex3f(-5, -3.0, 0);
			glVertex3f(5, -3.0, 0);	
		glEnd();

		glColor3f(1, 1, 1);
		print(-4.5, -3.4, 2, "Usa A-S-W-D para mover la esfera principal.");
		print(-4.5, -3.6, 2, "Q-E para cambiar profundidad de la esfera principal.");
		print(-4.5, -3.8, 2, "U-I-O-P para modificar iluminacion.");
		print(-4.5, -4.0, 2, "Y para rotar la esfera.");
		print(-4.5, -4.2, 2, "Z para aparecer una nueva esfera; C para dismunuir transparencia.");
		print(-4.5, -4.4, 2, "X para desaparecer la nueva esfera; V para aumentar transparencia.");
		/*----------------------------------------------------------------------*/


	glFlush();
	glutPostRedisplay();

}

void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
	glClear(GL_COLOR_BUFFER_BIT);


}

//Method to control the sphere using the keyboard.
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'q':
		pos_z = pos_z + .5;
		break;
	case 'w':
		pos_y = pos_y + .2;
		break;
	case 'r':
		anglex += 5;
		angleVx -= 5;
		break;
	case 't':
		angley += 5;
		angleVy += 5;
		break;
	case 'y':
		anglez += 5;
		angleVz += 5;
		break;
	case 'e':
		pos_z = pos_z - .5;
		break;
	case 'u':
		glDisable(GL_LIGHT0);
		break;
	case 'i':
		glEnable(GL_LIGHT0);
		break;
	case 'o':
		glDisable(GL_LIGHT1);
		break;
	case 'p':
		glEnable(GL_LIGHT1);
		break;
	case 'a':
		pos_x = pos_x - .2;
		break;
	case 's':
		pos_y = pos_y - .2;
		break;
	case 'd':
		pos_x = pos_x + .2;
		break;
	case 'z':
		appear = true;
		break;
	case 'x':
		appear = false;
		break;
	case 'c':
		transpSphere += 0.05;
		break;
	case 'v':
		transpSphere -= 0.05;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

//Function to detect if the secondary sphere is leaving the viewing window.
int outOfBounds(float a) {
	if (a > 3.8) {
		r = -1;
	}
	if (a < -3.8) {
		r = 1;
	}

	return r;
}

//Function to control the secondary sphere movement.
void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 64, timer, 0);

	x_n = x_n + outOfBounds(x_n) * 0.003;
	y_n = y_n + outOfBounds(y_n) * 0.001;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowPosition(150, 150);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Proyecto final");
	glutTimerFunc(0, timer, 0);

	init();

	glutDisplayFunc(draw);
	glutReshapeFunc(reshapeFcn);
	glutKeyboardFunc(keyboard); //Keyboard callback function to control the sphere.

	glutMainLoop();
}

