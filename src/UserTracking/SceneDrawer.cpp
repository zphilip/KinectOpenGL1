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
#define GL_WIN_SIZE_X 1300
#define GL_WIN_SIZE_Y 800
#define GL_WINDOW_POS_X 300
#define GL_WINDOW_POS_Y 0
// Maximum number of limbs (lines) we will draw as a skeleton.
#define MAX_LIMBS 16
//---------------------------------------------------------------------------
// Initialize static members
//---------------------------------------------------------------------------

SceneDrawer *SceneDrawer::m_pSingleton=NULL;
int SceneDrawer::m_windowHandle =0; 
int SceneDrawer::sub_windowHandle1 =0; 
int SceneDrawer::sub_windowHandle2 =0; 
int SceneDrawer::sub_windowHandle3 =0; 

// gap between subwindows
#define GAP  5
//  define the window position on screen
float main_window_x = GL_WINDOW_POS_X;
float main_window_y	= GL_WINDOW_POS_Y;

//  variables representing the window size
//float main_window_w = 256 + GAP * 2;
//float main_window_h = 256 + 64 + GAP * 3;
float main_window_w = GL_WIN_SIZE_X;
float main_window_h = GL_WIN_SIZE_Y;
//  define the window position on screen
float subwindow1_x = GAP;
float subwindow1_y = GAP;
float subwindow1_w = 640;
float subwindow1_h = 480;

float subwindow3_x = GAP + subwindow1_w + GAP;
float subwindow3_y = GAP;
float subwindow3_w = 640;
float subwindow3_h = 480;

float subwindow2_x = GAP;
float subwindow2_y = GAP + 480 + GAP;
float subwindow2_w = 1280;
float subwindow2_h = 300;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------


void SceneDrawer::DrawScene(UserTracker *pUserTrackerObj,int argc, char **argv,KinectAppManager *pSample, XnBool bShowLowConfidence)
{
    m_pUserTrackerObj=pUserTrackerObj;
    m_KinectApp =pSample;
    m_bShowLowConfidence=bShowLowConfidence;

#ifndef USE_GLES
	// Setup glut
	//KProgram::initGlut(argc, argv);
    glInit(&argc, argv);
#endif
	Depth3DMeshInit();
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

	//opengl texture
	memset(&g_texDepth,0, sizeof(g_texDepth));
	memset(&g_texImage,0, sizeof(g_texImage));
	memset(&g_texBackground,0, sizeof(g_texBackground));

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
    m_KinectApp->Cleanup();
    exit(exitCode);
}

void SceneDrawer::fixLocation(IntRect* pLocation, int xRes, int yRes)
{
	double resRatio = (double)xRes / yRes;

	double locationRatio = (pLocation->uRight - pLocation->uLeft) / (pLocation->uTop - pLocation->uBottom);

	if (locationRatio > resRatio) 
	{
		// location is wider. use height as reference.
		double width = (pLocation->uTop - pLocation->uBottom) * resRatio;
		pLocation->uRight = (pLocation->uLeft + width);
	}
	else if (locationRatio < resRatio)
	{
		// res is wider. use width as reference.
		double height = (pLocation->uRight - pLocation->uLeft) / resRatio;
		pLocation->uTop = (pLocation->uBottom + height);
	}
}

void SceneDrawer::InitTexture()
{
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
	m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);
	KinectDevice *m_Kinect = m_KinectApp->GetKinectDevice(0);    

    // initialize the texture for depth+user tracking

	// get the width and height of the texture as the nearest power of two larger than the
    // x/y resolution respectively.
    texWidth = 2;
    while(texWidth < g_nXRes) texWidth<<=1;
    texHeight = 2;
    while(texHeight < g_nYRes) texHeight<<=1;
	//simply the texture to using the same size
	texWidth = 640;
	texHeight =480;
	depthTexID = 0;
	glutSetWindow(sub_windowHandle3);
    glGenTextures(1,&depthTexID);
    pDepthTexBuf = new unsigned char[texWidth*texHeight*4];
    glBindTexture(GL_TEXTURE_2D,depthTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	// initialize the texture for 3D texture
	//glutSetWindow(sub_windowHandle1);
	//glGenTextures(1, &texture_rgb);
	//glBindTexture(GL_TEXTURE_2D, texture_rgb);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    memset(texcoords, 0, 8*sizeof(float));
    texcoords[0] = (float)g_nXRes/texWidth;
    texcoords[1] = (float)g_nYRes/texHeight;
    texcoords[2] = (float)g_nXRes/texWidth;
    texcoords[7] = (float)g_nYRes/texHeight;

	// initialize the texture for debugframe texture postion
	DepthLocation.uBottom = 0;
	DepthLocation.uTop = subwindow2_h - 1;
	DepthLocation.uLeft = 0;
	DepthLocation.uRight = subwindow2_w - 1;

	ImageLocation.uBottom = 0;
	ImageLocation.uTop = subwindow2_h - 1;
	ImageLocation.uLeft = 0;
	ImageLocation.uRight = subwindow2_w - 1;

	DepthLocation.uTop = subwindow2_h - 1;
	DepthLocation.uRight = subwindow2_w / 3 - 1;
	ImageLocation.uTop = subwindow2_h - 1;
	ImageLocation.uLeft = subwindow2_w / 3;

	glutSetWindow(SceneDrawer::sub_windowHandle2);
	int nXres = m_Kinect->getDepthMetaData()->FullXRes();
	TextureMapInit(&g_texDepth, m_Kinect->getColoredDepthBuffer(), m_Kinect->getDepthMetaData()->FullXRes(),
															m_Kinect->getDepthMetaData()->FullYRes(), 
															3, 
															m_Kinect->getDepthMetaData()->XRes(),
															m_Kinect->getDepthMetaData()->XRes());
	fixLocation(&DepthLocation, m_Kinect->getDepthMetaData()->FullXRes(), m_Kinect->getDepthMetaData()->FullYRes());
	TextureMapInit(&g_texImage, m_Kinect->getColorBuffer(), m_Kinect->getImageMetaData()->FullXRes(),
															m_Kinect->getImageMetaData()->FullYRes(), 
															3, 
															m_Kinect->getImageMetaData()->XRes(),
															m_Kinect->getImageMetaData()->XRes());
	fixLocation(&ImageLocation, m_Kinect->getImageMetaData()->FullXRes(), m_Kinect->getImageMetaData()->FullYRes());
}

void SceneDrawer::subwindow2_display (void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
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

    if(singleton->m_pUserTrackerObj->GetExitPoseState(0)>=1.0f)
    {
        singleton->ExitSample(EXIT_SUCCESS);
    }
    // Process the data
    singleton->DrawDepthMapTexture();
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
int fx = 0;
int fy = 0;
int cx = 0;
int cy = 0;
int ox = 0;
int oy = 0;
bool lp = false;
int maxdepth=-1;

void SceneDrawer::subwindow1_display (void)
{
	glPushMatrix();
	SceneDrawer *singleton=GetInstance();
    //if(singleton->g_bPause==FALSE)
    //   singleton->m_pUserTrackerObj->UpdateFrame();
	//singleton->Draw3DDepthMapTexture1();
	singleton->Draw3DMesh();
	glPopMatrix();
	glutSwapBuffers();
}

void SceneDrawer::subwindow1_mouse_motion(int x, int y) {
	if(lp) {
		cx = x-fx+ox;
		cy = fy-y+oy;
	}
}

void SceneDrawer::subwindow1_mouse(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON) {
		if(state==GLUT_DOWN) {
			lp = true;
			fx = x;
			fy = y;
		}
		else {
			lp = false;
			ox = cx;
			oy = cy;
		}
	}
}

void SceneDrawer::drawDebugFrame()
{
	SceneDrawer *singleton=GetInstance();
    //if(singleton->g_bPause==FALSE)
    //    singleton->m_pUserTrackerObj->UpdateFrame();
	// calculate locations
	/*
	KinectDevice *m_Kinect = singleton->m_KinectApp->GetKinectDevice(0);
	glutSetWindow(SceneDrawer::sub_windowHandle2);
	singleton->TextureMapInit(&singleton->g_texDepth, m_Kinect->getColoredDepthBuffer(), m_Kinect->getDepthMetaData()->FullXRes(),
															m_Kinect->getDepthMetaData()->FullYRes(), 
															3, 
															m_Kinect->getDepthMetaData()->XRes(),
															m_Kinect->getDepthMetaData()->XRes());
	singleton->fixLocation(&DepthLocation, m_Kinect->getDepthMetaData()->FullXRes(), m_Kinect->getDepthMetaData()->FullYRes());
	singleton->TextureMapInit(&singleton->g_texImage, m_Kinect->getColorBuffer(), m_Kinect->getImageMetaData()->FullXRes(),
															m_Kinect->getImageMetaData()->FullYRes(), 
															3, 
															m_Kinect->getImageMetaData()->XRes(),
															m_Kinect->getImageMetaData()->XRes());
	singleton->fixLocation(&ImageLocation, m_Kinect->getImageMetaData()->FullXRes(), m_Kinect->getImageMetaData()->FullYRes());
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the opengl env for fixed location view
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,subwindow2_w,subwindow2_h,0,-1.0,1.0);
	glDisable(GL_DEPTH_TEST); 

	singleton->TextureMapUpdate(&singleton->g_texImage);
	singleton->TextureMapDraw(&singleton->g_texImage, &singleton->ImageLocation);
	singleton->TextureMapUpdate(&singleton->g_texDepth);
	singleton->TextureMapDraw(&singleton->g_texDepth, &singleton->DepthLocation);

	glutSwapBuffers();
}

void SceneDrawer::TextureMapInit(XnTextureMap* pTex, unsigned char* pBuffer, int nSizeX, int nSizeY, unsigned int nBytesPerPixel, int nCurX, int nCurY)
{
	// check if something changed
	if (pTex->bInitialized && pTex->OrigSize.X == nSizeX && pTex->OrigSize.Y == nSizeY)
	{
		if (pTex->CurSize.X != nCurX || pTex->CurSize.Y != nCurY)
		{
			// update
			pTex->CurSize.X = nCurX;
			pTex->CurSize.Y = nCurY;
			return;
		}
	}

	// update it all
	pTex->OrigSize.X = nSizeX;
	pTex->OrigSize.Y = nSizeY;
	//pTex->Size.X = GetPowerOfTwo(nSizeX);
	//pTex->Size.Y = GetPowerOfTwo(nSizeY);
	pTex->Size.X = nSizeX;
	pTex->Size.Y = nSizeY;
	pTex->nBytesPerPixel = nBytesPerPixel;
	pTex->CurSize.X = nCurX;
	pTex->CurSize.Y = nCurY;
	pTex->pMap = pBuffer;
	
	if (!pTex->bInitialized)
	{
		glGenTextures(1, &pTex->nID);
		glBindTexture(GL_TEXTURE_2D, pTex->nID);

		switch (pTex->nBytesPerPixel)
		{
		case 3:
			pTex->nFormat = GL_RGB;
			break;
		case 4:
			pTex->nFormat = GL_RGBA;
			break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		pTex->bInitialized = TRUE;
	}
}

void SceneDrawer::TextureMapUpdate(XnTextureMap* pTex)
{
	// set current texture object
	glBindTexture(GL_TEXTURE_2D, pTex->nID);

	// set the current image to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pTex->Size.X, pTex->Size.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, pTex->pMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, pTex->pMap);
}


void SceneDrawer::TextureMapDraw(XnTextureMap* pTex, IntRect* pLocation)
{
	// set current texture object
	glBindTexture(GL_TEXTURE_2D, pTex->nID);

	// turn on texture mapping
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set drawing mode to rectangles
	glBegin(GL_QUADS);

	// set the color of the polygon
	glColor4f(1, 1, 1, 1);

	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(pLocation->uLeft, pLocation->uBottom);
	// upper right
	glTexCoord2f((float)pTex->OrigSize.X/(float)pTex->Size.X, 0);
	glVertex2f(pLocation->uRight, pLocation->uBottom);
	// bottom right
	glTexCoord2f((float)pTex->OrigSize.X/(float)pTex->Size.X, (float)pTex->OrigSize.Y/(float)pTex->Size.Y);
	glVertex2f(pLocation->uRight, pLocation->uTop);
	// bottom left
	glTexCoord2f(0, (float)pTex->OrigSize.Y/(float)pTex->Size.Y);
	glVertex2f(pLocation->uLeft, pLocation->uTop);

	glEnd();

	// turn off texture mapping
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
}

void SceneDrawer::Draw3DDepthMapTexture() 
{
	xn::DepthGenerator *depth = NULL;
	xn::ImageGenerator *image = NULL;
	xn::DepthMetaData pDepthMapMD;
	xn::ImageMetaData pImageMapMD;
	KinectDevice *m_Kinect = m_KinectApp->GetKinectDevice(0);    
	depth = m_Kinect->getDepthGenerator();
	image = m_Kinect->getImageGenerator();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	
	//rot_angle+=0.7;

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


void SceneDrawer::Draw3DDepthMapTexture1() 
{
	xn::DepthGenerator *depth = NULL;
	xn::ImageGenerator *image = NULL;
	KinectDevice *m_Kinect = m_KinectApp->GetKinectDevice(0);    
	xn::DepthMetaData *pDepthMapMD = m_Kinect->getDepthMetaData();
	xn::ImageMetaData *pImageMapMD = m_Kinect->getImageMetaData();
	unsigned short *tmpGrayPixels = (unsigned short *)pDepthMapMD->Data();

	depth = m_Kinect->getDepthGenerator();
	image = m_Kinect->getImageGenerator();
	const unsigned int xres = pDepthMapMD->XRes();
	const unsigned int yres = pDepthMapMD->YRes();

	if(maxdepth==-1)
		maxdepth = depth->GetDeviceMaxDepth();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	//glMatrixMode( GL_PROJECTION );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	//glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
	//glPushMatrix();
	glLoadIdentity();
	//glEnable( GL_LIGHT0 );  
	//glTranslatef(0.0f,0.0f,-4.0f);//move forward 4 units
	glColor3f(0.0f,0.0f,1.0f); //blue color
	glPointSize(10.0f);//set point size to 10 pixels
	//-------------------------------------------------
	glBegin(GL_POINTS);
	glVertex3f(1.0f,1.0f,0.0f);//upper-right corner
    glVertex3f(-1.0f,-1.0f,0.0f);//lower-left corner
	/*
	for(unsigned int y=0; y<yres; y++) {
		for(unsigned int x=0; x<xres; x++) {
				//glTexCoord2f(static_cast<float>(x)/static_cast<float>(640), static_cast<float>(y)/static_cast<float>(480));
				int offset = x+y*yres;
				// Convert kinect data to world xyz coordinate
				unsigned short rawDepth = tmpGrayPixels[offset];
				Vector3f v = m_Kinect->DepthToWorld(x,y,rawDepth);
				float rx = v.x();
				float ry = v.y();
				float rz = v.z();
				//glVertex3f(rx, (yres-ry), rz);
				glVertex3f(x, (yres-y), rawDepth);
		}
	}*/
	DepthmapPointCloud *m_PointCloud = m_Kinect->getDepthPointCloud();
	XnPoint3D * m_PointCloudData = m_PointCloud->getPointCloudData();
	for (unsigned int i=0; i<m_PointCloud->getPointCloudNum();i++)
	{
		glTexCoord2f(static_cast<float>(m_PointCloudData[i].X)/static_cast<float>(640), static_cast<float>(m_PointCloudData[i].Y)/static_cast<float>(480));
		glVertex3f(m_PointCloudData[i].X, (yres-m_PointCloudData[i].Y), m_PointCloudData[i].Z);
	}
	glEnd();
	//glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

// 画像の横幅（単位はOpenGLにおける長さ）
#define PLANE_SIZE 5.0

// ポリゴンが繋がっていないと判断する閾値（単位はOpenGLにおける長さ）
#define SPLIT_THRESHOLD		0.5

#define MAX_DEPTH 10000
float g_pDepthHist[MAX_DEPTH];
XnRGB24Pixel* g_pTexMap = NULL;
unsigned int g_nTexMapX = 0;
unsigned int g_nTexMapY = 0;

float* g_pVertices3f = NULL;			// 頂点バッファ
float* g_pNormals3f = NULL;				// 法線バッファ

// ↓分割数。これを小さくすると表示が荒く、大きくすると細かくなる
unsigned int g_nPolyX = 160; //640;		// 横方向の格子数
unsigned int g_nPolyY = 120; //480;		// 縦方向の格子数

float gRotateMat[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };	// カメラ回転行列
float gPanVec[]      = { 0.0, 0.0, 0.0 };						// カメラ平行移動量

void SceneDrawer::Depth3DMeshInit(void)
{
	KinectDevice *m_Kinect = m_KinectApp->GetKinectDevice(0);    
	xn::DepthMetaData *g_depthMD = m_Kinect->getDepthMetaData();
	xn::ImageMetaData *g_imageMD = m_Kinect->getImageMetaData();
	
	// デプスマップをカメラ画像に位置合わせ
	m_Kinect->getDepthGenerator()->GetAlternativeViewPointCap().SetViewPoint(*m_Kinect->getImageGenerator());
	// Texture map init
	g_nTexMapX = (((unsigned short)(g_depthMD->FullXRes()-1) / 512) + 1) * 512;
	g_nTexMapY = (((unsigned short)(g_depthMD->FullYRes()-1) / 512) + 1) * 512;
	g_pTexMap = (XnRGB24Pixel*)malloc(g_nTexMapX * g_nTexMapY * sizeof(XnRGB24Pixel));

	// 頂点バッファ確保
	g_pVertices3f = (float*)calloc(g_nPolyX * g_nPolyY * 3, sizeof(float));

	return;
}
void SceneDrawer::Depth3DMeshReshape(int width, int height)
{
	static const double kFovY = 40;
	int tx=0, ty=0;
    double nearDist, farDist, aspect;

	//GLUI_Master.get_viewport_area(&tx, &ty, &width, &height);
    glViewport(tx, ty, width, height);

    // Compute the viewing parameters based on a fixed fov and viewing
    // a canonical box centered at the origin.
    nearDist = 1.0 / tan((kFovY / 2.0) * K_PI / 180.0);
    farDist = nearDist + 100.0;
    aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    // Place the camera down the Z axis looking at the origin.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(0, 0, nearDist + 5.0,
              0, 0, 0,
              0, 1, 0);
}

//---------------------------------------------------------------------------
// 描画処理

// OpenGLの描画処理をここに書く。
//   gluing.h から呼ばれます
void SceneDrawer::Draw3DMesh (void)
{
	//gluPerspective(45, subwindow1_w/subwindow1_h, 1000, 5000);
	glTranslatef(gPanVec[0] / 50.0, gPanVec[1] / 50.0, -gPanVec[2] / 10.0);		// Is "trans->set_spped()" not working?
	glMultMatrixf(gRotateMat);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	KinectDevice *m_Kinect = m_KinectApp->GetKinectDevice(0);    
	xn::DepthMetaData *g_depthMD = m_Kinect->getDepthMetaData();
	xn::ImageMetaData *g_imageMD = m_Kinect->getImageMetaData();

	const XnDepthPixel* pDepth = g_depthMD->Data();
	const XnUInt8* pImage = g_imageMD->Data();

	// Calculate the accumulative histogram
	xnOSMemSet(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));

	unsigned int nNumberOfPoints = 0;
	for (XnUInt y = 0; y < g_depthMD->YRes(); ++y)
	{
		for (XnUInt x = 0; x < g_depthMD->XRes(); ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				g_pDepthHist[*pDepth]++;
				nNumberOfPoints++;
			}
		}
	}
	for (int nIndex=1; nIndex<MAX_DEPTH; nIndex++)
	{
		g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<MAX_DEPTH; nIndex++)
		{
			g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
		}
	}

	xnOSMemSet(g_pTexMap, 0, g_nTexMapX*g_nTexMapY*sizeof(XnRGB24Pixel));

	// check if we need to draw image frame to texture
	const XnRGB24Pixel* pImageRow = g_imageMD->RGB24Data();
	XnRGB24Pixel* pTexRow = g_pTexMap + g_imageMD->YOffset() * g_nTexMapX;

	for (XnUInt y = 0; y < g_imageMD->YRes(); ++y)
	{
		const XnRGB24Pixel* pImage = pImageRow;
		XnRGB24Pixel* pTex = pTexRow + g_imageMD->XOffset();

		for (XnUInt x = 0; x < g_imageMD->XRes(); ++x, ++pImage, ++pTex)
		{
			*pTex = *pImage;
		}

		pImageRow += g_imageMD->XRes();
		pTexRow += g_nTexMapX;
	}

	// 頂点座標初期化
	for (unsigned int y = 0; y < g_nPolyY; ++y)
	{
		for (unsigned int x = 0; x < g_nPolyX; ++x)
		{
			for (int i = 0; i < 3; ++i)
			{
				g_pVertices3f[(y * g_nPolyX + x) * 3 + i] = 0;
			}
		}
	}

	// 対応するポリゴン毎の深度の平均を求める。頂点バッファを一時的に作業領域に使っている
	int px, py;
	const XnDepthPixel* pDepthRow = g_depthMD->Data();
	//XnRGB24Pixel* 
	pTexRow = g_pTexMap + g_depthMD->YOffset() * g_nTexMapX;
	int nXRes = g_depthMD->FullXRes();
	int nYRes = g_depthMD->FullYRes();

	for (XnUInt y = 0; y < nYRes; ++y)
	{
		py = (y + g_depthMD->YOffset()) * g_nPolyY / nYRes;

		const XnDepthPixel* pDepth = pDepthRow;
		XnRGB24Pixel* pTex = pTexRow + g_depthMD->XOffset();

		for (XnUInt x = 0; x < nXRes; ++x, ++pDepth, ++pTex)
		{
			px = (x + g_depthMD->XOffset()) * g_nPolyX / nXRes;

			if (*pDepth != 0)	// 深度0となっている画素は平均値の計算から除く
			{
				int nHistValue = g_pDepthHist[*pDepth];

				// Z値の集計
				g_pVertices3f[(py * g_nPolyX + px) * 3 + 2] +=  (float)nHistValue;	// 頂点ベクトルの第3要素に入れておく（ホントは値が違うけど）

				// 面積の集計
				g_pVertices3f[(py * g_nPolyX + px) * 3] += 1.0;		// 頂点ベクトルの第1要素に入れておく（ホントは用途が違うけど）
			}
		}

		pDepthRow += g_depthMD->XRes();
		pTexRow += g_nTexMapX;
	}

	// 頂点座標を計算
	float dx = (float)PLANE_SIZE / (float)g_nPolyX;
	float dy = (float)PLANE_SIZE * (float)nYRes / (float)nXRes / (float)g_nPolyY;
	float xoffset = -(float)(PLANE_SIZE / 2);
	float yoffset =  (float)(PLANE_SIZE / 2) * (float)nYRes / (float)nXRes;
	float zoffset = -(float)(PLANE_SIZE / 2);
	for (unsigned int y = 0; y < g_nPolyY; ++y)
	{
		for (unsigned int x = 0; x < g_nPolyX; ++x)
		{
			int index = (y * g_nPolyX + x) * 3;

			if (g_pVertices3f[index] > 0.0) {
				// 奥行き計測面積ゼロでなければZ座標を計算
				g_pVertices3f[index + 2] /= (g_pVertices3f[index] * (float)PLANE_SIZE);
			} else {
				// 奥行き計測面積ゼロなら奥行きゼロとする
				g_pVertices3f[index + 2] = 0.0;
			}

			g_pVertices3f[index] = dx * (float)x + xoffset;			// X座標
			g_pVertices3f[index + 1] = dy * -(float)y + yoffset;	// Y座標
			g_pVertices3f[index + 2] = (g_pVertices3f[index + 2] / 5.0) + zoffset;	// Z座標。5.0で割っているのは適当な凹凸に調節するため。
		}
	}
	
	// Create the OpenGL texture map
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_nTexMapX, g_nTexMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, g_pTexMap);

	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	// Enable texture
	glEnable(GL_TEXTURE_2D);

	// 凹凸を付けて描画
	glBegin(GL_TRIANGLES);
	float texWidth = (float)nXRes / (float)g_nTexMapX;
	float texHeight = (float)nYRes / (float)g_nTexMapY;
	for (unsigned int y = 0; y < g_nPolyY - 1; ++y)
	{
		for (unsigned int x = 0; x < g_nPolyX - 1; ++x)
		{
			// 対応するテクスチャの座標
			float tx0 = ((float)x) / (float)(g_nPolyX - 1) * texWidth;
			float ty0 = ((float)y) / (float)(g_nPolyY - 1) * texHeight;
			float tx1 = ((float)x + 1) / (float)(g_nPolyX - 1) * texWidth;
			float ty1 = ((float)y + 1) / (float)(g_nPolyY - 1) * texHeight;

			// ポリゴンの頂点
			float* pUL = g_pVertices3f + ((y * g_nPolyX + x) * 3);
			float* pUR = pUL + 3;
			float* pBR = pUL + (g_nPolyX * 3) + 3;
			float* pBL = pUL + (g_nPolyX * 3);

			// 法線を求める
			float norm[3];

			// 閾値以上離れているメッシュの処理
			if (abs(pUR[2] - pUL[2]) > SPLIT_THRESHOLD || abs(pBR[2] - pBL[2]) > SPLIT_THRESHOLD
				|| abs(pBL[2] - pUL[2]) > SPLIT_THRESHOLD || abs(pBR[2] - pUR[2]) > SPLIT_THRESHOLD) {
// ちょっとトリッキーな書き方だけど、#ifの値で表示方法の選択ができる
// ↓ 1…離れたポリゴンは切り離す，0…切り離さず色を変える
#if 1
					continue;					// 描画しない。つまり隣のポリゴンと切り離す
#else
					glColor3f(0.0, 1.0, 0.0);	// 切り離さず緑にする
			} else {
					glColor3f(1.0, 1.0, 1.0);	// 通常はテクスチャの色になるよう、白とする
#endif
			}

			// 四角形の格子を書くには三角形が２つ要る

			// 三角形その１
			normal(pUL, pUR, pBR, norm);
			glNormal3fv(norm);
			// upper left
			glTexCoord2f(tx0, ty0);
			glVertex3fv(pUL);
			// upper right
			glTexCoord2f(tx1, ty0);
			glVertex3fv(pUR);
			// bottom right
			glTexCoord2f(tx1, ty1);
			glVertex3fv(pBR);

			// 三角形その２
			normal(pBL, pUL, pBR, norm);
			glNormal3fv(norm);
			// bottom left
			glTexCoord2f(tx0, ty1);
			glVertex3fv(pBL);
			// upper right
			glTexCoord2f(tx0, ty0);
			glVertex3fv(pUL);
			// bottom right
			glTexCoord2f(tx1, ty1);
			glVertex3fv(pBR);
		}
	}

	glEnd();
}

#ifndef USE_GLES
void SceneDrawer::glutIdle (void)
{
	SceneDrawer *singleton=GetInstance();
	if(singleton->g_bPause==FALSE)
		singleton->m_KinectApp->GetKinectDevice(0)->Update();
	//glutSetWindow(KProgram::mWindowHandle);
    //glutPostRedisplay();
	glutSetWindow(SceneDrawer::sub_windowHandle1);
    glutPostRedisplay();
	glutSetWindow(SceneDrawer::sub_windowHandle2);
	glutPostRedisplay();
	glutSetWindow(SceneDrawer::sub_windowHandle3);
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

// Set up general OpenGL rendering properties: lights, depth buffering, etc.
void SceneDrawer::Depth3DMeshinitGL()
{
	// Lighting
    static const GLfloat light_model_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};

    static const GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.9f, 0.9f};
    //static const GLfloat light0_direction[] = {-0.2f, -0.4f, 0.5f, 0.0f};
    static const GLfloat light0_direction[] = {0.408f, 0.408f, -0.816f, 0.0f};

    static const GLfloat light1_diffuse[] = {0.1f, 0.1f, 0.1f, 0.1f};
    //static const GLfloat light1_direction[] = {-0.2f, 0.4f, 0.2f, 0.0f};
    static const GLfloat light1_direction[] = {0.0f, 0.0f, -1.0f, 0.0f};
    
    // Enable depth buffering for hidden surface removal.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    // Setup other misc features.
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // Setup lighting model.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_direction);
    glEnable(GL_LIGHT1);

	// Background color
	glClearColor(0.3, 0.5, 0.7, 1.0);
}
void SceneDrawer::Depth3DMapTextureinitGL()
{
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);
	glClearColor(0, 0, 0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
}
void SceneDrawer::glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutInitWindowPosition(WINDOW_POS_X - GL_WIN_SIZE_X,WINDOW_POS_Y);
    m_windowHandle = glutCreateWindow ("User Selection Sample");
	glutReshapeFunc(SceneDrawer::main_reshape);
	glutDisplayFunc(SceneDrawer::main_display);
	
	/**** Subwindow 1 *****/
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	sub_windowHandle1 = glutCreateSubWindow (m_windowHandle, subwindow1_x, subwindow1_y, subwindow1_w, subwindow1_h);
	Depth3DMeshinitGL();
	glutDisplayFunc(SceneDrawer::subwindow1_display);
	//glutReshapeFunc(SceneDrawer::subwindow1_reshape);
	glutReshapeFunc(SceneDrawer::Depth3DMeshReshape);		
    glutKeyboardFunc(SceneDrawer::glutKeyboard);
    glutIdleFunc(SceneDrawer::glutIdle);
	glutMouseFunc(SceneDrawer::subwindow1_mouse);
	glutMotionFunc(SceneDrawer::subwindow1_mouse_motion);

	/**** Subwindow 3*****/
	sub_windowHandle3 = glutCreateSubWindow (m_windowHandle, subwindow3_x, subwindow3_y, subwindow3_w, subwindow3_h);
	glutDisplayFunc(SceneDrawer::subwindow2_display);
	//glutReshapeFunc(SceneDrawer::subwindow3_reshape);
    glutKeyboardFunc(SceneDrawer::glutKeyboard);
    glutIdleFunc(SceneDrawer::glutIdle);


	/**** Subwindow 2 *****/
    sub_windowHandle2 = glutCreateSubWindow (m_windowHandle, subwindow2_x, subwindow2_y, subwindow2_w, subwindow2_h);
	glutDisplayFunc (SceneDrawer::drawDebugFrame);
	//glutReshapeFunc(SceneDrawer::subwindow2_reshape);
	glutKeyboardFunc (SceneDrawer::glutKeyboard);
	glutIdleFunc(SceneDrawer::glutIdle);
}
//-------------------------------------------------------------------------
//  Main Window Display Function.
//
//	Set the Window Background color to black.
//-------------------------------------------------------------------------
void SceneDrawer::main_display (void)
{
	//  Notify that we are displaying the main window
	printf ("Drawing Main Window contents...\n");

	//  Set background color to black
    glClearColor(1.0, 0.5, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

	//  Swap front and back buffers
    glutSwapBuffers();
}

//-------------------------------------------------------------------------
//  Main Window Reshape Function.
//
//	Reset the position and dimensions of subwindows when main window size
//	changes.
//-------------------------------------------------------------------------
void SceneDrawer::main_reshape (int width, int height) 
{
	//  Notify that we are reshaping the main window
	printf ("Main Window: ");
	
	//  Just take the case when the user tries
	//  to make the size of the window very small...
	if (width < GAP * 4 || height < GAP * 6)
	{
		glutSetWindow (m_windowHandle);
		glutReshapeWindow (main_window_w, main_window_h);
		return;
	}
	
	//  Change the subwindow 1 dimensions as window dimensions change
	//  main_window_w          ---> subwindow1_w
	//  main_window_w' (width) ---> ??
	//  ==> 
	subwindow1_w = (subwindow1_w * (width-GAP*2.0))/(main_window_w-GAP*2.0);
	subwindow1_h = (subwindow1_h * (height-GAP*3.0))/(main_window_h-GAP*3.0);

	//  Set subwindow 1 as current window and then reposition and resize it
    glutSetWindow (sub_windowHandle1);
    glutPositionWindow (GAP, GAP);
    glutReshapeWindow (subwindow1_w, subwindow1_h);
	
	//  Change the subwindow 2 dimensions as window dimensions change
	subwindow2_w = (subwindow2_w * (width-GAP*2.0))/(main_window_w-GAP*2.0);
	subwindow2_h = (subwindow2_h * (height-GAP*3.0))/(main_window_h-GAP*3.0);

	//  Set subwindow 2 as current window and then reposition and resize it
    glutSetWindow (sub_windowHandle2);
    glutPositionWindow (GAP, GAP+subwindow1_h+GAP);
    glutReshapeWindow (subwindow2_w, subwindow2_h);

		//  Change the subwindow 2 dimensions as window dimensions change
	subwindow3_w = (subwindow3_w * (width-GAP*2.0))/(main_window_w-GAP*2.0);
	subwindow3_h = (subwindow3_h * (height-GAP*3.0))/(main_window_h-GAP*3.0);

	//  Set subwindow 2 as current window and then reposition and resize it
    glutSetWindow (sub_windowHandle3);
    glutPositionWindow (GAP+subwindow3_w+GAP, GAP);
    glutReshapeWindow (subwindow3_w, subwindow3_h);

	//  Stay updated with the window width and height
	main_window_w = width;
	main_window_h = height;

	//  Print current width and height on the screen
	printf ("Width: %d, Height: %d.\n", width, height);
}
//-------------------------------------------------------------------------
//  SubWindow 1 Reshape Function.
//
//	Preserve aspect ratio of viewport when subwindow is resized.
//-------------------------------------------------------------------------
void SceneDrawer::subwindow1_reshape (int width, int height) 
{
	//  Represents a side of the viewport. A viewport is intended to
	//  to take a square shape so that the aspect ratio is reserved
	int viewport_side = 0;

	//  Viewport x and y positions (Center viewport)
	int viewport_x = 0, viewport_y = 0;
	
	//  Calculate viewport side
	viewport_side = (width > height) ? height : width;

	//  Calculate viewport position
	viewport_x = (width - viewport_side) / 2;
	viewport_y = (height - viewport_side) / 2;

	//  Preserve aspect ratio
	glViewport (viewport_x, viewport_y, viewport_side, viewport_side);

	//  Set subwindow width and height
	subwindow1_w = width;
	subwindow1_h = height;

	//  Notify that we are reshaping subwindow 1
	printf ("Subwindow 1: ");

	//  Print current width and height
	printf ("Width: %d, Height: %d, Viewport Side: %d.\n", width, height, viewport_side);
}

//-------------------------------------------------------------------------
//  SubWindow 2 Reshape Function.
//
//	Preserve aspect ratio of viewport when subwindow is resized.
//-------------------------------------------------------------------------
void SceneDrawer::subwindow2_reshape (int width, int height) 
{
	//  Represents a side of the viewport. A viewport is intended to
	//  to take a square shape so that the aspect ratio is reserved
	int viewport_side = 0;

	//  Viewport x and y positions (Center viewport)
	int viewport_x = 0, viewport_y = 0;
	
	//  Calculate viewport side
	viewport_side = (width > height) ? height : width;

	//  Calculate viewport position
	viewport_x = (width - viewport_side) / 2;
	viewport_y = (height - viewport_side) / 2;

	//  Preserve aspect ratio
	glViewport (viewport_x, viewport_y, viewport_side, viewport_side);

	//  Set subwindow width and height
	subwindow2_w = width;
	subwindow2_h = height;

	//  Notify that we are reshaping subwindow 1
	printf ("Subwindow 2: ");

	//  Print current width and height
	printf ("Width: %d, Height: %d, Viewport Side: %d.\n", width, height, viewport_side);
}

//-------------------------------------------------------------------------
//  SubWindow 2 Reshape Function.
//
//	Preserve aspect ratio of viewport when subwindow is resized.
//-------------------------------------------------------------------------
void SceneDrawer::subwindow3_reshape (int width, int height) 
{
	//  Represents a side of the viewport. A viewport is intended to
	//  to take a square shape so that the aspect ratio is reserved
	int viewport_side = 0;

	//  Viewport x and y positions (Center viewport)
	int viewport_x = 0, viewport_y = 0;
	
	//  Calculate viewport side
	viewport_side = (width > height) ? height : width;

	//  Calculate viewport position
	viewport_x = (width - viewport_side) / 2;
	viewport_y = (height - viewport_side) / 2;

	//  Preserve aspect ratio
	glViewport (viewport_x, viewport_y, viewport_side, viewport_side);

	//  Set subwindow width and height
	subwindow3_w = width;
	subwindow3_h = height;

	//  Notify that we are reshaping subwindow 1
	printf ("Subwindow 3: ");

	//  Print current width and height
	printf ("Width: %d, Height: %d, Viewport Side: %d.\n", width, height, viewport_side);
}
#endif // USE_GLES

