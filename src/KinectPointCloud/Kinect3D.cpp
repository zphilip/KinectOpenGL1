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
//---OpenGL È«¾Ö±äÁ¿
float xyzdata[480][640][3];
float texture[480][640][3];
int glWinWidth = 640, glWinHeight = 480;
int width=640, height=480;
double eyex, eyey, eyez, atx, aty, atz; // eye* - ÉãÏñ»úÎ»ÖÃ£¬at* - ×¢ÊÓµãÎ»ÖÃ

bool leftClickHold = false, rightClickHold = false;
int mx,my; // Êó±ê°´¼üÊ±ÔÚ OpenGL ´°¿ÚµÄ×ø±ê
int ry = 90, rx = 90; // ÉãÏñ»úÏà¶Ô×¢ÊÓµãµÄ¹Û²ì½Ç¶È
double mindepth, maxdepth; // Éî¶ÈÊý¾ÝµÄ¼«Öµ 
double radius = 6000.0; // ÉãÏñ»úÓë×¢ÊÓµãµÄ¾àÀë



/************************************************************************/
/* OpenGLÏìÓ¦º¯Êý */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Êó±ê°´¼üÏìÓ¦º¯Êý
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
// Êó±êÔË¶¯ÏìÓ¦º¯Êý
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
// ÈýÎ¬Í¼ÏñÏÔÊ¾ÏìÓ¦º¯Êý
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

// ¶ÔµãÔÆÊý¾Ý½øÐÐÈý½Ç»¯
// ²Î¿¼×Ô£ºhttp://www.codeproject.com/KB/openGL/OPENGLTG.aspx
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
// »æÖÆÍ¼ÏñµãÔÆ
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
// ´°¿Ú±ä»¯Í¼ÏñÖØ¹¹ÏìÓ¦º¯Êý
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
// ÔØÈëÈýÎ¬×ø±êÊý¾Ý
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
// ÔØÈëÍ¼ÏñÎÆÀíÊý¾Ý
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
/* ½«ÊÓ²îÍ¼ÓÉ»Ò¶ÈÍ¼×ª»»ÎªÎ±²ÊÉ«Í¼ */
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
/* ±£´æµ±Ç°»­ÃæµÄÊý¾Ý */
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
Vec3f point = mat.at<Vec3f>(y, x); // Vec3f ÊÇ template Àà¶¨Òå
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
/* Ö÷³ÌÐò */
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
// ¿ªÊ¼»ñÈ¡²¢ÏÔÊ¾ Kinect Í¼Ïñ
for(;;)
{
depthMap.setTo(cvScalarAll(0));
xyzMap.setTo(cvScalarAll(0));
disparityMap.setTo(cvScalarAll(0));
colorDisparityMap.setTo(cvScalarAll(0));
validColorDisparityMap.setTo(cvScalarAll(0));
bgrImage.setTo(cvScalarAll(0));

// ×¥È¡Êý¾Ý
if( !capture.grab() )
{
cout << "Can not grab images." << endl;
return -1;
}

// ¶ÁÈ¡Éî¶ÈÊý¾Ý
capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
minMaxLoc( depthMap, &mindepth, &maxdepth );
// ¶ÁÈ¡ÊÓ²îÊý¾Ý
capture.retrieve( disparityMap, CV_CAP_OPENNI_DISPARITY_MAP_32F ); 
colorizeDisparity( disparityMap, colorDisparityMap, maxDisparity );
colorDisparityMap.copyTo( validColorDisparityMap, disparityMap != 0 ); 
imshow( "Colorized disparity map", validColorDisparityMap );
// ¶ÁÈ¡²ÊÉ«Í¼ÏñÊý¾Ý
capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
imshow( "Color image", bgrImage );
// ÀûÓÃÊÓ²îÊý¾Ý¼ÆËãÈýÎ¬µãÔÆ×ø±ê
cv::reprojectImageTo3D(disparityMap, xyzMap, matQ, true);

//////////////////////////////////////////////////////////////////////////
// OpenGLÏÔÊ¾
IplImage glXYZ = xyzMap;
IplImage glTexture = bgrImage;
load3dDataToGL(&glXYZ); // ÔØÈë»·¾³ÈýÎ¬µãÔÆ
loadTextureToGL(&glTexture); // ÔØÈëÎÆÀíÊý¾Ý
glutReshapeFunc (reshape); // ´°¿Ú±ä»¯Ê±ÖØ»æÍ¼Ïñ
glutDisplayFunc(renderScene); // ÏÔÊ¾ÈýÎ¬Í¼Ïñ 
glutMouseFunc(mouse); // Êó±ê°´¼üÏìÓ¦
glutMotionFunc(motion); // Êó±êÒÆ¶¯ÏìÓ¦
glutPostRedisplay(); // Ë¢ÐÂ»­Ãæ

//////////////////////////////////////////////////////////////////////////
// °´¼üÏûÏ¢ÏìÓ¦
int c = cvWaitKey(30);
if( c == 27 )
break;

// OpenCV ´¦Àí¼üÅÌÏìÓ¦ÏûÏ¢ºó£¬ÔÙÏÔÊ¾ OpenGL Í¼Ïñ
glutMainLoopEvent();
} // ÍË³öÑ­»·

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