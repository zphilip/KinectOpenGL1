/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "SceneDrawer.h"
#include "KProgram.h"
//---------------------------------------------------------------------------
// definitions
//---------------------------------------------------------------------------

#ifdef USE_GLES
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
#endif

// window size (X,Y) of the OpenGL portion
#define GL_WIN_SIZE_X 480
#define GL_WIN_SIZE_Y 360

// Maximum number of limbs (lines) we will draw as a skeleton.
#define MAX_LIMBS 16

//---------------------------------------------------------------------------
// Initialize static members
//---------------------------------------------------------------------------

SceneDrawer *SceneDrawer::m_pSingleton=NULL;
int SceneDrawer::m_windowHandle =0; 

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------


void SceneDrawer::DrawScene(UserTracker *pUserTrackerObj,int argc, char **argv,SampleManager *pSample, XnBool bShowLowConfidence)
{
    m_pUserTrackerObj=pUserTrackerObj;
    m_pSample=pSample;
    m_bShowLowConfidence=bShowLowConfidence;

#ifndef USE_GLES
	// Setup glut
	KProgram::initGlut(argc, argv);
    glInit(&argc, argv);
#endif
    InitTexture();
#ifndef USE_GLES
    glutMainLoop();
#else
    if (!opengles_init(GL_WIN_SIZE_X, GL_WIN_SIZE_Y, &display, &surface, &context))
    {
        printf("Error initializing opengles\n");
        ExitSample(EXIT_FAILURE);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    while (1)
    {
        glutDisplay();
        eglSwapBuffers(display, surface);
    }
    // we should never reach here! we have a while(1) above!
    ExitSample(EXIT_SUCCESS); 

#endif
}


SceneDrawer *SceneDrawer::GetInstance()
{
    if(m_pSingleton==NULL)
        m_pSingleton=new SceneDrawer();
    return m_pSingleton;
}

SceneDrawer::SceneDrawer()
{
    // drawing defaults
    g_bDrawBackground = TRUE;
    g_bDrawPixels = TRUE;
    g_bDrawSkeleton = TRUE;
    g_bPrintID = TRUE;
    g_bPrintState = TRUE;
    g_bPause=FALSE;

    // buffer initialization
    pLimbsPosArr=XN_NEW_ARR(XnPoint3D,(MAX_LIMBS*2));
    pConfidenceArr=XN_NEW_ARR(XnFloat,MAX_LIMBS);

    // following are dummy assignments which will be overriden when DrawScene is called
    // (either in DrawScene itself or in InitTexture
    m_pUserTrackerObj=NULL; 
    depthTexID=0;
    pDepthTexBuf=NULL;
    texWidth=0;
    texHeight=0;
    for(int i=0; i<8; i++)
        texcoords[i] = 0;

}



void SceneDrawer::DrawDepthMapTexture()
{
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
    m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);

    if (g_bDrawPixels)
    {
        m_pUserTrackerObj->FillTexture(pDepthTexBuf,texWidth,texHeight,g_bDrawBackground);
    }
    else
    {
        xnOSMemSet(pDepthTexBuf, 0, 3*2*g_nXRes*g_nYRes); // makes the texture empty.
    }

    // makes sure we draw the relevant texture
    glBindTexture(GL_TEXTURE_2D, depthTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pDepthTexBuf);

    // Display the OpenGL texture map
    glColor4f(0.75,0.75,0.75,1);

    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    GLfloat verts[8] = { g_nXRes, g_nYRes, g_nXRes, 0, 0, 0, 0, g_nYRes };
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //TODO: Maybe glFinish needed here instead - if there's some bad graphics crap
    glFlush();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
}

#define MAX_USER_LABEL_LEN 50
#define MAX_ID_LABEL_LEN 120
void SceneDrawer::DrawLabels(XnUserID nUserId)
{
#ifndef USE_GLES
    char strUserLabel[MAX_USER_LABEL_LEN] = "";
    char strOutputLabel[MAX_ID_LABEL_LEN] = "";
    XnFloat color[3];

    XnPoint3D com;
    xnOSMemSet(strOutputLabel, 0, sizeof(strOutputLabel));
    XnStatus res=m_pUserTrackerObj->GetUserColor(nUserId,color);
    XnFloat userExitPose=m_pUserTrackerObj->GetExitPoseState(nUserId)*100.0f;
    if(res!=XN_STATUS_OK)
        return; // bad user!
    xnOSMemSet(strUserLabel, 0, sizeof(strUserLabel));
    res=m_pUserTrackerObj->GetUserStringPos(nUserId,com,strUserLabel,MAX_USER_LABEL_LEN-1);
    if(res!=XN_STATUS_OK)
        return; // bad user!

    if (!g_bPrintState)
    {
        if(userExitPose>0)
        {
            sprintf(strOutputLabel, "%d - Exit wait %3.0f%% done.", nUserId,userExitPose);
        }
        else
        {
            sprintf(strOutputLabel, "%d", nUserId);
        }
        
    }
    else 
    {
        if(userExitPose>0)
        {
            sprintf(strOutputLabel, "%d - %s - Exit wait %3.0f%% done.", nUserId,strUserLabel,userExitPose);
        }
        else
        {
            sprintf(strOutputLabel, "%d - %s", nUserId,strUserLabel);
        }
        
    }

    glColor4f(1-color[0], 1-color[1], 1-color[2], 1);

    glRasterPos2i(com.X, com.Y);
    int len = (int)strlen(strOutputLabel);

    for(int c=0; c<len; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,strOutputLabel[c]);
    }
#endif
}


#define drawOneLine(x1,y1,x2,y2)  \
    glVertex2f ((x1),(y1)); glVertex2f ((x2),(y2)); ;

void SceneDrawer::DrawSkeleton(XnUserID nUserId)
{
    XnFloat color[3];

    XnUInt16 numLimbs=MAX_LIMBS;
    if(m_pUserTrackerObj->GetLimbs(nUserId,pLimbsPosArr,pConfidenceArr,numLimbs)!=XN_STATUS_OK)
        return; // no limbs to draw
    if(numLimbs==0)
        return; // no limbs to draw
    XnStatus res=m_pUserTrackerObj->GetUserColor(nUserId,color);
    if(res!=XN_STATUS_OK)
        return; // bad user!

    glColor4f(1-color[0], 1-color[1], 1-color[2], 1);
    for(XnUInt16 j=0; j<numLimbs; j++)
    {
#ifndef USE_GLES
        if(pConfidenceArr[j]<=0.5)
        {
            if(m_bShowLowConfidence==FALSE)
            {
                continue; // we simply do not show this limb...
            }
            glEnable(GL_LINE_STIPPLE);
            if(pConfidenceArr[j]==0.5)
            {
                glLineStipple(1,0x0101);
            }
            else
            {
                glLineStipple(1,0x00FF);
            }            
        }
        glBegin(GL_LINES);
        glVertex2f(pLimbsPosArr[j*2].X, pLimbsPosArr[j*2].Y);
        glVertex2f(pLimbsPosArr[(j*2)+1].X, pLimbsPosArr[(j*2)+1].Y);
        glEnd();
        if(pConfidenceArr[j]<=0.5)
        {
            glDisable(GL_LINE_STIPPLE);
        }
#else
        GLfloat verts[4] = {pLimbsPosArr[j*2].X, pLimbsPosArr[j*2].Y, pLimbsPosArr[(j*2)+1].X, pLimbsPosArr[(j*2)+1].Y};
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glDrawArrays(GL_LINES, 0, 2);
        glFlush();
#endif
    }
#ifndef USE_GLES
   // glEnd();
#endif
}

void SceneDrawer::ExitSample(int exitCode)
{
#if USE_GLES
    opengles_shutdown(display, surface, context);
#endif
    m_pSample->Cleanup();
    exit(exitCode);
}



void SceneDrawer::InitTexture()
{
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
    m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);

    // get the width and height of the texture as the nearest power of two larger than the
    // x/y resolution respectively.
    texWidth = 2;
    while(texWidth < g_nXRes) texWidth<<=1;
    texHeight = 2;
    while(texHeight < g_nYRes) texHeight<<=1;

    // initialize the texture
    depthTexID = 0;
    glGenTextures(1,&depthTexID);
    pDepthTexBuf = new unsigned char[texWidth*texHeight*4];
    glBindTexture(GL_TEXTURE_2D,depthTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    memset(texcoords, 0, 8*sizeof(float));
    texcoords[0] = (float)g_nXRes/texWidth;
    texcoords[1] = (float)g_nYRes/texHeight;
    texcoords[2] = (float)g_nXRes/texWidth;
    texcoords[7] = (float)g_nYRes/texHeight;
}

void SceneDrawer::glutDisplay (void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup the OpenGL viewpoint
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
    SceneDrawer *singleton=GetInstance();

    singleton->m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);

#ifndef USE_GLES
    glOrtho(0, g_nXRes, g_nYRes, 0, -1.0, 1.0);
#else
    glOrthof(0, g_nXRes, g_nYRes, 0, -1.0, 1.0);
#endif

    glDisable(GL_TEXTURE_2D);

    if(singleton->g_bPause==FALSE)
        singleton->m_pUserTrackerObj->UpdateFrame();
    if(singleton->m_pUserTrackerObj->GetExitPoseState(0)>=1.0f)
    {
        singleton->ExitSample(EXIT_SUCCESS);
    }
    // Process the data
    //singleton->DrawDepthMapTexture();
	singleton->Draw3DDepthMapTexture();
    XnUserID aUsers[15];
    XnUInt16 nUsers = 15;
    xn::UserGenerator *pUserGenerator=singleton->m_pUserTrackerObj->GetUserGenerator();
    pUserGenerator->GetUsers(aUsers, nUsers);
    for (int i = 0; i < nUsers; ++i)
    {
        if (singleton->g_bPrintID)
        {
            singleton->DrawLabels(aUsers[i]);
        }
        if (singleton->g_bDrawSkeleton)
        {
            if(pUserGenerator->GetSkeletonCap().IsTracking(aUsers[i]))
            {
                singleton->DrawSkeleton(aUsers[i]);
				KVertex position = singleton->m_pUserTrackerObj->getHeadPosition(aUsers[i]);
				KProgram::headtrack.refreshData(position.mX,position.mY,position.mZ);
				KProgram::headtrack.refreshData(KProgram::x2,KProgram::y2,KProgram::z2);
            }
            
        }

    }

#ifndef USE_GLES
    glutSwapBuffers();
#endif
}

float rot_angle=0;
float scalex=0.9, scaley=0.9, transx=41, transy=35;
GLubyte aDepthMap[640*480];
GLuint texture_rgb, texture_depth;
int fx = 0;
int fy = 0;
int cx = 0;
int cy = 0;
int ox = 0;
int oy = 0;
bool lp = false;
int maxdepth=-1;

void SceneDrawer::Draw3DDepthMapTexture() 
{
	xn::DepthGenerator *depth;
	xn::ImageGenerator *image;
	xn::DepthMetaData pDepthMapMD;
	xn::ImageMetaData pImageMapMD;
	depth = m_pUserTrackerObj->GetDepthGenerator();
	image = m_pUserTrackerObj->GetImageGenerator();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45, WINDOW_SIZE_X/WINDOW_SIZE_Y, 1000, 5000);
	//glOrtho(0, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, -128, 128);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
//	glTranslatef(-12.8/640.0, 9.0/480.0, 0);
//	glTranslatef(-12.8/630.0, 9.0/480.0,0);
	glScalef(scalex, scaley, 1.0);
	glTranslatef(transx/630, transy/480, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	rot_angle+=0.7;

	// Aktuelle Depth Metadaten auslesen
	depth->GetMetaData(pDepthMapMD);
	// Aktuelle Depthmap auslesen
	const XnDepthPixel* pDepthMap = depth->GetDepthMap();

	if(maxdepth==-1)
		maxdepth = getMaxDepth(pDepthMap);

	// Aktuelle Image Metadaten auslesen 
	image->GetMetaData(pImageMapMD);
	//Aktuelles Bild auslesen
	const XnRGB24Pixel* pImageMap = image->GetRGB24ImageMap();

	glColor3f(1, 1, 1);
//	XnDepthPixel maxdepth = depth.GetDeviceMaxDepth();
	const unsigned int xres = pDepthMapMD.XRes();
	const unsigned int yres = pDepthMapMD.YRes();

	for(unsigned int y=0; y<yres-1; y++) {
		for(unsigned int x=0; x<xres; x++) {
			aDepthMap[x+y*xres] = static_cast<GLubyte>(static_cast<float>(pDepthMap[x+y*xres])/static_cast<float>(maxdepth)*255);
		}
	}

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-100, -100, -2000);
	glRotatef(cx,0,1,0);
	glRotatef(cy,1,0,0);
	glTranslatef(-320, -240, 1000);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_rgb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageMap);
	glBegin(GL_POINTS);
	for(unsigned int y=0; y<yres-1; y++) {
		for(unsigned int x=0; x<630; x++) {
			if(pDepthMap[x+y*xres]!=0) {
				glTexCoord2f(static_cast<float>(x)/static_cast<float>(630), static_cast<float>(y)/static_cast<float>(480)); 
				glVertex3f(x, (yres-y), -pDepthMap[x+y*xres]/2.00);
#ifdef DEBUGOUT
				datei << t_gamma[pDepthMap[x+y*xres]] << endl;
#endif
			}
		}
	}
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

#ifndef USE_GLES
void SceneDrawer::glutIdle (void)
{
	glutSetWindow(SceneDrawer::m_windowHandle);
    // Display the frame
    glutPostRedisplay();
	glutSetWindow(KProgram::mWindowHandle);
    // Display the frame
    glutPostRedisplay();

}



void SceneDrawer::glutKeyboard (unsigned char key, int /*x*/, int /*y*/)
{
    SceneDrawer *singleton=GetInstance();
    switch (key)
    {
    case 27:
        singleton->ExitSample(EXIT_SUCCESS); 
    case 'b':
        // Draw background?
        singleton->g_bDrawBackground = !singleton->g_bDrawBackground;
        break;
    case 'x':
        // Draw pixels at all?
        singleton->g_bDrawPixels = !singleton->g_bDrawPixels;
        break;
    case 's':
        // Draw Skeleton?
        singleton->g_bDrawSkeleton = !singleton->g_bDrawSkeleton;
        break;
    case 'i':
        // Print label?
        singleton->g_bPrintID = !singleton->g_bPrintID;
        break;
    case 'l':
        // Print ID & state as label, or only ID?
        singleton->g_bPrintState = !singleton->g_bPrintState;
        break;
    case'p':
        singleton->g_bPause=!singleton->g_bPause;
        break;
    }
}

void SceneDrawer::glInit (int * pargc, char ** argv)
{
	//glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutInitWindowPosition(WINDOW_POS_X - GL_WIN_SIZE_X,WINDOW_POS_Y);
    m_windowHandle = glutCreateWindow ("User Selection Sample");
    //glutFullScreen();
    glutSetCursor(GLUT_CURSOR_NONE);

    glutDisplayFunc(SceneDrawer::glutDisplay);
    glutKeyboardFunc(SceneDrawer::glutKeyboard);
    glutIdleFunc(SceneDrawer::glutIdle);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

#endif // USE_GLES

