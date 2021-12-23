#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include <cmath> 

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;


//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("rock.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void addZ(std::vector<double*>* arr, double addValue)
{
	std::vector<double*> vector = *arr;

	for each (double* var in vector)
	{
		var[2] += addValue;
	}
}

double* getCenterDot(double* firstDot, double* secondDot)
{
	double centerDot[] = { ((firstDot[0] + secondDot[0]) / 2),((firstDot[1] + secondDot[1]) / 2),firstDot[2] };
	return centerDot;
}

std::vector<double*> circleDot(unsigned int cntDot, double* firstDot, double* secondDot, double* thirdDot = NULL) {

	std::vector<double*> dotVector;
	double centerDot[] = { ((firstDot[0] + secondDot[0]) / 2),((firstDot[1] + secondDot[1]) / 2),firstDot[2] };
	double radius;

	double addAngle = acos((firstDot[0] * 0 + firstDot[1] * 1) / (sqrt(pow(firstDot[0], 2) + pow(firstDot[1], 2)) * sqrt(pow(0, 2) + pow(1, 2)))) * (3.1415926 / 180);


	if (cntDot == 0) return dotVector;
	else if (thirdDot == NULL)
	{
		radius = (sqrt(pow(secondDot[0] - firstDot[0], 2) + pow(secondDot[1] - firstDot[1], 2) + pow(secondDot[2] - firstDot[2], 2))) / 2;
		for (int i = 0; i < cntDot; i++) {


			float angle = (3.1415926 * -float(i) / cntDot) + 3 * addAngle;
			double dx = radius * cosf(angle);
			double dy = radius * sinf(angle);
			dotVector.push_back(new double[] { centerDot[0] + dx, centerDot[1] + dy, centerDot[2] });
		}
	}
	else
	{

	}

	return dotVector;
}


void GetNormal(double* firstDot, double* secondDot, double* thirdDot, double* normal)
{
	normal[2] = 0;
	double vector1[3] = { secondDot[0] - firstDot[0],secondDot[1] - firstDot[1], secondDot[2] - firstDot[2] };
	double vector2[3] = { thirdDot[0] - firstDot[0],thirdDot[1] - firstDot[1], secondDot[2] - firstDot[2] };

	normal[0] = (vector1[1] * vector2[2] - vector2[1] * vector1[2])/(sqrt(pow(vector1[0],2)+ pow(vector2[0], 2)));
	normal[1] = -1 * (vector1[0] * vector2[2] - vector2[0] * vector1[2] )/ (sqrt(pow(vector1[1], 2) + pow(vector2[1], 2)));
	normal[2] = 0;
	return;
}

void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };

	//glBindTexture(GL_TEXTURE_2D, texId);

	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);

	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);

	//glEnd();
	////����� ��������� ���������� ��������


#pragma region ��� ������

	glBindTexture(GL_TEXTURE_2D, texId);

#pragma region ����������� �������� �����
	std::vector<double*> dotArr;
	dotArr.push_back(new double[] { 1.5, -3, 0 });
	dotArr.push_back(new double[] { -2, -3.5, 0 });
	dotArr.push_back(new double[] { -0.5, -0.5, 0 });
	dotArr.push_back(new double[] { -4, 1.5, 0 });
	dotArr.push_back(new double[] { -0.5, 3, 0 });
	dotArr.push_back(new double[] { 0.5, 0.5, 0 });
	dotArr.push_back(new double[] { 3, 1, 0 });
	dotArr.push_back(new double[] { 0.5, -0.5, 0 });

	std::vector<double*> dotArr2;
	dotArr2.push_back(new double[] { 1.5, -3, 0 });
	dotArr2.push_back(new double[] { -2, -3.5, 0 });
	dotArr2.push_back(new double[] { -0.5, -0.5, 0 });
	dotArr2.push_back(new double[] { -4, 1.5, 0 });
	dotArr2.push_back(new double[] { -0.5, 3, 0 });
	dotArr2.push_back(new double[] { 0.5, 0.5, 0 });
	dotArr2.push_back(new double[] { 3, 1, 0 });
	dotArr2.push_back(new double[] { 0.5, -0.5, 0 });
#pragma endregion

	double height = 4;
	double angle = 0;
	double numIter = 1;

	addZ(&dotArr2, height);
	std::vector<double*> BotomCircleArr = circleDot(2000, dotArr[0], dotArr[1]);
	std::vector<double*> TopCircleArr = circleDot(2000, dotArr2[0], dotArr2[1]);

#pragma region ��������� �������������


	glNormal3d(0, 0, -1);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 1, 0);
	double* centerDot = getCenterDot(dotArr[0], dotArr[1]);
	glVertex3d(centerDot[0], centerDot[1], 0);
	glVertex3dv(dotArr[0]);
	

	for (int i = 0; i < BotomCircleArr.size(); i++)
	{
		glTexCoord2d(BotomCircleArr[i][0], BotomCircleArr[i][1]);
		glVertex3dv(BotomCircleArr[i]);
	}
	
	glVertex3dv(dotArr[1]);
	glEnd();

	glNormal3d(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 1, 0);
	centerDot = getCenterDot(dotArr2[0], dotArr2[1]);
	glVertex3d(centerDot[0], centerDot[1], height);
	glVertex3dv(dotArr2[0]);
	for (int i = 0; i < TopCircleArr.size(); i++)
	{
		glTexCoord2d(TopCircleArr[i][0], TopCircleArr[i][1]);
		glVertex3dv(TopCircleArr[i]);
	}
	glVertex3dv(dotArr2[1]);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(1, 0, 1);
	for (int i = 0; i < BotomCircleArr.size()-1; i++)
	{
		
		double normalVector[3] = { 0,0,0 };
		GetNormal(BotomCircleArr[i], TopCircleArr[i], TopCircleArr[i+1], normalVector);
		glNormal3dv(normalVector);
		glTexCoord2d(BotomCircleArr[i][0], BotomCircleArr[i][1]);
		glVertex3dv(BotomCircleArr[i]);
		glVertex3dv(TopCircleArr[i]);
	}
	glVertex3dv(dotArr[1]);
	glVertex3dv(dotArr2[1]);
	glEnd();

#pragma endregion

#pragma region ��������� ������

#pragma region �����
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0, 0, 1);
	glNormal3d(0, 0, -1);
	for (int i = 7; i > -1; i--) {
		glTexCoord3dv(dotArr[i]);
		glVertex3dv(dotArr[i]);
	}
	glEnd();
#pragma endregion

#pragma region  ������
	glNormal3d(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0, 0, 1);

	for (int i = 7; i > -1; i--) {
		glTexCoord3dv(dotArr[i]);
		glVertex3dv(dotArr2[i]);
	}
	glEnd();
#pragma endregion

#pragma region ������
	glBegin(GL_TRIANGLES);

	glColor3d(0.2, 0.8, 0.2);
	//glVertex3dv(dotArr[0]);
	//glVertex3dv(dotArr2[0]);
	std::vector<double> triangle;

	
	for (int i = 1; i < 7 ; i++) {
		glTexCoord2d(0,0);
		double normalVector[3] = { 0,0,0 };
		GetNormal(dotArr[i], dotArr2[i], dotArr[i + 1], normalVector);
		glNormal3dv(normalVector);
		glVertex3dv(dotArr[i]);
		glTexCoord2d(7, 0);
		glVertex3dv(dotArr2[i]);
		glTexCoord2d(0, 7);
		glVertex3dv(dotArr[i+1]);

	}
	for (int i = 1; i < 7; i++) {
		glTexCoord2d(7, 7);
		double normalVector[3] = { 0,0,0 };
		GetNormal(dotArr[i+1], dotArr2[i], dotArr2[i + 1], normalVector);
		glNormal3dv(normalVector);
		glVertex3dv(dotArr2[i]);
		glTexCoord2d(7, 0);
		glVertex3dv(dotArr[i+1]);
		glTexCoord2d(0, 7);
		glVertex3dv(dotArr2[i + 1]);
	}

	double normalVector[3] = { 0,0,0 };
	GetNormal(dotArr[7], dotArr2[7], dotArr[0], normalVector);
	glNormal3dv(normalVector);
	glTexCoord2d(7, 7);
	glVertex3dv(dotArr[7]);
	glTexCoord2d(7, 0);
	glVertex3dv(dotArr2[7]);
	glTexCoord2d(0, 7);
	glVertex3dv(dotArr[0]);
	

	GetNormal(dotArr[0], dotArr2[0], dotArr2[7], normalVector);
	glTexCoord2d(7, 7);
	glVertex3dv(dotArr2[7]);
	glTexCoord2d(7, 0);
	glVertex3dv(dotArr[0]);
	glTexCoord2d(0, 7);
	glVertex3dv(dotArr2[0]);

	glEnd();
#pragma endregion



#pragma endregion



#pragma endregion


	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}