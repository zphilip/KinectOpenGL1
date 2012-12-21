#ifndef _OgreKinect
#define _OgreKinect

#include"KinectDevice.h"
#include "Ogre.h"

using namespace Kinect;
using namespace Ogre;

class OgreKinect: public KinectDevice
{
public:
	OgreKinect();	//kinect init for openni
	virtual ~OgreKinect();
	virtual XnStatus initPrimeSensor();
	virtual bool Update(); 
	virtual void shutdown();

	//functions about handle the texture and buffer, not classify yet
	void ParseUserTexture(xn::SceneMetaData *sceneMetaData,	bool m_front);
	//create Ogre Texture
	void createMutliDynamicTexture();
	void createOgreUserTexture(const std::string,const std::string);
	void createOgreDepthTexture(const std::string,const std::string);
	void createOgreColorTexture(const std::string,const std::string);
	void createOgreColoredDepthTexture(const std::string,const std::string);

	//update the Ogre Texture
	bool UpdateTexture();

	//normal inline texture functions
	Ogre::TexturePtr getColorTexture()
	{
		return mColorTexture; 
	}
	
	Ogre::TexturePtr getDepthTexture()
	{
		return mDepthTexture; 
	}

	Ogre::TexturePtr getColoredDepthTexture()
	{
		return mColoredDepthTexture; 
	}

private:

	//User buffer&texture for Ogre
	Ogre::TexturePtr mUserTexture;
	Ogre::MaterialPtr mUserMaterial;
	Ogre::PixelBox   mUserPixelBox;
	bool             mUserTextureAvailable;

	//Color RGB buffer&texture for Ogre
	Ogre::TexturePtr mColorTexture;
	Ogre::MaterialPtr mColorMaterial;
	Ogre::PixelBox   mColorPixelBox;
	bool             mColorTextureAvailable;

	//Depth buffer&texture for Ogre
	Ogre::TexturePtr mDepthTexture;
	Ogre::MaterialPtr mDepthMaterial;
	Ogre::PixelBox   mDepthPixelBox;
	bool             mDepthTextureAvailable;

	//Color&depth RGB buffer&texture for Ogre
	Ogre::TexturePtr mColoredDepthTexture;
	Ogre::MaterialPtr mColoredDepthMaterial;	
	Ogre::PixelBox   mColoredDepthPixelBox;
	bool             mColoredDepthTextureAvailable;

	//Ogre::Vector3 DepthToWorld(int x, int y, int depthValue);
	//Ogre::Vector2 WorldToColor(const Ogre::Vector3 &pt);
};
#endif