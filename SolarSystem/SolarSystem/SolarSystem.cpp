#include "stdafx.h"
#include <Windows.h>
#include <gl/glut.h>

#define windowWidth 600
#define windowHeight 600

//1.完成基本模型绘制：圆形轨道，单色球体 done
//2.完成光照效果  done
//3.轨道转椭圆  
//4.抗锯齿
//5.添加月亮与其他行星
//6.完成3d贴图
//7.自转
//8.视角变换

double cnt=0; //用于计时函数的计数器，并用于计算各行星旋转的角度

void universeTimer(int a);
void updateSun();
void updateEarth();
void drawSolarSystem();

//计时函数
void universeTimer(int a)
{
	//假设以天为单位，每0.1秒一天
	//Sleep(100);
	cnt++;
	//if (cnt >= 1000)
		//cnt = 0;
	drawSolarSystem();
	glutTimerFunc(20, universeTimer, 0);
}

/*
太阳 1392000 km
水星直径 4878 km
金星直径 12103.6 km
地球直径 12756.3 km
火星直径 6794 km
木星直径 142984 km
土星直径 120536 km
天王星直径 51118 km
海王星直径 49,528 km
*/

//更新太阳位置
void updateSun()
{
	glPushMatrix();
	//glColor3f(1.0, 0.0, 0.0);
	glColor3d(244.0 / 255.0, 116.0 / 255.0, 7.0 / 255.0);
	//太阳自转周期25天
	glRotated((25.0 / 360.0)*cnt / 360.0, 0.0, 0.0, -1.0);

	GLfloat sunAmbientDiffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sunAmbientDiffuse); //太阳不反射
	//GLfloat sunEmission[4] = { 1.0, 1.0, 1.0, 1.0 };
	//glMaterialfv(GL_FRONT, GL_SHININESS, sunEmission); //太阳本身发光

	glutSolidSphere(18000, 100, 100);
	glPopMatrix();
}

//更新地球位置
void updateEarth()
{
	glPushMatrix();
	glColor3d(33.0/255.0, 41.0/255.0, 78.0/255.0);//蓝色
	glRotatef((365.0 / 360.0)*cnt, 0.0, 0.0, -1.0);
	glTranslatef(100000, 0.0, 0.0);

	GLfloat earthAmbientDiffuse[4] = { 0.0, 0.0, 0.0, 1.0 }; //没有漫射光以及反射的环境光
	GLfloat earthSpecular[4] = { 0.2, 0.2, 0.2, 0.5 };
	GLfloat earthShiness = 0.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, earthAmbientDiffuse); 
	glMaterialfv(GL_FRONT, GL_SPECULAR, earthSpecular); //主要反射光的成分为镜面反射
	glMaterialf(GL_FRONT, GL_SHININESS, earthShiness);  //材质设置为最粗糙

	glutSolidSphere(15000, 100, 100);
	glPopMatrix();
}

//整体绘制更新
void drawSolarSystem()
{
	glEnable(GL_COLOR_MATERIAL); //使得球体保持原有颜色

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空原有显示数据
	//启用抗锯齿
	//glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	//glEnable(GL_POLYGON_SMOOTH);
	glMatrixMode(GL_PROJECTION); //操作投影矩阵
	glLoadIdentity(); //载入单位矩阵（将当前坐标重置为初始坐标）
	gluPerspective(70, windowWidth / windowHeight, 1, 1000000);//设置视角
	glMatrixMode(GL_MODELVIEW);//操作模型矩阵
	glLoadIdentity();
	gluLookAt(0, -200000, 100000, 0, 0, 0, 0, 0, 1);//设置观察位置

	//设置太阳处的光源
	GLfloat lightPosition[4] = { 0.0,0.0,0.0,1.0 }; //处于太阳处
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	GLfloat lightAmbient[4] = { 1.0,1.0,1.0,0.5 }; //为了使得各个球体有一定亮度以方便观察，使其有环境光属性
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	
	//光属性使用默认值

	//使用光照
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	updateSun();
	updateEarth();
	//交换缓冲区
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	//使用双缓冲模式
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("solar system demo");
	glutDisplayFunc(&drawSolarSystem);
	glutTimerFunc(20, universeTimer, 0);
	glutMainLoop();
	return 0;
}






