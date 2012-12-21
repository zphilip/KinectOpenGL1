#pragma once
#include <cstdlib>
#include <gl/glut.h>
#include "defines.h"
#include "KHeadTrack.h"
#include "KGlutInput.h"
#include "kKinect.h"
#include "SampleManager.h"


class KProgram
{
	// This to be the main-helper-class and thus it should only be static, so no constructor
private:
	SampleManager* m_pSampleManage;
	//get head position from samplemanager
	KVertex getHeadPosition(void);

public:
	KProgram(SampleManager* pSampleManage);
	~KProgram(void);
	
	// Saves the window-handle of the main window
	static int mWindowHandle;
	// Sets up Glut for working
	static void initGlut(int argc, char* argv[]);
	static void glutDisplay(void);
	static void glutIdle(void);
	static void showWindow(void);
	static kKinect kinect;
	static KHeadTrack headtrack;

	static float x2;
	static float y2;
	static float z2;
};

