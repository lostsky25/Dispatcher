#ifndef BOOT_LIB_SIG_
#define BOOT_LIB_SIG_

#include "open_sdk.h"
//#include "pm.sig"

union SIGNAL
{
	SIGSELECT sigNo;
	struct PmInstallLoadModuleRequest PmInstallLoadModuleRequest; 
	struct PmInstallLoadModuleReply PmInstallLoadModuleReply; 

	struct PmCreateProgramRequest PmCreateProgramRequest; 
	struct PmCreateProgramReply PmCreateProgramReply; 

	struct PmStartProgramRequest PmStartProgramRequest; 
	struct PmStartProgramReply PmStartProgramReply; 
};

#endif
