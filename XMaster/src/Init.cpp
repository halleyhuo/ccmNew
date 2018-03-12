#include "Init.h"
#include "Fluoro.h"
CXtpCom* xtpCommInstanse = NULL;
static CFluoro* fluoroInstanse = NULL;
namespace IDC
{
    const int debug_level_remote=LOG_NOTICE+1; 
    const int debug_level_local=0;
}

int main(int argc, char* argv[])
{    
	xtpCommInstanse = new CXtpCom();
	xtpCommInstanse->Init();	
	xtpCommInstanse->Sigleton(xtpCommInstanse);

	fluoroInstanse = new CFluoro();
	fluoroInstanse->Init();	

     while(true)
     {
        sleep(5);
     }
	return 0;
}
