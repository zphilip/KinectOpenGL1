#ifndef _KinectPCL
#define _KinectPCL

#undef max
#undef min
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/io/openni_grabber.h>
#include <XnCppWrapper.h>
#include <stdio.h>

using namespace std;
using namespace pcl;
using namespace xn;
/** 
* use  1.create object 2.set output mode 3.initilization 4.update and processing 
* ex.. 1.DepthmapPointCloud cloud; 2.cloud.setOutputMode(getDefaultOutputMode()); 3.cloud.init(); 4. ... ;
*/
class DepthmapPointCloud	
{
public:
	DepthmapPointCloud(Context &contex, DepthGenerator &depthGenerator, XnMapOutputMode &outputMode)
						: m_hContex(contex), m_hDepthGenerator(depthGenerator), m_nStatus(XN_STATUS_OK),
						  m_pProjecttiveCloud(0), m_pRealworldCloud(0) 
	{
		m_nOutputMode.nFPS = outputMode.nFPS;
		m_nOutputMode.nXRes= outputMode.nXRes;
		m_nOutputMode.nYRes = outputMode.nYRes;
		m_pProjecttiveCloud = new XnPoint3D[getPointCloudNum()];
		m_pRealworldCloud = new XnPoint3D[getPointCloudNum()];
	}	
	~DepthmapPointCloud()
	{
		stop();
	}
	inline XnPoint3D* getPointCloudData()
	{
		return m_pRealworldCloud;
	}
	inline const XnUInt32 getPointCloudNum() const
	{
		return m_nOutputMode.nXRes*m_nOutputMode.nYRes;
	}
	inline const XnMapOutputMode& getXYRes() const 
	{
		return m_nOutputMode;
	}
	inline const XnMapOutputMode getDefaultOutputMode()
	{
		// set the depth map output mode
		XnMapOutputMode outputMode = {XN_VGA_X_RES, XN_VGA_Y_RES, 30};
		return outputMode;
	}
	/** 
	* test: return projective point cloud data
	*/
	inline XnPoint3D* getRawPointData()
	{
		return m_pProjecttiveCloud;
	}
	/** 
	* test: get field of view
	*/
	inline void getFieldofView(XnFieldOfView &FOV)const
	{
		m_hDepthGenerator.GetFieldOfView(FOV);
	}

	void DepthmapPointCloud::stop()
	{
		m_hContex.Release();
		delete [] m_pProjecttiveCloud;
		delete [] m_pRealworldCloud;
	}

	void DepthmapPointCloud::updataPointCloud()
	{
		try
		{
			// update to next frame data
			m_nStatus = m_hContex.WaitOneUpdateAll(m_hDepthGenerator);
			// printf("DataSize: %d\n", m_hDepthGenerator.GetDataSize());		// test
			// store depthmap data to projective cloudpoint data
			XnUInt32 index, shiftStep;
			for (XnUInt32 row=0; row<m_nOutputMode.nYRes; ++row)
			{
				shiftStep = row*m_nOutputMode.nXRes;
				for (XnUInt32 column=0; column<m_nOutputMode.nXRes; ++column)
				{
					index = shiftStep + column;
					m_pProjecttiveCloud[index].X = (XnFloat)column;
					m_pProjecttiveCloud[index].Y = (XnFloat)row;
					m_pProjecttiveCloud[index].Z = m_hDepthGenerator.GetDepthMap()[index]*0.001f; // mm -> m
				}
			}
			if (m_nStatus != XN_STATUS_OK) throw m_nStatus;
			// convert projective pointcloud to real world pointcloud
			m_hDepthGenerator.ConvertProjectiveToRealWorld(m_nOutputMode.nXRes*m_nOutputMode.nYRes, m_pProjecttiveCloud, m_pRealworldCloud);
		}
		catch (...)
		{
			printError();
			stop();
		}
	}

private:
	Context &m_hContex;
	DepthGenerator &m_hDepthGenerator;
	XnStatus m_nStatus;
	XnMapOutputMode m_nOutputMode;
	XnPoint3D *m_pProjecttiveCloud;
	XnPoint3D *m_pRealworldCloud;

	inline void printError()
	{
		if (m_nStatus != XN_STATUS_OK)
		{
			printf("Error: %s", xnGetStatusString(m_nStatus));
			exit(-1);
		}
	}
	DepthmapPointCloud(const DepthmapPointCloud &rhs);	// don't define copy constructor
	DepthmapPointCloud& operator=(const DepthmapPointCloud &rhs); // don't define assignment operator
};
class SimpleOpenNIViewer
{
public:
    SimpleOpenNIViewer () : viewer ("PCL OpenNI Viewer") {}

    void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZ>::ConstPtr &cloud)
    {
    if (!viewer.wasStopped())
        viewer.showCloud (cloud);
    }

    void run ()
    {
    pcl::Grabber* interface = new pcl::OpenNIGrabber();

    boost::function<void (const pcl::PointCloud<pcl::PointXYZ>::ConstPtr&)> f =
        boost::bind (&SimpleOpenNIViewer::cloud_cb_, this, _1);

    interface->registerCallback (f);

    interface->start ();

    while (!viewer.wasStopped())
    {
        boost::this_thread::sleep (boost::posix_time::seconds (1));
    }

    interface->stop ();
    }

    pcl::visualization::CloudViewer viewer;
};

#endif