// Std-includes of c++
#include <iostream>
using namespace std;

// Internal includes of this program
#include "KProgram.h"
#include "KinectAppManager.h"

int main(int argc, char* argv[]){

	KinectAppManager* pKinectApp=NULL; // will hold the sample manager which initializes and runs the sample

    if(pKinectApp==NULL) // by default we use the closest detector option
    {
        pKinectApp=XN_NEW(ClosestSampleManager,1); // choose the default.
		new KProgram(pKinectApp);
    }

	if(pKinectApp->StartKinectApp(argc,argv)!=XN_STATUS_OK) 
    {
		XN_DELETE(pKinectApp);
    }
			
	return 0;
}