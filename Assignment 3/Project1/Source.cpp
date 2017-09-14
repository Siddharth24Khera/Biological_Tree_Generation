#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include <iostream>
#include <list>
#include<vector>
#include<cmath>
#include "Dependencies\freeglut\freeglut.h"
#include "Texture.h"


#define PI 3.14159265

using namespace std;

/* Global variables */
char title[] = "Tree";

static GLuint text;
static GLuint leafText;


float degree = 0.0f;
float verticalDegree = 0.0f;
float eyex = 0.0f, eyey = 0.0f;
float eyez = 50.0f;

double uniformDevFront = 0.0;
double uniformDevRight = 0.0;
double uniformDevUp = 0.0;


class Vertex
{
private:
	double x, y, z;
public:
	Vertex() { x = 0; y = 0; z = 0; }
	Vertex(double a, double b, double c) {
		x = a; y = b; z = c;
	}
	Vertex(const Vertex &v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
	double getXcoord() { return x; }
	double getYcoord() { return y; }
	double getZcoord() { return z; }
};

class Cylinder
{
private:
	Vertex start, end;
	double startGirth, endGirth;

public:
	Cylinder() { start = Vertex(0, 0, 0); end = Vertex(10, 10, 10); startGirth = 1; endGirth = 1; }
	Cylinder(Vertex a, Vertex b, double sg, double eg) {
		start = a; end = b; startGirth = sg; endGirth = eg;
	}
	Vertex getStart() { return start; }
	Vertex getEnd() { return end; }
	double getStartGirth() { return startGirth; }
	double getEndGirth() { return endGirth; }
};

class Control {
private:
	double x, y, z;
	double strength;
public:

	Control() {
		x = 10; y = 10; z = 10; strength = 0.4;
	}
	Control(Vertex v, double d) {
		x = v.getXcoord(); y = v.getYcoord(); z = v.getZcoord(); strength = d;
	}

	double getXCoordi() { return x; }
	double getYCoordi() { return y; }
	double getZCoordi() { return z; }

	double getStrength() { return strength; }

};

list<Control> listOfControlPoints;


class TreeGen
{
private:
	double branchStemRatioLeft;
	double branchStemRatioRight;
	double branchStemRatioCentre;
	double girthRatioLeft;
	double girthRatioRight;
	double girthRatioCentre;
	double girthDecay;
	double Langle;
	double Rangle;
	bool isTernary;
	int branchAngleType;
	double divAngle;
	/*
	0 = constant ;
	1 = decrease constantly from bottom to top ;
	*/
	double branchAngleParameter;
	int depthOfBranching;
	list<Cylinder> listOfBranches;
	list <Cylinder> currentListOfBranches;

public:

	TreeGen() {
		FILE * pFile;
		pFile = fopen("input.txt", "r");
		float bsl = 0, bsr = 0, bsc = 0, grl = 0, grr = 0, grc = 0, la = 0, ra = 0, dep = 0, dec = 0, ter = 0, typ = 0, par = 0, div = 0;
		fscanf(pFile, "branchStemRatioLeft = %f branchStemRatioRight = %f branchStemRatioCentre = %f girthRatioLeft = %f girthRatioRight = %f girthRatioCentre = %f Langle = %f Rangle = %f depthOfBranching = %f girthDecay = %f isTernary = %f branchAngleType = %f branchAngleParameter = %f divAngle = %f", &bsl,&bsr,&bsc,&grl,&grr,&grc,&la,&ra,&dep,&dec,&ter,&typ,&par,&div);
		fclose(pFile);
		branchStemRatioLeft = bsl;
		branchStemRatioRight = bsr;
		branchStemRatioCentre = bsc;
		girthRatioLeft = grl;
		girthRatioRight = grr;
		girthRatioCentre = grc;
		Langle = la;
		Rangle = ra;		// by convention, I have kept it negative
		depthOfBranching = dep;
		girthDecay = dec;
		isTernary = ter;
		branchAngleType = typ;
		branchAngleParameter = par;
		divAngle = div;
	}

	TreeGen(double left, double right, double centre, double girthL, double girthR, double lang, double rang, double girDecay, int depth) {
		branchStemRatioLeft = left;
		branchStemRatioRight = right;
		branchStemRatioCentre = centre;
		girthRatioLeft = girthL;
		girthRatioRight = girthR;
		Langle = lang;
		Rangle = rang;
		girthDecay = girDecay;
		depthOfBranching = depth;
	}

	list<Cylinder> getListOfBranches() { return listOfBranches; }
	list<Cylinder> getOuterListOfBranches() { return currentListOfBranches; }


	vector<Vertex> getNextVerticesTree(Cylinder l, int count) {

		/*double cosThetaBy2 = cos((angle / 2) * PI / 180);
		double sinThetaBy2 = sin((angle / 2) * PI / 180);*/

		double x1f, y1f, x2f, y2f, x3f, y3f, z1f, z2f, z3f;
		double x10, y10, x20, y20, z20, z10;
		x20 = l.getEnd().getXcoord(); y20 = l.getEnd().getYcoord(); z20 = l.getEnd().getZcoord();
		x10 = l.getStart().getXcoord(); y10 = l.getStart().getYcoord(); z10 = l.getStart().getZcoord();

		double u, v, w;
		v = x20 - x10;
		w = y20 - y10;
		u = z20 - z10;

		double s = 1 / sqrt(u*u + v*v);

		double t = sqrt(u*u + v*v + w*w);

		if (count == 1 || (u == 0 && v == 0)) {
			x1f = x20 + branchStemRatioRight*(v*cos(Rangle * PI / 180) - w*sin(Rangle * PI / 180));
			y1f = y20 + branchStemRatioRight*(v*sin(Rangle * PI / 180) + w*cos(Rangle * PI / 180));
			z1f = 0;

			x2f = (branchStemRatioCentre + 1)*x20 - branchStemRatioCentre*x10;
			y2f = (branchStemRatioCentre + 1)*y20 - branchStemRatioCentre*y10;
			z2f = 0;

			x3f = x20 + branchStemRatioLeft*(v*cos(Langle * PI / 180) - w*sin(Langle * PI / 180));
			y3f = y20 + branchStemRatioLeft*(v*sin(Langle * PI / 180) + w*cos(Langle * PI / 180));
			z3f = 0;
			vector<Vertex> vertexVector;

			/*Vertex v1 = Vertex(x1f, y1f, 0);
			vertexVector.push_back(v1);

			if (isTernary) {
				Vertex v2 = Vertex(x2f, y2f, 0);
				vertexVector.push_back(v2);
			}

			Vertex v3 = Vertex(x3f, y3f, 0);
			vertexVector.push_back(v3);*/

			double x1, y1, z1;
			x1 = (x20 - x10) / t; y1 = (y20 - y10) / t; z1 = (z20 - z10) / t;
			double d1 = sqrt(y1*y1 + z1*z1);
			double alpha, beta;

			beta = atan2(x1, d1) * 180 / PI;
			alpha = atan2(y1, z1) * 180 / PI;

			/*Vertex v1 = Vertex(x1f, y1f, z1f);
			vertexVector.push_back(v1);*/
			glMatrixMode(GL_MODELVIEW);

			glPushMatrix();
			glLoadIdentity();
			glTranslated(x10, y10, z10);
			glRotatef(-alpha, 1.0, 0.0, 0.0);
			glRotatef(beta, 0.0, 1.0, 0.0);
			glRotatef(divAngle * count, 0.0, 0.0, 1.0);
			glRotatef(-beta, 0.0, 1.0, 0.0);
			glRotatef(alpha, 1.0, 0.0, 0.0);
			glTranslated(-x10, -y10, -z10);
			GLfloat mat[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, mat);
			glPopMatrix();

			double q1, w1, e1;
			q1 = mat[0] * x1f + mat[4] * y1f + mat[8] * z1f + mat[12];
			w1 = mat[1] * x1f + mat[5] * y1f + mat[9] * z1f + mat[13];
			e1 = mat[2] * x1f + mat[6] * y1f + mat[10] * z1f + mat[14];

			Vertex v1 = Vertex(q1, w1, e1);
			vertexVector.push_back(v1);


			if (isTernary) {
				Vertex v2 = Vertex(x2f, y2f, z2f);
				vertexVector.push_back(v2);
			}

			double q3, w3, e3;
			q3 = mat[0] * x3f + mat[4] * y3f + mat[8] * z3f + mat[12];
			w3 = mat[1] * x3f + mat[5] * y3f + mat[9] * z3f + mat[13];
			e3 = mat[2] * x3f + mat[6] * y3f + mat[10] * z3f + mat[14];
			Vertex v3 = Vertex(q3, w3, e3);
			vertexVector.push_back(v3);

			return vertexVector;
		}



		z1f = z20 + branchStemRatioRight*(u*cos(Rangle * PI / 180) - v*s*t*sin(Rangle * PI / 180)) + uniformDevFront;
		x1f = x20 + branchStemRatioRight*(v*cos(Rangle * PI / 180) + u*s*t*sin(Rangle * PI / 180)) + uniformDevRight;
		y1f = y20 + branchStemRatioRight*w*cos(Rangle * PI / 180) + uniformDevUp;

		x2f = x20 + branchStemRatioCentre*v;
		y2f = y20 + branchStemRatioCentre*w;
		z2f = z20 + branchStemRatioCentre*u;


		z3f = z20 + branchStemRatioLeft*(u*cos(Langle * PI / 180) - v*s*t*sin(Langle * PI / 180)) + uniformDevFront;
		x3f = x20 + branchStemRatioLeft*(v*cos(Langle * PI / 180) + u*s*t*sin(Langle * PI / 180)) + uniformDevRight;
		y3f = y20 + branchStemRatioLeft*w*cos(Langle * PI / 180) + uniformDevUp;

		if (listOfControlPoints.size() != 0) {
			for (Control c : listOfControlPoints) {
				double l1 = sqrt(pow(c.getXCoordi() - x1f, 2) + pow(c.getYCoordi() - y1f, 2) + pow(c.getZCoordi() - z1f, 2));
				double l2 = sqrt(pow(c.getXCoordi() - x2f, 2) + pow(c.getYCoordi() - y2f, 2) + pow(c.getZCoordi() - z2f, 2));
				double l3 = sqrt(pow(c.getXCoordi() - x3f, 2) + pow(c.getYCoordi() - y3f, 2) + pow(c.getZCoordi() - z3f, 2));

				x1f = x1f + c.getStrength()*(c.getXCoordi() - x1f) / l1;
				y1f = y1f + c.getStrength()*(c.getYCoordi() - y1f) / l1;
				z1f = z1f + c.getStrength()*(c.getZCoordi() - z1f) / l1;

				x2f = x2f + c.getStrength()*(c.getXCoordi() - x2f) / l2;
				y2f = y2f + c.getStrength()*(c.getYCoordi() - y2f) / l2;
				z2f = z2f + c.getStrength()*(c.getZCoordi() - z2f) / l2;

				x3f = x3f + c.getStrength()*(c.getXCoordi() - x3f) / l3;
				y3f = y3f + c.getStrength()*(c.getYCoordi() - y3f) / l3;
				z3f = z3f + c.getStrength()*(c.getZCoordi() - z3f) / l3;
			}
		}
		vector<Vertex> vertexVector;

		
		
	
		double x1, y1, z1;
		x1 = (x20 - x10) / t; y1 = (y20 - y10) / t; z1 = (z20 - z10) / t;
		double d1 = sqrt(y1*y1 + z1*z1);
		double alpha, beta;

		beta = atan2(x1,d1) * 180 / PI;
		alpha = atan2(y1, z1) * 180 / PI;

		Vertex v1 = Vertex(x1f, y1f, z1f);
		vertexVector.push_back(v1);
		/*glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		glLoadIdentity();
		glTranslated(x10,y10,z10);
		glRotatef(-alpha, 1.0, 0.0, 0.0);
		glRotatef(beta, 0.0, 1.0, 0.0);
		glRotatef(140.0 * count, 0.0, 0.0, 1.0);
		glRotatef(-beta, 0.0, 1.0, 0.0);
		glRotatef(alpha, 1.0, 0.0, 0.0);
		glTranslated(-x10, -y10, -z10);		
		GLfloat mat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		glPopMatrix();

		double q1, w1, e1;
		q1 = mat[0] * x1f + mat[4] * y1f + mat[8] * z1f + mat[12];
		w1 = mat[1] * x1f + mat[5] * y1f + mat[9] * z1f + mat[13];
		e1 = mat[2] * x1f + mat[6] * y1f + mat[10] * z1f + mat[14];

		Vertex v1 = Vertex(q1, w1, e1);
		vertexVector.push_back(v1);*/
		

		if (isTernary) {
			Vertex v2 = Vertex(x2f, y2f, z2f);
			vertexVector.push_back(v2);
		}	

		/*double q3, w3, e3;	
		q3 = mat[0] * x3f + mat[4] * y3f + mat[8] * z3f + mat[12];
		w3 = mat[1] * x3f + mat[5] * y3f + mat[9] * z3f + mat[13];
		e3 = mat[2] * x3f + mat[6] * y3f + mat[10] * z3f + mat[14];
		Vertex v3 = Vertex(q3, w3, e3);
		vertexVector.push_back(v3);*/

		Vertex v3 = Vertex(x3f, y3f, z3f);
		vertexVector.push_back(v3);
		

		return vertexVector;
	}

	void makeTree() {

		list <Cylinder> totalListOfBranches;
		totalListOfBranches.push_back(Cylinder(Vertex(0, -15, 0), Vertex(0, -10, 0), 1.0, 1.0*girthDecay));

		list <Cylinder> currentListOfBranchesTemp;
		currentListOfBranches.push_back(Cylinder(Vertex(0, -15, 0), Vertex(0, -10, 0), 1.0, 1.0*girthDecay));
		int count = 0;
		while (count < depthOfBranching) {
			count++;
			for (Cylinder l : currentListOfBranches) {
				double x1=0, x2=0, x3=0, x4=0;
				vector<Vertex> vertexVector = getNextVerticesTree(l, count);

				currentListOfBranchesTemp.push_back(Cylinder(l.getEnd(), vertexVector.front(), l.getEndGirth()*girthRatioRight, l.getEndGirth()*girthRatioRight*girthDecay));
				totalListOfBranches.push_back(Cylinder(l.getEnd(), vertexVector.front(), l.getEndGirth()*girthRatioRight, l.getEndGirth()*girthRatioRight*girthDecay));

				currentListOfBranchesTemp.push_back(Cylinder(l.getEnd(), vertexVector.back(), l.getEndGirth()*girthRatioLeft, l.getEndGirth()*girthRatioLeft*girthDecay));
				totalListOfBranches.push_back(Cylinder(l.getEnd(), vertexVector.back(), l.getEndGirth()*girthRatioLeft, l.getEndGirth()*girthRatioLeft*girthDecay));
				vertexVector.pop_back();

				if (isTernary) {
					currentListOfBranchesTemp.push_back(Cylinder(l.getEnd(), vertexVector.back(), l.getEndGirth()*girthRatioCentre, l.getEndGirth()*girthRatioCentre*girthDecay));
					totalListOfBranches.push_back(Cylinder(l.getEnd(), vertexVector.back(), l.getEndGirth()*girthRatioCentre, l.getEndGirth()*girthRatioCentre*girthDecay));
				}
			}
			if (branchAngleType == 1) {
				Langle -= branchAngleParameter;
				Rangle += branchAngleParameter;
			}
			currentListOfBranches.clear();
			currentListOfBranches = currentListOfBranchesTemp;
			currentListOfBranchesTemp.clear();
		}
		listOfBranches = totalListOfBranches;
	}
};


TreeGen tree;

void inputKey(int key, int x, int y)
{

	switch (key) {
	case GLUT_KEY_RIGHT: {degree -= 5.0f; glutPostRedisplay(); break; }
	case GLUT_KEY_LEFT: {degree += 5.0f; glutPostRedisplay(); break; }
	case GLUT_KEY_UP: {eyez -= 1.0f; glutPostRedisplay(); break; }
	case GLUT_KEY_DOWN: { eyez += 1.0f; glutPostRedisplay(); break; }
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{
		exit(0);
		break;
	}
	case 's':
	{
		verticalDegree -= 1.0f;
		glutPostRedisplay();
		break;
	}
	case 'w':
	{
		verticalDegree += 1.0f;
		glutPostRedisplay();
		break;
	}
	case 'r': {
		eyez = 50;
		degree = 0.0f;
		verticalDegree = 0.0f;
		glutPostRedisplay();
		break;

	}
	}
}

void drawBranch(Cylinder li) {
	Vertex v1 = li.getStart(); Vertex v2 = li.getEnd();
	double a1, a2, a3, b1, b2, b3;
	a1 = v1.getXcoord(); a2 = v1.getYcoord(); a3 = v1.getZcoord();
	b1 = v2.getXcoord(); b2 = v2.getYcoord(); b3 = v2.getZcoord();

	double len = sqrt(pow(a1 - b1, 2) + pow(a2 - b2, 2) + pow(a3 - b3, 2));
	double u, v, w;
	u = (b1 - a1) / len; v = (b2 - a2) / len; w = (b3 - a3) / len;
	double alpha, beta;

	beta = asin(u) * 180 / PI;
	alpha = atan2(v, w) * 180 / PI;

	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	gluQuadricTexture(quadratic, 1);

	GLUquadricObj *quad2;
	quad2 = gluNewQuadric();
	gluQuadricTexture(quad2, 1);


	glPushMatrix();
	glTranslated(a1, a2, a3);
	glutSolidSphere(li.getStartGirth(), 32, 32);
	glRotatef(-alpha, 1.0, 0.0, 0.0);
	glRotatef(beta, 0.0, 1.0, 0.0);

	gluCylinder(quadratic, li.getStartGirth(), li.getEndGirth(), len, 32, 32);

	glRotatef(-beta, 0.0, 1.0, 0.0);
	glRotatef(alpha, 1.0, 0.0, 0.0);
	glTranslated(-a1, -a2, -a3);
	glTranslated(b1, b2, b3);

	gluSphere(quad2, li.getEndGirth(), 32, 32);

	glTranslated(-b1, -b2, -b3);
	glPopMatrix();

	glFlush();
}

int abcd = 1;

void drawLeaf(Cylinder li) {
	Vertex v2 = li.getEnd();
	double x, y, z;
	x = v2.getXcoord(); y = v2.getYcoord(); z = v2.getZcoord();

	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	gluQuadricTexture(quadratic, 1);

	glPushMatrix();
	glTranslated(x, y, z);
	if (abcd == 1) {
		glRotatef(60.0f, -1.0f, 0.0f, 0.0f);
		glRotatef(60.0f, 0.0f, 0.0f, -1.0f);
		abcd = -abcd;
	}
	else {
		glRotatef(60.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
		abcd = -abcd;
	}
	glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
	gluDisk(quadratic, 0, 0.3, 32, 32);

	glPopMatrix();

	glFlush();
}

void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, text);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eyex, eyey, eyez, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//glTranslatef(0, 0, -20);
	glRotatef(degree, 0.0f, 1.0f, 0.0f);
	glRotatef(verticalDegree, 1.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 0.0f, 1.0f);
	if (listOfControlPoints.size() != 0) {
		for (Control c : listOfControlPoints) {
			glTranslatef(c.getXCoordi(), c.getYCoordi(), c.getZCoordi());
			glutSolidSphere(0.5, 30, 30);
			glTranslatef(-c.getXCoordi(), -c.getYCoordi(), -c.getZCoordi());

		}
	}



	list<Cylinder> totalListOfLines = tree.getListOfBranches();
	for (Cylinder l : totalListOfLines) {
		drawBranch(l);
	}

	

	/*glBindTexture(GL_TEXTURE_2D, leafText);

	glColor3f(0.0f, 1.0f, 0.0f);
	list<Cylinder> currentListOfLines = tree.getOuterListOfBranches();
	for (Cylinder l : currentListOfLines) {
	drawLeaf(l);
	}*/
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	gluPerspective(60.0f, aspect, 0.1f, 200.0f);

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow(title);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutSpecialFunc(inputKey);
	glutKeyboardFunc(keyboard);

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClearDepth(1.0f);
	Texture tex;
	text = tex.loadBMP_custom("./images/tree.bmp");
	leafText = tex.loadBMP_custom("./images/green.bmp");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_NORMALIZE); 
	glEnable(GL_CULL_FACE);


	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position1[] = { 0.0, 0.0, eyez, 0.0 };
	GLfloat light_position2[] = { 0.0, 10.0, 0.0, 0.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);


	Control controlPoint;

	//listOfControlPoints.push_back(controlPoint);

	tree.makeTree();
	glutMainLoop();

	return 0;
}