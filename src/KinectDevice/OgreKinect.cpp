#include "OgreKinect.h"

OgreKinect::OgreKinect() : KinectDevice()
{
	mColorTexture.setNull();
	mDepthTexture.setNull();
	mColoredDepthTexture.setNull();
	mColorTextureAvailable = false;
	mDepthTextureAvailable = false;
	mColoredDepthTextureAvailable = false;

	mColoredDepthPixelBox = Ogre::PixelBox(KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT, 1, Ogre::PF_R8G8B8, mColoredDepthBuffer);
	mDepthPixelBox = Ogre::PixelBox(KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT, 1, Ogre::PF_L8, mDepthBuffer);
	mColorPixelBox = Ogre::PixelBox(KINECT_COLOR_WIDTH, KINECT_COLOR_HEIGHT, 1, Ogre::PF_B8G8R8, mColorBuffer);
}

OgreKinect::~OgreKinect()
{
	shutdown();
}

//init Kinect or Xtion
XnStatus OgreKinect::initPrimeSensor()
{
	return KinectDevice::initPrimeSensor();

}

//update the all buffer and texture from kinect
bool OgreKinect::Update()
{
	if (!KinectDevice::Update())
		return false;
	ParseUserTexture(&sceneMetaData, true);
	return UpdateTexture();
}

bool OgreKinect::UpdateTexture()
{
	bool updated = false;

	if (!mColorTexture.isNull() && mColorTextureAvailable)
	{
		Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mColorTexture->getBuffer();
		pixelBuffer->blitFromMemory(mColorPixelBox);
		mColorTextureAvailable = false;
		updated = true;
	}
	if (!mDepthTexture.isNull() && mDepthTextureAvailable)
	{
		Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mDepthTexture->getBuffer();
		pixelBuffer->blitFromMemory(mDepthPixelBox);
		mDepthTextureAvailable = false;
		updated = true;
	}
	if (!mColoredDepthTexture.isNull()&& mColoredDepthTextureAvailable)
	{
		Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mColoredDepthTexture->getBuffer();
		pixelBuffer->blitFromMemory(mColoredDepthPixelBox);		
		mColoredDepthTextureAvailable = false;
		updated = true;
	}

	return updated;
}
//Parse sceneMetaData into UserTexture
void OgreKinect::ParseUserTexture(xn::SceneMetaData *sceneMetaData, bool m_front)
{
#if SHOW_DEPTH || SHOW_BAR
	//TexturePtr texture = TextureManager::getSingleton().getByName("MyDepthTexture");
	//TexturePtr texture = TextureManager::getSingleton().getByName("MyDepthTexture2");
	if(mUserTexture.isNull())
		return;
	// Get the pixel buffer
	HardwarePixelBufferSharedPtr pixelBuffer = mUserTexture->getBuffer();
	// Lock the pixel buffer and get a pixel box
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD); 
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	unsigned char* pDest = static_cast<unsigned char*>(pixelBox.data);

	// Get label map 
	const XnLabel* pUsersLBLs = sceneMetaData->Data();
		
	for (size_t j = 0; j < KINECT_DEPTH_HEIGHT; j++)
	{
		pDest = static_cast<unsigned char*>(pixelBox.data) + j*pixelBox.rowPitch*4;
#if SHOW_DEPTH
		for(size_t i = 0; i < KINECT_DEPTH_WIDTH; i++)
#elif SHOW_BAR
		for(size_t i = 0; i < 50; i++)
#endif
		{
			// fix i if we are mirrored
			uint fixed_i = i;
			if(!m_front)
			{
				fixed_i = KINECT_DEPTH_WIDTH - i;
			}

			// determine color
#if SHOW_DEPTH
			unsigned int color = GetColorForUser(pUsersLBLs[j*KINECT_DEPTH_WIDTH + fixed_i]);

			// if we have a candidate, filter out the rest
			if (m_candidateID != 0)
			{
				if  (m_candidateID == pUsersLBLs[j*KINECT_DEPTH_WIDTH + fixed_i])
				{
					color = GetColorForUser(1);
					if( j > KINECT_DEPTH_HEIGHT*(1 - m_pStartPoseDetector->GetDetectionPercent()))
					{
						//highlight user
						color |= 0xFF070707;
					}
					if( j < KINECT_DEPTH_HEIGHT*(m_pEndPoseDetector->GetDetectionPercent()))
					{	
						//hide user
						color &= 0x20F0F0F0;
					}
				}
				else
				{
					color = 0;
				}
			}
#elif SHOW_BAR
			// RED. kinda.
			unsigned int color = 0x80FF0000;
			if( j > KINECT_DEPTH_HEIGHT*(1 - m_pStartPoseDetector->GetDetectionPercent()))
			{
				//highlight user
				color |= 0xFF070707;
			}
			if( j < KINECT_DEPTH_HEIGHT*(m_pEndPoseDetector->GetDetectionPercent()))
			{	
				//hide user
				color &= 0x20F0F0F0;
			}

			if ((m_pStartPoseDetector->GetDetectionPercent() == 1) ||
				(m_pEndPoseDetector->GetDetectionPercent() == 1))
			{
				color = 0;
			}
#endif
				
			// write to output buffer
			*((unsigned int*)pDest) = color;
			pDest+=4;
		}
	}
	// Unlock the pixel buffer
	pixelBuffer->unlock();
#endif // SHOW_DEPTH
}


//create multi screen with dynamic texture
void OgreKinect::createMutliDynamicTexture()
{
	// create the kinect depth image material
	const std::string userTextureName        = "KinectUserTexture";
	const std::string colorTextureName        = "KinectColorTexture";
	const std::string depthTextureName        = "KinectDepthTexture";
	const std::string coloredDepthTextureName = "KinectColoredDepthTexture";
	
	createOgreUserTexture(userTextureName, "KinectUserMatrial");
	createOgreColorTexture(colorTextureName,"KinectColorMaterial");
	createOgreDepthTexture(depthTextureName, "KinectDepthMaterial");
	createOgreColoredDepthTexture(coloredDepthTextureName, "KinectColoredDepthMaterial");
}

//create User texture
void OgreKinect::createOgreUserTexture(const std::string UserTextureName, const std::string materialName)
{
	if(!UserTextureName.empty())
	{		
		mUserTexture  = TextureManager::getSingleton().createManual(
			UserTextureName, // name
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,      // type
			KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT,// width & height
			0,                // number of mipmaps
			PF_BYTE_BGRA,     // pixel format
			TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}
	if(!materialName.empty())
	{
		// Create a material using the texture
		Ogre::MaterialPtr material = MaterialManager::getSingleton().create(
				materialName, // name
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		material->getTechnique(0)->getPass(0)->createTextureUnitState(UserTextureName);
		material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureRotate(Ogre::Degree(180)); 
		//material->getTechnique(0)->getPass(0)->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	}
}
//create Depth texture
void OgreKinect::createOgreDepthTexture(const std::string depthTextureName,const std::string materialName)
{
	if(!depthTextureName.empty())
	{
		mDepthTexture = TextureManager::getSingleton().createManual(
			depthTextureName, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			TEX_TYPE_2D, 
			KINECT_DEPTH_WIDTH, 
			KINECT_DEPTH_HEIGHT, 
			0,
			PF_L8, 
			TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}
	if(!materialName.empty())
	{
		//Create Material
		Ogre::MaterialPtr material = MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
		material->getTechnique(0)->getPass(0)->setAlphaRejectSettings(CMPF_GREATER, 127);
		material->getTechnique(0)->getPass(0)->createTextureUnitState(depthTextureName);
		//material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureRotate(Ogre::Degree(180)); 
		//material->getTechnique(0)->getPass(0)->setVertexProgram("Ogre/Compositor/StdQuad_vp");
		//material->getTechnique(0)->getPass(0)->setFragmentProgram("KinectDepth");
	}
}
//create Color texture
void OgreKinect::createOgreColorTexture(const std::string colorTextureName, const std::string materialName)
{
	if(!colorTextureName.empty())
	{
		mColorTexture = TextureManager::getSingleton().createManual(
			colorTextureName, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			TEX_TYPE_2D, 
			KINECT_DEPTH_WIDTH, 
			KINECT_DEPTH_HEIGHT, 
			0,
			PF_R8G8B8, 
			TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}
	if(!materialName.empty())
	{
		//Create Material
		Ogre::MaterialPtr material = MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
		material->getTechnique(0)->getPass(0)->createTextureUnitState(colorTextureName);
		material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureRotate(Ogre::Degree(180)); 
	}
}
//create ColorDepth texture
void OgreKinect::createOgreColoredDepthTexture(const std::string coloredDepthTextureName,const std::string materialName)
{
	if(!coloredDepthTextureName.empty())
	{
		mColoredDepthTexture = TextureManager::getSingleton().createManual(
		coloredDepthTextureName, 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		TEX_TYPE_2D, 
		KINECT_DEPTH_WIDTH, 
		KINECT_DEPTH_HEIGHT, 
		0,
		PF_R8G8B8, 
		TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
		//mColoredDepthBuffer   = new unsigned char[Ogre::Kinect::depthWidth * Ogre::Kinect::depthHeight * 3];
		//mColoredDepthPixelBox = Ogre::PixelBox(Ogre::Kinect::depthWidth, Ogre::Kinect::depthHeight, 1, Ogre::PF_R8G8B8, mColoredDepthBuffer);
	}
	if(!materialName.empty())
	{
		//Create Material
		Ogre::MaterialPtr material = MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
		material->getTechnique(0)->getPass(0)->createTextureUnitState(coloredDepthTextureName);
		//material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureRotate(Ogre::Degree(180)); 
	}
}

void OgreKinect::shutdown()
{
	mColorTexture.setNull();
	mDepthTexture.setNull();
	mColoredDepthTexture.setNull();
	KinectDevice::shutdown();
}


