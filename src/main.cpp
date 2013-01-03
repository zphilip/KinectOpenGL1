// Std-includes of c++
#include <iostream>
#include <pthread.h>

using namespace std;

// Internal includes of this program
#include "KProgram.h"
#include "KinectAppManager.h"

void *PCLView(void *)
{
   SimpleOpenNIViewer v;
   v.run ();
   pthread_exit(NULL);
   
   return 0;
}

int main(int argc, char* argv[]){

	KinectAppManager* pKinectApp=NULL; // will hold the sample manager which initializes and runs the sample
	pthread_t threads;

    if(pKinectApp==NULL) // by default we use the closest detector option
    {
        pKinectApp=XN_NEW(ClosestSampleManager,1); // choose the default.
		new KProgram(pKinectApp);
		//pthread_create(&threads, NULL,PCLView,NULL);
		
    }

	if(pKinectApp->StartKinectApp(argc,argv)!=XN_STATUS_OK) 
    {
		XN_DELETE(pKinectApp);
    }
			
	return 0;
}

