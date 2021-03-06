#include "stdafx.h"
#include <Windows.h>
#include <gl/glut.h>
#include <GL/GLAUX.H>
#include <cmath>

#define windowWidth 600
#define windowHeight 600
#define planets 3
#define PI 3.1415926
#define ES_distant 150000 //地日距离
#define ER 10000 //地球半径
#define SR 50000 //太阳半径
#define ME_distant 30000//地月距离
#define MR 5000

//视角在一个球面上运动，将视角位置表达为半径与两个角度
typedef struct
{
	double zAngle;  //以z轴为中轴旋转的角度
	double xyAngle; //以xy平面上的一条线为中轴旋转的角度
	double radius;
	double x;
	double y;
	double z;
}viewPosition;

//1.完成基本模型绘制：圆形轨道，单色球体 done
//2.完成光照效果  done
//3.轨道角度  done
//4.抗锯齿
//5.添加月亮与其他行星  done
//6.完成3d贴图  done
//7.自转  done
//8.视角变换 done

double cnt=0; //用于计时函数的计数器，并用于计算各行星旋转的角度
// 材质标记数组，暂时为2，太阳与地球
GLuint Texture[planets];
GLUquadricObj *Surface[planets];
viewPosition view;
bool EARTHFOCUS = false;
bool SHOWORBIT = false;
double speedFactor = 20;
//将旋转角度改为步进方式计算
double sunRotate = 0, earthRotate = 0, earthSelfRotate = 0, moonRotate = 0;
double sunRotateStep = 360.0 / 25.0 / 10.0;
double moonRotateStep = 360.0 / 27.0 / 10.0;
double earthRotateStep = 365.0 / 360.0 / 10.0;
double earthCoordX, earthCoordY;  //地球当前坐标

void universeTimer(int a);
void updateSun();
void updateEarth();
void updateMoon();
void drawSolarSystem();
void calculatePosition();
void keyDControl(int key, int x, int y);
void keyScaleControl(unsigned char key, int x, int y);
void drawOrbit();

void drawOrbit() //画轨道
{
	glPushMatrix();
	glutWireTorus(300, ES_distant, 100, 100);
	glTranslatef(earthCoordX, earthCoordY, 0.0);
	glutWireTorus(200, ME_distant, 100, 100);
	glPopMatrix();
}

void calculatePosition()
{
	double arcxy = view.xyAngle*PI / 180.0;
	double arcz = view.zAngle*PI / 180.0;
	view.x = view.radius*cos(arcxy)*cos(arcz);
	view.y = view.radius*cos(arcxy)*sin(arcz);
	view.z = view.radius*sin(arcxy);
}

//计时函数
void universeTimer(int a)
{
	//假设以天为单位，每10个cnt为1天
	cnt++;
	if (cnt >= 10000000)
		cnt = 0;    //防止溢出
	drawSolarSystem();
	glutTimerFunc(10, universeTimer, 0);
}

//更新太阳位置
void updateSun()
{
	Surface[0] = gluNewQuadric(); //使用曲面创建球
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	gluQuadricTexture(Surface[0], GL_TRUE);//纹理函数

	//glColor3f(1.0, 0.0, 0.0);
	//glColor3d(244.0 / 255.0, 116.0 / 255.0, 7.0 / 255.0);
	//太阳自转周期25天
	sunRotate += sunRotateStep / speedFactor;
	glRotated(sunRotate, 0.0, 0.0, -1.0);
	glDisable(GL_LIGHTING);
	//太阳不设置光照，自身最亮
	gluSphere(Surface[0],SR, 100, 100);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

//更新地球位置
void updateEarth()
{
	Surface[1] = gluNewQuadric(); //使用曲面创建球
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	gluQuadricTexture(Surface[1], GL_TRUE);//纹理函数

	//glColor3d(33.0/255.0, 41.0/255.0, 78.0/255.0);//蓝色
	earthRotate += earthRotateStep / speedFactor;
	glRotatef(earthRotate, 0.0, 0.0, -1.0);
	glTranslatef(ES_distant, 0.0, 0.0);
	earthSelfRotate += 36.0 / speedFactor;
	glRotatef(earthSelfRotate, -3.9, 0.0, 9.2); //地球自转

	GLfloat earthAmbientDiffuse[4] = { 0.0, 0.0, 0.0, 1.0 }; //没有漫射光以及反射的环境光
	GLfloat earthSpecular[4] = { 0.4, 0.4, 0.4, 0.5 };
	GLfloat earthShiness = 0.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, earthAmbientDiffuse); 
	glMaterialfv(GL_FRONT, GL_SPECULAR, earthSpecular); //主要反射光的成分为镜面反射
	glMaterialf(GL_FRONT, GL_SHININESS, earthShiness);  //材质设置为最粗糙

	gluSphere(Surface[1],ER, 100, 100);
	glPopMatrix();
}

//月亮
void updateMoon()
{
	Surface[2] = gluNewQuadric(); //使用曲面创建球
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, Texture[2]);
	gluQuadricTexture(Surface[2], GL_TRUE);//纹理函数
	moonRotate += moonRotateStep / speedFactor;
	glRotatef(earthRotate, 0.0, 0.0, -1.0);
	glTranslatef(ES_distant, 0.0, 0.0);
	glRotatef(moonRotate, 0.0, 0.0, 1.0); //月亮围绕地球
	glTranslatef(ME_distant, 0.0, 0.0);

	GLfloat earthAmbientDiffuse[4] = { 0.0, 0.0, 0.0, 1.0 }; //没有漫射光以及反射的环境光
	GLfloat earthSpecular[4] = { 0.4, 0.4, 0.4, 0.5 };
	GLfloat earthShiness = 0.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, earthAmbientDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, earthSpecular); //主要反射光的成分为镜面反射
	glMaterialf(GL_FRONT, GL_SHININESS, earthShiness);  //材质设置为最粗糙

	gluSphere(Surface[2], MR, 100, 100);
	glPopMatrix();
}

void loadTexture()
{
	AUX_RGBImageRec *Image[planets];
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //这是干嘛用的
	glGenTextures(planets, Texture);
	//绑定并设置太阳材质
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	//设定缩放模式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if (Image[0] = auxDIBImageLoadA("sun.bmp")) 
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Image[0]->sizeX, Image[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Image[0]->data);
	
	//绑定地球材质
	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	//设定缩放模式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if (Image[1] = auxDIBImageLoadA("earth.bmp"))
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Image[1]->sizeX, Image[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Image[1]->data);

	//绑定月球材质
	glBindTexture(GL_TEXTURE_2D, Texture[2]);
	//设定缩放模式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if (Image[2] = auxDIBImageLoadA("moon.bmp"))
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Image[2]->sizeX, Image[2]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Image[2]->data);

}

//整体绘制更新
void drawSolarSystem()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空原有显示数据
	//启用抗锯齿
	//glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	//glEnable(GL_POLYGON_SMOOTH);
	//glMatrixMode(GL_PROJECTION); //操作投影矩阵
	//glLoadIdentity(); //载入单位矩阵（将当前坐标重置为初始坐标）
	//gluPerspective(70, windowWidth / windowHeight, 1, 1000000);//设置视角

    //glMatrixMode(GL_MODELVIEW);//操作模型矩阵

	//计算地球坐标，用于地球视角与计算月球轨迹
	earthCoordX = ES_distant * cos(-earthRotate * PI / 180.0);
	earthCoordY = ES_distant * sin(-earthRotate * PI / 180.0);
	glLoadIdentity();
	if (EARTHFOCUS)
		gluLookAt(view.x+earthCoordX, view.y+earthCoordY, view.z,earthCoordX ,earthCoordY , 0, 0, 0, 1);//设置观察位置
	else
		gluLookAt(view.x, view.y, view.z, 0, 0, 0, 0, 0, 1);//设置观察位置
	
	//光属性使用默认值
	//设置太阳处的光源
	GLfloat lightPosition[4] = { 0.0,0.0,0.0,1.0 }; //处于太阳处
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	GLfloat lightAmbient[4] = { 0.2,0.2,0.2,0.5 }; //为了使得各个球体有一定亮度以方便观察，使其有环境光属性
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

	updateSun();
	updateEarth();
	updateMoon();
	if(SHOWORBIT)
		drawOrbit();
	//交换缓冲区
	glutSwapBuffers();
}

//键盘+-控制缩放
void keyScaleControl(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'o':
		SHOWORBIT = SHOWORBIT ? false : true;
		break;
	case 'q':   //q键放大视角
		if (view.radius < 300000)
			view.radius += 2000;
		break;
	case 'w':   //w键缩小视角
		if (view.radius > 30000)
			view.radius -= 2000;
		break;
	case 'e':   //e键切换地球视角
		if (EARTHFOCUS)
		{
			EARTHFOCUS = false;
			view.radius = 220000;
			view.xyAngle = view.zAngle = 0;
		}
		else
		{
			EARTHFOCUS = true;
			view.radius = 100000;
		}
		break;
	case 's':   //切换速度
		speedFactor = (speedFactor == 20) ? 1 : 20;
		break;
	default:
		return;
	}
	calculatePosition();
}

//键盘方向键控制方位
void keyDControl(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)  //方向键左键，视角向左旋转
		view.zAngle -= 5;
	else if (key == GLUT_KEY_RIGHT)  //方向键右键，视角向右旋转
		view.zAngle += 5;
	else if (key == GLUT_KEY_UP)  //方向键上键，视角向上旋转
	{
		if (view.xyAngle < 90)
			view.xyAngle += 1;
	}
	else if (key == GLUT_KEY_DOWN)  //方向键下键，视角向下旋转
	{
		if (view.xyAngle > -90)
			view.xyAngle -= 1;
	}
	else
		return;
	calculatePosition();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	//初始化视角位置
	view.radius = 200000;
	view.xyAngle = 0;
	view.zAngle = 0;
	calculatePosition();
	//使用双缓冲模式
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("SEM DEMO");
	//注册键盘事件处理函数
	glutSpecialFunc(&keyDControl);
	glutKeyboardFunc(&keyScaleControl);

	glMatrixMode(GL_PROJECTION); //操作投影矩阵
	glLoadIdentity(); //载入单位矩阵（将当前坐标重置为初始坐标）
	gluPerspective(70, windowWidth / windowHeight, 1, 1000000);//设置视角
	glMatrixMode(GL_MODELVIEW);//操作模型矩阵
	glLoadIdentity();
	gluLookAt(view.x, view.y, view.z, 0, 0, 0, 0, 0, 1);//设置初始观察位置

	glEnable(GL_COLOR_MATERIAL); //使得球体保持原有颜色
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//使用光照
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	//设置太阳处的光源
	GLfloat lightPosition[4] = { 0.0,0.0,0.0,1.0 }; //处于太阳处
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	GLfloat lightAmbient[4] = { 0.2,0.2,0.2,0.5 }; //为了使得各个球体有一定亮度以方便观察，使其有环境光属性
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

	glutDisplayFunc(&drawSolarSystem);
	loadTexture();
	glutTimerFunc(15, universeTimer, 0);
	glutMainLoop();
	return 0;
}






