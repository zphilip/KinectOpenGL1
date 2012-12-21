#include "KProgram.h"
#include "SceneDrawer.h"

int KProgram::mWindowHandle = 0;
kKinect KProgram::kinect = kKinect();

float KProgram::x2=0.9f;
float KProgram::y2=0.57f;
float KProgram::z2=2.55f;

KProgram::KProgram(SampleManager* pSampleManage)
{
	m_pSampleManage = pSampleManage;
}

KProgram::~KProgram(void)
{
}

static GLfloat Xrot = 0, Yrot = 0, Zrot = 0;
static GLboolean Anim = GL_TRUE;

static void Display1( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glPushMatrix();
   glRotatef(Xrot, 1, 0, 0);
   glRotatef(Yrot, 0, 1, 0);
   glRotatef(Zrot, 0, 0, 1);

   glShadeModel(GL_FLAT);

   glBegin(GL_TRIANGLE_STRIP);
   glColor3f(1, 0, 0);
   glVertex2f(-1, -1);
   glVertex2f( 1, -1);
   glColor3f(1, 0, 0);
   glVertex2f( -1, 1);
   glColor3f(0, 0, 1);
   glVertex2f( 1, 1);
   glEnd();

   glPopMatrix();

   glutSwapBuffers();
}

void KProgram::initGlut(int argc, char* argv[]){
	
	// Initalize Glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	// Set window size and position
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutInitWindowPosition(WINDOW_POS_X,WINDOW_POS_Y);

	// Create the window and save the handle
	KProgram::mWindowHandle = glutCreateWindow(WINDOW_TITLE);

	// Setting the Clear-Color
	glClearColor(WINDOW_CLEAR_COLOR);

	// GL-ENABLES:
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_FOG);

	// Fog Configuration
	glFogi(GL_FOG_MODE, GL_LINEAR);

	// Setting the glut-funcs
	glutDisplayFunc(KProgram::glutDisplay);
	//glutDisplayFunc(Display1);
	glutIdleFunc(KProgram::glutIdle);	
	//glutIdleFunc(SceneDrawer::glutIdle);	
	glutMouseFunc(KGlutInput::glutMouse);
	glutKeyboardFunc(KGlutInput::glutKeyboard);
	glutMotionFunc(KGlutInput::glutMouseMotion);
}

KHeadTrack KProgram::headtrack(SCREEN_HEIGTH_MM);

void KProgram::glutDisplay(void)
{
	glutSetWindow(KProgram::mWindowHandle);
	// First delete the old scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Then paint the new scene
	headtrack.renderScene();

	// Done painting --> now show result
	glutSwapBuffers();
}

void KProgram::glutIdle(void)
{
	glutSetWindow(KProgram::mWindowHandle);
	//get head position 
	//SceneDrawer *singleton=SceneDrawer::GetInstance();
	//singleton->m_pUserTrackerObj->getHeadPostion(nUserId);
	//KVertex position = kinect.getPosition();
	//headtrack.refreshData(position.mX,position.mY,position.mZ);
    //headtrack.refreshData(x2,y2,z2);

	// Done idle --> now repaint the window
	glutPostRedisplay();
	//set screndrawer windows
	glutSetWindow(SceneDrawer::m_windowHandle);
    // Display the frame
    glutPostRedisplay();
}


void KProgram::showWindow(void){
	glutMainLoop();
}
