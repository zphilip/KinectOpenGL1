// Kinect_glTest.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#WWW.CNKINECT.COM
#include <GL/glut.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>

using namespace std;
using namespace cv;

#define SIGN(x) ( (x)<0 ? -1:((x)>0?1:0 ) )

//////////////////////////////////////////////////////////////////////////
// 
//---OpenGL 全局变量
float xyzdata[480][640][3];
float texture[480][640][3];
int glWinWidth = 640, glWinHeight = 480;
int width=640, height=480;
double eyex, eyey, eyez, atx, aty, atz; // eye* - 摄像机位置，at* - 注视点位置

bool leftClickHold = false, rightClickHold = false;
int mx,my; // 鼠标按键时在 OpenGL 窗口的坐标
int ry = 90, rx = 90; // 摄像机相对注视点的观察角度
double mindepth, maxdepth; // 深度数据的极值 
double radius = 6000.0; // 摄像机与注视点的距离



/************************************************************************/
/* OpenGL响应函数 */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// 鼠标按键响应函数
void mouse(int button, int state, int x, int y)
{
if(button == GLUT_LEFT_BUTTON)
{
if(state == GLUT_DOWN)
{
leftClickHold=true;
}
else
{
leftClickHold=false;
}
}

if (button== GLUT_RIGHT_BUTTON)
{
if(state == GLUT_DOWN)
{
rightClickHold=true;
}
else
{
rightClickHold=false;
}
}
}

//////////////////////////////////////////////////////////////////////////
// 鼠标运动响应函数
void motion(int x, int y)
{
int rstep = 5; 
if(leftClickHold==true)
{
if( abs(x-mx) > abs(y-my) )
{
rx += SIGN(x-mx)*rstep; 
}
else
{
ry -= SIGN(y-my)*rstep; 
}

mx=x;
my=y;
glutPostRedisplay();
}

if(rightClickHold==true)
{
radius += SIGN(y-my)*100.0;
radius = std::max( radius, 100.0 );
mx=x;
my=y;
glutPostRedisplay();
}
}

//////////////////////////////////////////////////////////////////////////
// 三维图像显示响应函数
void renderScene(void) 
{
// clear screen and depth buffer
glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
// Reset the coordinate system before modifying 
glLoadIdentity(); 
// set the camera position
atx = 0.0f;
aty = 0.0f;
atz = ( mindepth - maxdepth ) / 2.0f;
eyex = atx + radius * sin( CV_PI * ry / 180.0f ) * cos( CV_PI * rx/ 180.0f ); 
eyey = aty + radius * cos( CV_PI * ry/ 180.0f ); 
eyez = atz + radius * sin( CV_PI * ry / 180.0f ) * sin( CV_PI * rx/ 180.0f );
gluLookAt (eyex, eyey, eyez, atx, aty, atz, 0.0, 1.0, 0.0);
glRotatef(0,0,1,0);
glRotatef(-180,1,0,0);

// 对点云数据进行三角化
// 参考自：http://www.codeproject.com/KB/openGL/OPENGLTG.aspx
// we are going to loop through all of our terrain's data points,
// but we only want to draw one triangle strip for each set along the x-axis.
for (int i = 0; i < height; i++)
{
glBegin(GL_TRIANGLE_STRIP);
for (int j = 0; j < width; j++)
{
// for each vertex, we calculate the vertex color, 
// we set the texture coordinate, and we draw the vertex.
/*
the vertexes are drawn in this order:

0 ---> 1
/
/
/
2 ---> 3
*/

// draw vertex 0
glTexCoord2f(0.0f, 0.0f);
glColor3f(texture[i][j][0]/255.0f, texture[i][j][1]/255.0f, texture[i][j][2]/255.0f);
glVertex3f(xyzdata[i][j][0], xyzdata[i][j][1], xyzdata[i][j][2]);

// draw vertex 1
glTexCoord2f(1.0f, 0.0f);
glColor3f(texture[i+1][j][0]/255.0f, texture[i+1][j][1]/255.0f, texture[i+1][j][2]/255.0f);
glVertex3f(xyzdata[i+1][j][0], xyzdata[i+1][j][1], xyzdata[i+1][j][2]);

// draw vertex 2
glTexCoord2f(0.0f, 1.0f);
glColor3f(texture[i][j+1][0]/255.0f, texture[i][j+1][1]/255.0f, texture[i][j+1][2]/255.0f);
glVertex3f(xyzdata[i][j+1][0], xyzdata[i][j+1][1], xyzdata[i][j+1][2]);

// draw vertex 3
glTexCoord2f(1.0f, 1.0f);
glColor3f(texture[i+1][j+1][0]/255.0f, texture[i+1][j+1][1]/255.0f, texture[i+1][j+1][2]/255.0f);
glVertex3f(xyzdata[i+1][j+1][0], xyzdata[i+1][j+1][1], xyzdata[i+1][j+1][2]);
}
glEnd();
}
// enable blending
glEnable(GL_BLEND);

// enable read-only depth buffer
glDepthMask(GL_FALSE);

// set the blend function to what we use for transparency
glBlendFunc(GL_SRC_ALPHA, GL_ONE);

// set back to normal depth buffer mode (writable)
glDepthMask(GL_TRUE);

// disable blending
glDisable(GL_BLEND);

/* float x,y,z;
// 绘制图像点云
glPointSize(1.0); 
glBegin(GL_POINTS);
for (int i=0;i<height;i++){ 
for (int j=0;j<width;j++){
// color interpolation 
glColor3f(texture[i][j][0]/255, texture[i][j][1]/255, texture[i][j][2]/255);
x= xyzdata[i][j][0];
y= xyzdata[i][j][1]; 
z= xyzdata[i][j][2]; 
glVertex3f(x,y,z); 
}
}
glEnd(); */

glFlush();
glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// 窗口变化图像重构响应函数
void reshape (int w, int h) 
{
glWinWidth = w;
glWinHeight = h;
glViewport (0, 0, (GLsizei)w, (GLsizei)h);
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
gluPerspective (45, (GLfloat)w / (GLfloat)h, 1.0, 15000.0); 
glMatrixMode (GL_MODELVIEW);
}

//////////////////////////////////////////////////////////////////////////
// 载入三维坐标数据
void load3dDataToGL(IplImage* xyz)
{
CvScalar s;
//accessing the image pixels
for (int i=0;i<height;i++)
{ 
for (int j=0;j<width;j++)
{
s=cvGet2D(xyz,i,j); // s.val[0] = x, s.val[1] = y, s.val[2] = z
xyzdata[i][j][0] = s.val[0];
xyzdata[i][j][1] = s.val[1];
xyzdata[i][j][2] = s.val[2];
}
} 
}

//////////////////////////////////////////////////////////////////////////
// 载入图像纹理数据
void loadTextureToGL(IplImage* img)
{ 
CvScalar ss;
//accessing the image pixels
for (int i=0;i<height;i++)
{ 
for (int j=0;j<width;j++)
{
ss=cvGet2D(img,i,j); // ss.val[0] = red, ss.val[1] = green, ss.val[2] = blue
texture[i][j][2] = ss.val[0];
texture[i][j][1] = ss.val[1];
texture[i][j][0] = ss.val[2];
}
} 
}



/************************************************************************/
/* colorizeDisparity */
/* 将视差图由灰度图转换为伪彩色图 */
/************************************************************************/
void colorizeDisparity( const Mat& gray0, Mat& rgb, double maxDisp=-1.f, float S=1.f, float V=1.f )
{
CV_Assert( !gray0.empty() );
Mat gray;
if (gray0.type() == CV_32FC1)
{
gray0.convertTo( gray, CV_8UC1 );
}
else if (gray0.type() == CV_8UC1)
{
gray0.copyTo(gray);
} 
else
{
return;
}

if( maxDisp <= 0 )
{
maxDisp = 0;
minMaxLoc( gray, 0, &maxDisp );
}

rgb.create( gray.size(), CV_8UC3 );
rgb = Scalar::all(0);
if( maxDisp < 1 )
return;

for( int y = 0; y < gray.rows; y++ )
{
for( int x = 0; x < gray.cols; x++ )
{
uchar d = gray.at<uchar>(y,x);
unsigned int H = ((uchar)maxDisp - d) * 240 / (uchar)maxDisp;

unsigned int hi = (H/60) % 6;
float f = H/60.f - H/60;
float p = V * (1 - S);
float q = V * (1 - f * S);
float t = V * (1 - (1 - f) * S);

Point3f res;

if( hi == 0 ) //R = V, G = t, B = p
res = Point3f( p, t, V );
if( hi == 1 ) // R = q, G = V, B = p
res = Point3f( p, V, q );
if( hi == 2 ) // R = p, G = V, B = t
res = Point3f( t, V, p );
if( hi == 3 ) // R = p, G = q, B = V
res = Point3f( V, q, p );
if( hi == 4 ) // R = t, G = p, B = V
res = Point3f( V, p, t );
if( hi == 5 ) // R = V, G = p, B = q
res = Point3f( q, p, V );

uchar b = (uchar)(std::max(0.f, std::min (res.x, 1.f)) * 255.f);
uchar g = (uchar)(std::max(0.f, std::min (res.y, 1.f)) * 255.f);
uchar r = (uchar)(std::max(0.f, std::min (res.z, 1.f)) * 255.f);

rgb.at<Point3_<uchar> >(y,x) = Point3_<uchar>(b, g, r); 
}
}
}



/************************************************************************/
/* saveData */
/* 保存当前画面的数据 */
/************************************************************************/
void saveData(const char* filename, const Mat& mat, int flag = 0) 
{
FILE* fp = fopen(filename, "wt");
if (3 != flag)
{
fprintf(fp, "%02d\n", mat.rows);
fprintf(fp, "%02d\n", mat.cols);
}
switch (flag)
{
case 0: 
for(int y = 0; y < mat.rows; y++)
{
for(int x = 0; x < mat.cols; x++)
{
short depth = mat.at<short>(y, x); 
fprintf(fp, "%d\n", depth);
}
}
break;
case 1:
for(int y = 0; y < mat.rows; y++)
{
for(int x = 0; x < mat.cols; x++)
{
uchar disp = mat.at<uchar>(y,x);
fprintf(fp, "%d\n", disp);
}
}
break;
case 2:
for(int y = 0; y < mat.rows; y++)
{
for(int x = 0; x < mat.cols; x++)
{
float disp = mat.at<float>(y,x);
fprintf(fp, "%10.4f\n", disp);
}
}
break;
case 3:
for(int y = 0; y < mat.rows; y++)
{
for(int x = 0; x < mat.cols; x++)
{
Vec3f point = mat.at<Vec3f>(y, x); // Vec3f 是 template 类定义
fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
}
}
break;
case 4:
imwrite(filename, mat);
break;
default:
break;
}

fclose(fp);
}



/************************************************************************/
/* 主程序 */
/************************************************************************/
int main(int argc, char** argv)
{
cout << "Kinect opening ..." << endl;
VideoCapture capture( CV_CAP_OPENNI );
cout << "done." << endl;

if( !capture.isOpened() )
{
cout << "Can not open a capture object." << endl;
return -1;
}

capture.set(CV_CAP_OPENNI_DEPTH_GENERATOR_VIEW_POINT, 1.0);
double b = capture.get( CV_CAP_OPENNI_DEPTH_GENERATOR_BASELINE ); // mm
double F = capture.get( CV_CAP_OPENNI_DEPTH_GENERATOR_FOCAL_LENGTH ); // pixels

double q[] =
{
1, 0, 0, -320.0,
0, 1, 0, -240.0,
0, 0, 0, F,
0, 0, 1./b, 0 
};
Mat matQ(4, 4, CV_64F, q);

// Get max disparity
const int minDistance = 400; // mm
float maxDisparity = b * F / minDistance;

Mat depthMap;
Mat xyzMap;
Mat disparityMap;
Mat bgrImage;
Mat colorDisparityMap;
Mat validColorDisparityMap;

// Print some avalible Kinect settings.
cout << "\nDepth generator output mode:" << endl <<
"FRAME_WIDTH " << capture.get( CV_CAP_PROP_FRAME_WIDTH ) << endl <<
"FRAME_HEIGHT " << capture.get( CV_CAP_PROP_FRAME_HEIGHT ) << endl <<
"FRAME_MAX_DEPTH " << capture.get( CV_CAP_PROP_OPENNI_FRAME_MAX_DEPTH ) << " mm" << endl <<
"FPS " << capture.get( CV_CAP_PROP_FPS ) << endl;

cout << "\nImage generator output mode:" << endl <<
"FRAME_WIDTH " << capture.get( CV_CAP_OPENNI_IMAGE_GENERATOR+CV_CAP_PROP_FRAME_WIDTH ) << endl <<
"FRAME_HEIGHT " << capture.get( CV_CAP_OPENNI_IMAGE_GENERATOR+CV_CAP_PROP_FRAME_HEIGHT ) << endl <<
"FPS " << capture.get( CV_CAP_OPENNI_IMAGE_GENERATOR+CV_CAP_PROP_FPS ) << endl;


//////////////////////////////////////////////////////////////////////////
//***OpenGL Window
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
glutInitWindowPosition(10,320);
glutInitWindowSize(glWinWidth, glWinHeight);
glutCreateWindow("3D image");

//////////////////////////////////////////////////////////////////////////
//***OpenCV Window
namedWindow("Colorized disparity map", 1);
namedWindow("Color image", 1);

//////////////////////////////////////////////////////////////////////////
// 开始获取并显示 Kinect 图像
for(;;)
{
depthMap.setTo(cvScalarAll(0));
xyzMap.setTo(cvScalarAll(0));
disparityMap.setTo(cvScalarAll(0));
colorDisparityMap.setTo(cvScalarAll(0));
validColorDisparityMap.setTo(cvScalarAll(0));
bgrImage.setTo(cvScalarAll(0));

// 抓取数据
if( !capture.grab() )
{
cout << "Can not grab images." << endl;
return -1;
}

// 读取深度数据
capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
minMaxLoc( depthMap, &mindepth, &maxdepth );
// 读取视差数据
capture.retrieve( disparityMap, CV_CAP_OPENNI_DISPARITY_MAP_32F ); 
colorizeDisparity( disparityMap, colorDisparityMap, maxDisparity );
colorDisparityMap.copyTo( validColorDisparityMap, disparityMap != 0 ); 
imshow( "Colorized disparity map", validColorDisparityMap );
// 读取彩色图像数据
capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
imshow( "Color image", bgrImage );
// 利用视差数据计算三维点云坐标
cv::reprojectImageTo3D(disparityMap, xyzMap, matQ, true);

//////////////////////////////////////////////////////////////////////////
// OpenGL显示
IplImage glXYZ = xyzMap;
IplImage glTexture = bgrImage;
load3dDataToGL(&glXYZ); // 载入环境三维点云
loadTextureToGL(&glTexture); // 载入纹理数据
glutReshapeFunc (reshape); // 窗口变化时重绘图像
glutDisplayFunc(renderScene); // 显示三维图像 
glutMouseFunc(mouse); // 鼠标按键响应
glutMotionFunc(motion); // 鼠标移动响应
glutPostRedisplay(); // 刷新画面

//////////////////////////////////////////////////////////////////////////
// 按键消息响应
int c = cvWaitKey(30);
if( c == 27 )
break;

// OpenCV 处理键盘响应消息后，再显示 OpenGL 图像
glutMainLoopEvent();
} // 退出循环

if(argc>1)
{
saveData("C:\\Stereo IO Data\\xyz.txt", xyzMap, 3);
saveData("C:\\Stereo IO Data\\depth.txt", depthMap, 0 );
saveData("C:\\Stereo IO Data\\disp.txt", disparityMap, 2 );
Mat image;
bgrImage.convertTo(image, CV_8UC3);
//saveData("C:\\Stereo IO Data\\image.jpg", image, 4);
imwrite("C:\\Stereo IO Data\\image.jpg", image);
}
return 0;
}