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
#ifndef XNV_SCENE_DRAWER_H_
#define XNV_SCENE_DRAWER_H_

#include "UserTracker.h"
#include "KinectAppManager.h"
#include "XnPlatform.h"

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
	#include <GL/glut.h>
#endif
#else
#include "opengles.h"
#endif

typedef struct
{
	int X;
	int Y;
} IntPair;

typedef struct  
{
	int uBottom;
	int uLeft;
	int uTop;
	int uRight;
} IntRect;

typedef struct XnTextureMap
{
	IntPair Size;
	IntPair OrigSize;
	unsigned char* pMap;
	unsigned int nBytesPerPixel;
	GLuint nID;
	GLenum nFormat;
	bool bInitialized;
	IntPair CurSize;
} XnTextureMap;

/// @brief Class to draw the scene for the sample
/// 
/// The goal of this class is to contain everything needed to draw the scene (image, skeleton
/// etc.), separating the graphics implementation elements (e.g. OpenGL) from the the OpenNI elements.
/// This should enable the user to concentrate on the UserTracker class which contains the elements
/// relevant to OpenNI and ignore the graphics.
/// @note This class can be ignored by anyone who wishes to learn only the OpenNI elements.
/// 
/// @note This class is implemented based on the singleton pattern i.e. we can only have one drawer!
/// @ingroup UserSelectionGraphics
class SceneDrawer
{
public:
    /// @brief Start running the scene
    /// 
    /// This method starts running the scene. It will not return until the program exits!
    /// @param pUserTrackerObj A pointer to the user tracker object. This is where the scene drawer
    /// takes the information to draw and provides feedback from keyboard. @note The CleanUp method
    /// will be called on the user tracker when the program exits from inside the DrawScene method!.
    /// @param argc Number of command line arguments (same as @ref main()).
    /// @param argv The command line arguments (same as @ref main()).
    /// @param pSample The sample manager to use
    /// @param bShowLowConfidence When this is true, low confidence limbs will be shown as dotted or 
    ///        dashed lines instead of disappearing
    void DrawScene(UserTracker *pUserTrackerObj,int argc, char **argv,KinectAppManager *pSample, XnBool bShowLowConfidence);

    /// @brief gets the singleton instance.
    static SceneDrawer *GetInstance();
	static int m_windowHandle;
	static int sub_windowHandle1;
	static int sub_windowHandle2;
	static int sub_windowHandle3;
private:
    static SceneDrawer *m_pSingleton; ///< The singleton instance
    /// Private constructor just to make sure we can't create an object. Only GetInstance creates a 
    /// new object.
    SceneDrawer(); 

    /// @brief Draws the texture which shows the depth and users in the scene.
    void DrawDepthMapTexture();
	void Draw3DDepthMapTexture();
	void Draw3DDepthMapTexture1();
	void Draw3DMesh (void);
    /// @brief Method to draw the labels (nUserId and state) for each user in the scene
    /// 
    /// @param nUserId The user whose labels we will draw. 
    void DrawLabels(XnUserID nUserId);

    /// @brief Method to draw the skeleton for each user in the scene
    /// 
    /// @param nUserId The user whose skeleton we will draw. 
    void DrawSkeleton(XnUserID nUserId);

    /// @brief Method to exit from the DrawScene method (and the program as a whole)
    /// 
    /// @param exitCode The exit code we will exit with.
    /// 
    /// @note this will call the CleanUp method on the m_pUserTrackerObj member!
    void ExitSample(int exitCode);

    /// @brief Does all initialization required for texture drawing.
    void InitTexture();
	void TextureMapUpdate(XnTextureMap* pTex);
	void TextureMapDraw(XnTextureMap* pTex, IntRect* pLocation);
	void SceneDrawer::TextureMapInit(XnTextureMap* pTex, unsigned char* pBuffer, int nSizeX, int nSizeY, unsigned int nBytesPerPixel, int nCurX, int nCurY);
	void Depth3DMeshInit(void);
	static void Depth3DMeshReshape(int width, int height);

	void Depth3DMapTextureinitGL();
	void Depth3DMeshinitGL();
	static void TestinitGL();
	static void Testdisplay();
	static void Testreshape(GLsizei width, GLsizei height);
	static void glPCLView();
	static void glPCLReSizeGLScene(int Width, int Height);
	void glPCLInitGL(int Width, int Height);
	float RawDepthToMeters(int depthValue);
    /// @brief An internal callback which is called from inside DrawScene main loop to do the drawing.
    ///
    /// This method does the single frame update including both updating the user tracker and calling
    /// the various internal methods to draw the texture, labels and skeleton.
    /// @note this is a callback for glutMainLoop if using glut (rather than GLES) and called all 
    /// the time in the while loop if we are using GLES.
    static void subwindow2_display (void);
	static void subwindow1_display (void);
	static void drawDebugFrame();
	static void main_display (void);
	static void main_reshape (int width, int height);
	static void subwindow1_reshape (int width, int height);
	static void subwindow2_reshape (int width, int height);
	static void subwindow3_reshape (int width, int height);
	static void subwindow1_mouse_motion(int x, int y) ;
	static void subwindow1_mouse(int button, int state, int x, int y);
	static void glPCLMousePress(int button, int state, int x, int y);
	static void glPCLMouseMoved(int x, int y);

#ifndef USE_GLES
    /// @brief An internal callback which is called from inside DrawScene main loop for background 
    /// frames.
    /// 
    /// This callback is called by glutMainLoop whenever the program is in the background and no drawing
    /// is required.
    static void glutIdle (void);

    /// @brief An internal callback which is called from inside DrawScene main loop whenever there is
    /// a keyboard event.
    /// 
    /// This callback is called by glutMainLoop whenever a keyboard event occurs when the program 
    /// window is in focus. 
    /// @param key The key pressed
    /// @param x The x position of the mouse when the key was pressed in windows relative coordinates
    /// @param y The y position of the mouse when the key was pressed in windows relative coordinates
    static void glutKeyboard (unsigned char key, int x, int y);

    /// @brief Internal method to perform initialization for OpenGL when using glut (rather than GLES)
    /// 
    /// @param pargc Number of command line arguments (same as @ref main()).
    /// @param pargv The command line arguments (same as @ref main()).
    void glInit (int * pargc, char ** pargv);
	//---------------------------------------------------------------------------
	// ベクトル・行列演算関数

	// ベクトルの長さを返す
	float length(const float* vec)
	{
		return sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	}

	// ベクトルの外積を計算
	void cross(const float* vec0, const float* vec1, float* dest)
	{
		dest[0] = vec0[1] * vec1[2] - vec0[2] * vec1[1];
		dest[1] = vec0[2] * vec1[0] - vec0[0] * vec1[2];
		dest[2] = vec0[0] * vec1[1] - vec0[1] * vec1[0];
	}

	// 三角形ポリゴンの法線ベクトルを計算
	void normal(const float* vec0, const float* vec1, const float* vec2, float* norm)
	{
		float r10[3], r20[3], crs[3];
		for (int i = 0; i < 3; ++i)
		{
			r10[i] = vec0[i] - vec1[i];
			r20[i] = vec0[i] - vec2[i];
		}
		cross(r10, r20, crs);

		float len = length(crs);
		if (len == 0.0) {
			norm[0] = 0;
			norm[1] = 0;
			norm[2] = 1;
		} else {
			norm[0] = crs[0] / len;
			norm[1] = crs[1] / len;
			norm[2] = crs[2] / len;
		}
	}

	// Do the projection from u,v,depth to X,Y,Z directly in an opengl matrix
	// These numbers come from a combination of the ros kinect_node wiki, and
	// nicolas burrus' posts.
	void LoadVertexMatrix()
	{
		float fx = 594.21f;
		float fy = 591.04f;
		float a = -0.0030711f;
		float b = 3.3309495f;
		float cx = 339.5f;
		float cy = 242.7f;
		GLfloat mat[16] = {
			1/fx,     0,  0, 0,
			0,    -1/fy,  0, 0,
			0,       0,  0, a,
			-cx/fx, cy/fy, -1, b
		};
		glMultMatrixf(mat);
	}

	// This matrix comes from a combination of nicolas burrus's calibration post
	// and some python code I haven't documented yet.
	void LoadRGBMatrix()
	{
		float mat[16] = {
			5.34866271e+02,   3.89654806e+00,   0.00000000e+00,   1.74704200e-02,
			-4.70724694e+00,  -5.28843603e+02,   0.00000000e+00,  -1.22753400e-02,
			-3.19670762e+02,  -2.60999685e+02,   0.00000000e+00,  -9.99772000e-01,
			-6.98445586e+00,   3.31139785e+00,   0.00000000e+00,   1.09167360e-02
		};
		glMultMatrixf(mat);
	}

	int getMaxDepth(const XnDepthPixel* depthmap, int size=640*480) {
		int max_tmp=0;
		for(int i=0; i<size; i++) {
			if(depthmap[i]>max_tmp)
				max_tmp=depthmap[i];
		}
	
		return max_tmp;
	};

	int GetPowerOfTwo(int num)
	{
		int result = 1;

		while (result < num)
			result <<= 1;

		return result;
	};

	void fixLocation(IntRect* pLocation, int xRes, int yRes);

#endif // USE_GLES

	// members
    /// @brief A pointer to the user tracker object. This is where the scene drawer
    /// takes the information to draw and provides feedback from keyboard.
    UserTracker *m_pUserTrackerObj; 


    /// @name TextureInitializationMembers
    /// members used for initializing the texture
    /// @{
	XnTextureMap g_texDepth;
	XnTextureMap g_texImage;
	XnTextureMap g_texBackground;
	GLuint depthTexID;
    unsigned char* pDepthTexBuf;
	
	IntRect DepthLocation;
	IntRect ImageLocation;
	int texWidth, texHeight;
    GLfloat texcoords[8];
    /// @}
	GLubyte aDepthMap[640*480];
	GLuint texture_rgb, texture_depth;
	/* Texture maps for depth and image */

    XnPoint3D* pLimbsPosArr; ///< @brief used to store the limbs information
    XnFloat * pConfidenceArr; ///< @brief used to store the confidence of the limbs information


    /// @name PrintMemebers
    /// members used for deciding what to draw
    /// @{
  
    /// @brief True when we want to draw the background image
    XnBool g_bDrawBackground;
    /// @brief True when we want to draw any texture
    XnBool g_bDrawPixels;
    /// @brief True when we want to draw the skeleton
    XnBool g_bDrawSkeleton;
    /// @brief True when we want to draw any labels on users
    XnBool g_bPrintID;
    /// @brief True when we want to draw full labels 
    ///     
    /// If false and g_bPrintID is true we will only draw the user id.
    XnBool g_bPrintState;

    XnBool g_bPause; ///< @brief True when pausing (i.e. UserTracker not updated).

    /// @brief When this is true, low confidence limbs will be shown as dotted or 
    ///        dashed lines instead of disappearing
    XnBool m_bShowLowConfidence; 

    KinectAppManager *m_KinectApp;
    /// @}
};

#endif // XNV_SCENE_DRAWER_H_
