// Std-includes of c++
#include <iostream>
using namespace std;

// Internal includes of this program
#include "KProgram.h"
#include "SampleManager.h"

int main(int argc, char* argv[]){

	SampleManager* pSampleManager=NULL; // will hold the sample manager which initializes and runs the sample

    if(pSampleManager==NULL) // by default we use the closest detector option
    {
        pSampleManager=XN_NEW(ClosestSampleManager,1); // choose the default.
		new KProgram(pSampleManager);
    }

    if(pSampleManager->StartSample(argc,argv)!=XN_STATUS_OK) 
    {
		XN_DELETE(pSampleManager);
    }
			
	return 0;
}