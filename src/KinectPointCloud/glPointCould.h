#ifndef _glPointCloud
#define _glPointCloud

#include <GL/gl.h>
#include <GL/glut.h>

static GLuint gl_rgb_tex;
static int mx=-1,my=-1;        // Prevous mouse coordinates
static int rotangles[2] = {0}; // Panning angles
static float zoom = 8;         // zoom factor
static int color = 0;          // Use the RGB texture or just draw it as color

class glPointCloud	
{
public:

	glPointCloud(){};
	~glPointCloud(){}
	// Do the projection from u,v,depth to X,Y,Z directly in an opengl matrix
	// These numbers come from a combination of the ros kinect_node wiki, and
	// nicolas burrus' posts.
	static void LoadVertexMatrix()
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
	static void LoadRGBMatrix()
	{
		float mat[16] = {
			5.34866271e+02,   3.89654806e+00,   0.00000000e+00,   1.74704200e-02,
			-4.70724694e+00,  -5.28843603e+02,   0.00000000e+00,  -1.22753400e-02,
			-3.19670762e+02,  -2.60999685e+02,   0.00000000e+00,  -9.99772000e-01,
			-6.98445586e+00,   3.31139785e+00,   0.00000000e+00,   1.09167360e-02
		};
		glMultMatrixf(mat);
	}

	static void glPCLInitGL(int Width, int Height)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glGenTextures(1, &gl_rgb_tex);
		glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPCLReSizeGLScene(Width, Height);
	}
	static void glPCLMouseMoved(int x, int y)
	{
		if (mx>=0 && my>=0) {
			rotangles[0] += y-my;
			rotangles[1] += x-mx;
		}
		mx = x;
		my = y;
	}
	static void glPCLMousePress(int button, int state, int x, int y)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			mx = x;
			my = y;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			mx = -1;
			my = -1;
		}
	}
	static void glPCLDraw(unsigned short *tmpDepthPixels,unsigned short * tmpImagePixels )
	{
		short *depth = 0;
		char *rgb = 0;
		//SceneDrawer *singleton=GetInstance();
		//KinectDevice *m_Kinect = singleton->m_KinectApp->GetKinectDevice(0);    
		//xn::DepthMetaData *pDepthMapMD = m_Kinect->getDepthMetaData();
		//xn::ImageMetaData *pImageMapMD = m_Kinect->getImageMetaData();
		//unsigned short *tmpGrayPixels = (unsigned short *)pDepthMapMD->Data();

		static unsigned int indices[480][640];
		static short xyz[480][640][3];
		int i,j;
		for (i = 0; i < 480; i++) {
			for (j = 0; j < 640; j++) {
				xyz[i][j][0] = j;
				xyz[i][j][1] = i;
				//xyz[i][j][2] = singleton->RawDepthToMeters(tmpGrayPixels[i*640+j]);
				xyz[i][j][2] = ( tmpDepthPixels[i*640+j] & 0xFFF8 ) >>3;
				if (i==j)
					printf("World depth is: %i \n", tmpDepthPixels[i*640+j] );
				indices[i][j] = i*640+j;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glPushMatrix();
		glScalef(zoom,zoom,1);
		glTranslatef(0,0,-3.5);
		glRotatef(rotangles[0], 1,0,0);
		glRotatef(rotangles[1], 0,1,0);
		glTranslatef(0,0,1.5);

		LoadVertexMatrix();

		// Set the projection from the XYZ to the texture image
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1/640.0f,1/480.0f,1);
		LoadRGBMatrix();
		LoadVertexMatrix();
		glMatrixMode(GL_MODELVIEW);

		glPointSize(1);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_SHORT, 0, xyz);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_SHORT, 0, xyz);

		if (0)
			glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpImagePixels);

		glPointSize(2.0f);
		glDrawElements(GL_POINTS, 640*480, GL_UNSIGNED_INT, indices);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glutSwapBuffers();

	}
	static void glPCLReSizeGLScene(int Width, int Height)
	{
		glViewport(0,0,Width,Height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, 4/3., 0.3, 200);
		glMatrixMode(GL_MODELVIEW);
	}

	static void glKeyPressed(unsigned char key, int x, int y)
	{
		if (key == 'w')
			zoom *= 1.1f;
		if (key == 's')
			zoom /= 1.1f;
		if (key == 'c')
			color = !color;
	}

};
#endif