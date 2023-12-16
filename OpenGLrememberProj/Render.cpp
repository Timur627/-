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

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;
bool alphaBlending = false;

//êëàññ äëÿ íàñòðîéêè êàìåðû
class CustomCamera : public Camera
{
public:
	//äèñòàíöèÿ êàìåðû
	double camDist;
	//óãëû ïîâîðîòà êàìåðû
	double fi1, fi2;

	
	//çíà÷íèÿ ìàñåðû ïî óìîë÷àíèþ
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//ñ÷èòàåò ïîçèöèþ êàìåðû, èñõîäÿ èç óãëîâ ïîâîðîòà, âûçûâàåòñÿ äâèæêîì
	void SetUpCamera()
	{
		//îòâå÷àåò çà ïîâîðîò êàìåðû ìûøêîé
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//ôóíêöèÿ íàñòðîéêè êàìåðû
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //ñîçäàåì îáúåêò êàìåðû


//Êëàññ äëÿ íàñòðîéêè ñâåòà
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//íà÷àëüíàÿ ïîçèöèÿ ñâåòà
		pos = Vector3(1, 1, 3);
	}

	
	//ðèñóåò ñôåðó è ëèíèè ïîä èñòî÷íèêîì ñâåòà, âûçûâàåòñÿ äâèæêîì
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//ëèíèÿ îò èñòî÷íèêà ñâåòà äî îêðóæíîñòè
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//ðèñóåì îêðóíîñòü
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ïàðàìåòðû èñòî÷íèêà ñâåòà
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// õàðàêòåðèñòèêè èçëó÷àåìîãî ñâåòà
		// ôîíîâîå îñâåùåíèå (ðàññåÿííûé ñâåò)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// äèôôóçíàÿ ñîñòàâëÿþùàÿ ñâåòà
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// çåðêàëüíî îòðàæàåìàÿ ñîñòàâëÿþùàÿ ñâåòà
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //ñîçäàåì èñòî÷íèê ñâåòà




//ñòàðûå êîîðäèíàòû ìûøè
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//ìåíÿåì óãëû êàìåðû ïðè íàæàòîé ëåâîé êíîïêå ìûøè
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//äâèãàåì ñâåò ïî ïëîñêîñòè, â òî÷êó ãäå ìûøü
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

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
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

	if (key == 'Y')
	{
		alphaBlending = !alphaBlending;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//âûïîëíÿåòñÿ ïåðåä ïåðâûì ðåíäåðîì
void initRender(OpenGL *ogl)
{
	//íàñòðîéêà òåêñòóð

	//4 áàéòà íà õðàíåíèå ïèêñåëÿ
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//íàñòðîéêà ðåæèìà íàëîæåíèÿ òåêñòóð
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//âêëþ÷àåì òåêñòóðû
	glEnable(GL_TEXTURE_2D);
	

	//ìàññèâ òðåõáàéòíûõ ýëåìåíòîâ  (R G B)
	RGBTRIPLE *texarray;

	//ìàññèâ ñèìâîëîâ, (âûñîòà*øèðèíà*4      4, ïîòîìó÷òî   âûøå, ìû óêàçàëè èñïîëüçîâàòü ïî 4 áàéòà íà ïèêñåëü òåêñòóðû - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//ãåíåðèðóåì ÈÄ äëÿ òåêñòóðû
	glGenTextures(1, &texId);
	//áèíäèì àéäèøíèê, âñå ÷òî áóäåò ïðîèñõîäèòü ñ òåêñòóðîé, áóäòå ïðîèñõîäèòü ïî ýòîìó ÈÄ
	glBindTexture(GL_TEXTURE_2D, texId);

	//çàãðóæàåì òåêñòóðó â âèäåîïÿìÿòü, â îïåðàòèâêå íàì áîëüøå  îíà íå íóæíà
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//îò÷èñòêà ïàìÿòè
	free(texCharArray);
	free(texarray);

	//íàâîäèì øìîí
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//êàìåðó è ñâåò ïðèâÿçûâàåì ê "äâèæêó"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// íîðìàëèçàöèÿ íîðìàëåé : èõ äëèíû áóäåò ðàâíà 1
	glEnable(GL_NORMALIZE);

	// óñòðàíåíèå ñòóïåí÷àòîñòè äëÿ ëèíèé
	glEnable(GL_LINE_SMOOTH); 


	//   çàäàòü ïàðàìåòðû îñâåùåíèÿ
	//  ïàðàìåòð GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ëèöåâûå è èçíàíî÷íûå ðèñóþòñÿ îäèíàêîâî(ïî óìîë÷àíèþ), 
	//                1 - ëèöåâûå è èçíàíî÷íûå îáðàáàòûâàþòñÿ ðàçíûìè ðåæèìàìè       
	//                ñîîòâåòñòâåííî ëèöåâûì è èçíàíî÷íûì ñâîéñòâàì ìàòåðèàëîâ.    
	//  ïàðàìåòð GL_LIGHT_MODEL_AMBIENT - çàäàòü ôîíîâîå îñâåùåíèå, 
	//                íå çàâèñÿùåå îò ñòî÷íèêîâ
	// ïî óìîë÷àíèþ (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void calculateNormal(const double point1[3], const double point2[3], const double point3[3], double normal[3], int invert) {
	double vector1[3];
	double vector2[3];

	// Вычисляем векторы, соединяющие точки 1-2 и 1-3
	for (int i = 0; i < 3; ++i) {
		vector1[i] = point2[i] - point1[i];
		vector2[i] = point3[i] - point1[i];
	}

	// Вычисляем нормаль как векторное произведение векторов 1-2 и 1-3
	normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
	normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
	normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

	// Проверяем значение параметра invert
	if (invert == 1) {
		// Инвертируем знаки компонентов нормали, если invert равно 1
		for (int i = 0; i < 3; ++i) {
			normal[i] = -normal[i];
		}
	}
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//àëüôàíàëîæåíèå

	if (alphaBlending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	//íàñòðîéêà ìàòåðèàëà
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//ôîíîâàÿ
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//äèôóçíàÿ
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//çåðêàëüíàÿ
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//ðàçìåð áëèêà
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//÷òîá áûëî êðàñèâî, áåç êâàäðàòèêîâ (ñãëàæèâàíèå îñâåùåíèÿ)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Ïðîãàòü òóò  


	//Íà÷àëî ðèñîâàíèÿ êâàäðàòèêà ñòàíêèíà
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	double g1[] = { -3, -5, 0 };
	double g2[] = { 2, -8, 0 };
	double a[] = { -1, 0, 0 };
	double b[] = { 1, 1, 0 };
	double d[] = { -6, 3, 0 };
	double c[] = { 5, 3, 0 };
	double f1[] = { -2, 8, 0 };
	double f2[] = { 5, 7, 0 };

	double g11[] = { -3, -5, 1 };
	double g21[] = { 2, -8, 1 };
	double a1[] = { -1, 0, 1 };
	double b1[] = { 1, 1, 1 };
	double d1[] = { -6, 3, 1 };
	double c1[] = { 5, 3, 1 };
	double f11[] = { -2, 8, 1 };
	double f21[] = { 5, 7, 1 };

	double normal[] = { 0, 0, 0 };

	glBindTexture(GL_TEXTURE_2D, texId);

	//glColor3d(0.6, 0.6, 0.1);
	glColor4d(0.6, 0.6, 0.1, 1);
	glBegin(GL_QUADS);

	/*glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 0.35);
	glVertex2dv(D);*/

	glEnd();
	//êîíåö ðèñîâàíèÿ êâàäðàòèêà ñòàíêèíà

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3dv(a);
	glVertex3dv(g1);
	glVertex3dv(g2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(g2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3dv(a);
	glVertex3dv(d);
	glVertex3dv(f1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3dv(a);
	glVertex3dv(f1);
	glVertex3dv(f2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(f2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(f2);
	glEnd();


	glBegin(GL_QUADS);
	glColor4d(0.1, 0.6, 0.1, 1);
	calculateNormal(f1, f2, f21, normal, 1);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(f1);
	glVertex3dv(f2);
	glVertex3dv(f21);
	glVertex3dv(f11);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(f1[0], f1[1], f1[2]);
	glVertex3d(f1[0] + normal[0], f1[1]+ normal[1], f1[2]+normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(g1, g2, g21, normal, 0);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(g1);
	glVertex3dv(g2);
	glVertex3dv(g21);
	glVertex3dv(g11);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(g21[0], g21[1], g21[2]);
	glVertex3d(g21[0] + normal[0], g21[1] + normal[1], g21[2] + normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(f1, f11, d1, normal, 1);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(d);
	glVertex3dv(f1);
	glVertex3dv(f11);
	glVertex3dv(d1);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(f11[0], f11[1], f11[2]);
	glVertex3d(f11[0] + normal[0], f11[1] + normal[1], f11[2] + normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(a, a1, d1, normal, 0);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(d);
	glVertex3dv(a);
	glVertex3dv(a1);
	glVertex3dv(d1);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(d[0], d[1], d[2]);
	glVertex3d(d[0] + normal[0], d[1] + normal[1], d[2] + normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(a, g1, g11, normal, 0);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(a);
	glVertex3dv(g1);
	glVertex3dv(g11);
	glVertex3dv(a1);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(a[0], a[1], a[2]);
	glVertex3d(a[0] + normal[0], a[1] + normal[1], a[2] + normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(g2, g21, b1, normal, 1);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(b);
	glVertex3dv(g2);
	glVertex3dv(g21);
	glVertex3dv(b1);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(g2[0], g2[1], g2[2]);
	glVertex3d(g2[0] + normal[0], g2[1] + normal[1], g2[2] + normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(b, c, c1, normal, 0);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(c1);
	glVertex3dv(b1);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(b[0], b[1], b[2]);
	glVertex3d(b[0] + normal[0], b[1] + normal[1], b[2] + normal[2]);
	glEnd();*/

	glBegin(GL_QUADS);
	calculateNormal(c, c1, f2, normal, 1);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(c);
	glVertex3dv(f2);
	glVertex3dv(f21);
	glVertex3dv(c1);
	glEnd();

	/*glBegin(GL_LINES);
	glVertex3d(f2[0], f2[1], f2[2]);
	glVertex3d(f2[0] + normal[0], f2[1] + normal[1], f2[2] + normal[2]);
	glEnd();*/

	glBegin(GL_TRIANGLES);
	glColor4d(1, 0.6, 0.5, 0.8);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.45, 0.45);
	glVertex3dv(a1);
	glTexCoord2d(0.35, 0.2);
	glVertex3dv(g11);
	glTexCoord2d(0.6, 0.05);
	glVertex3dv(g21);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.45, 0.45);
	glVertex3dv(a1);
	glTexCoord2d(0.55, 0.5);
	glVertex3dv(b1);
	glTexCoord2d(0.6, 0.05);
	glVertex3dv(g21);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.45, 0.45);
	glVertex3dv(a1);
	glTexCoord2d(0.25, 0.6);
	glVertex3dv(d1);
	glTexCoord2d(0.4, 0.85);
	glVertex3dv(f11);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.45, 0.45);
	glVertex3dv(a1);
	glTexCoord2d(0.4, 0.85);
	glVertex3dv(f11);
	glTexCoord2d(0.75, 0.8);
	glVertex3dv(f21);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.55, 0.5);
	glVertex3dv(b1);
	glTexCoord2d(0.75, 0.6);
	glVertex3dv(c1);
	glTexCoord2d(0.75, 0.8);
	glVertex3dv(f21);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.45, 0.45);
	glVertex3dv(a1);
	glTexCoord2d(0.55, 0.5);
	glVertex3dv(b1);
	glTexCoord2d(0.75, 0.8);
	glVertex3dv(f21);
	glEnd();

   //Ñîîáùåíèå ââåðõó ýêðàíà

	
	glMatrixMode(GL_PROJECTION);	//Äåëàåì àêòèâíîé ìàòðèöó ïðîåêöèé. 
	                                //(âñåê ìàòðè÷íûå îïåðàöèè, áóäóò åå âèäîèçìåíÿòü.)
	glPushMatrix();   //ñîõðàíÿåì òåêóùóþ ìàòðèöó ïðîåöèðîâàíèÿ (êîòîðàÿ îïèñûâàåò ïåðñïåêòèâíóþ ïðîåêöèþ) â ñòåê 				    
	glLoadIdentity();	  //Çàãðóæàåì åäèíè÷íóþ ìàòðèöó
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //âðóáàåì ðåæèì îðòîãîíàëüíîé ïðîåêöèè

	glMatrixMode(GL_MODELVIEW);		//ïåðåêëþ÷àåìñÿ íà ìîäåë-âüþ ìàòðèöó
	glPushMatrix();			  //ñîõðàíÿåì òåêóùóþ ìàòðèöó â ñòåê (ïîëîæåíèå êàìåðû, ôàêòè÷åñêè)
	glLoadIdentity();		  //ñáðàñûâàåì åå â äåôîëò

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //êëàññèê ìîåãî àâòîðñòâà äëÿ óäîáíîé ðàáîòû ñ ðåíäåðîì òåêñòà.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "Y - вкл/выкл альфа наложение" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //âîññòàíàâëèâàåì ìàòðèöû ïðîåêöèè è ìîäåë-âüþ îáðàòüíî èç ñòåêà.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}