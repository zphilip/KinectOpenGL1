#pragma once
#include <XnCppWrapper.h>
#include <iostream>
#include <string>
#include "KVertex.h"
#include "defines.h"
#include "UserTracker.h"

class kKinect
{
public:
	kKinect();
	~kKinect(void);
	KVertex getPosition(void);
private:
	xn::Context mContext;
	xn::DepthGenerator mDepth;
	xn::ImageGenerator mImage;
	xn::UserGenerator user;
	//the UserGenerator from sample manager
    xn::UserGenerator * m_pUserGenerator;
	UserTracker *m_pUserTracker; ///< @brief The user tracker used when running
	xn::SkeletonCapability* skeleton;
	XnUserID pUser[1];
	XnUInt16 nUsers;

	void initKinect(void);
	bool checkError(std::string message, XnStatus RetVal);
public:
	void reset(void);
	void calibrateUser();
};

