#include "boot_init.h"
#include "boot_lib.sig"
#include <string.h>

///////////////////////////////INIT MODULE/////////////////////
int16_t init_pm(struct PM_INFO* pm, int8_t start_status){
	int16_t retcode = boot_pm_install(pm->path, pm->name);

	PROCESS pm_pid = boot_pm_create(pm->name);

	if(start_status == AUTO_START){
		//Запуск ПМ
		retcode = boot_pm_start(pm_pid);
	}

    return retcode;
}
///////////////////////////////INIT MODULE/////////////////////

/////////////////////////////PM_INSTALL////////////////////////
int16_t boot_pm_install(const char * bootpath, const char * handle)
{
	if(!bootpath || !handle)
	{
		print_debug("Error: %s - invalid argument\n", __func__);
		return ERR_PM_INSTALL;
	}

	int ret_code;
	
	PROCESS open_sdk_pm_pid;

	if (hunt("open_sdk_program_mgr", 0, &open_sdk_pm_pid, (void *)0) == 0)
	{
		print_debug("open_sdk_program_mgr not found\n");
		
		return ERR_open_sdk_MANAGER;
	}

	static SIGSELECT sig_sel[2] = {1, PM_INSTALL_LOAD_MODULE_REPLY};

	union SIGNAL *sig;
	FILE *f;

	f = fopen(bootpath, "r");

	if (f == NULL)
	{
		print_debug("%s not found!\n", bootpath);
		return ERR_FD_OPEN;
	}

	fclopen_sdk(f);

	struct PmInstallLoadModuleReply   *reply_install;

	sig = alloc(sizeof(struct PmInstallLoadModuleRequest), PM_INSTALL_LOAD_MODULE_REQUEST);	
	sig->PmInstallLoadModuleRequest.options = 0;
	strcpy(sig->PmInstallLoadModuleRequest.file_format, "ELF");
	strcpy(sig->PmInstallLoadModuleRequest.file_name, bootpath);
	strcpy(sig->PmInstallLoadModuleRequest.install_handle, handle);
	sig->PmInstallLoadModuleRequest.conf_no_of_strings = 0;
	sig->PmInstallLoadModuleRequest.conf_size = 0;

	/* Send request to PM. */
	send(&sig, open_sdk_pm_pid);
	reply_install = (struct PmInstallLoadModuleReply *)receive(sig_sel);

	ret_code = reply_install->status;
	free_buf ((union SIGNAL **)&reply_install);

	if (ret_code != RET_SUCCESS)
	{
		print_debug("installation of boot module %s failed with status 0x%x\n", bootpath, ret_code);
		return ERR_PM_INSTALL;
	}

	return NO_ERROR;
}
/////////////////////////////PM_INSTALL////////////////////////

/////////////////////////////PM_CREATE////////////////////////
PROCESS boot_pm_create(const char * handle)
{
	struct     PmCreateProgramReply   *reply_creat;
	PROCESS prog_pid = (PROCESS)NIL;
	PROCESS open_sdk_pm_pid = (PROCESS)NIL;
	union SIGNAL *sig;

		static SIGSELECT sig_sel[2] = {1, PM_CREATE_PROGRAM_REPLY};

	if (hunt("open_sdk_program_mgr", 0, &open_sdk_pm_pid, (void *)0) == 0)
	{
		print_debug("open_sdk_program_mgr not found\n");
		
            return (PROCESS)NIL;
	}

	sig = alloc(sizeof (struct PmCreateProgramRequest), PM_CREATE_PROGRAM_REQUEST);

	strcpy(sig->PmCreateProgramRequest.install_handle, handle);
	sig->PmCreateProgramRequest.domain = PM_NEW_DOMAIN;
	sig->PmCreateProgramRequest.conf_size = 0;
	sig->PmCreateProgramRequest.conf_no_of_strings = 0;

	send(&sig, open_sdk_pm_pid);

	reply_creat = (struct PmCreateProgramReply *)receive(sig_sel);

	int ret_code = reply_creat->status;
	prog_pid = reply_creat->progpid;
	free_buf ((union SIGNAL **)&reply_creat);

	if (ret_code != RET_SUCCESS)
	{
		print_debug("Creating LM %s failed!\n", handle);
	}

	return prog_pid;
}
/////////////////////////////PM_CREATE////////////////////////

/////////////////////////////PM_START////////////////////////
int16_t boot_pm_start(PROCESS prog_pid)
{
	struct     PmStartProgramReply   *reply_start;
	union SIGNAL *sig;

	static SIGSELECT sig_sel[2] = {1,PM_START_PROGRAM_REPLY};

	PROCESS open_sdk_pm_pid;

	if (hunt("open_sdk_program_mgr", 0, &open_sdk_pm_pid, (void *)0) == 0)
	{
		print_debug("open_sdk_program_mgr not found\n");
		
		return ERR_open_sdk_MANAGER;
	}
	
	sig = alloc(sizeof (struct PmStartProgramRequest), PM_START_PROGRAM_REQUEST);

	sig->PmStartProgramRequest.progpid = prog_pid;

	send(&sig, open_sdk_pm_pid);
	
	reply_start = (struct PmStartProgramReply *)receive(sig_sel);
	
	int ret_code = reply_start->status;
	free_buf ((union SIGNAL **)&reply_start);

	return ret_code;
}
/////////////////////////////PM_START////////////////////////
