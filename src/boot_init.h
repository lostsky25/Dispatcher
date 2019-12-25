#ifndef BOOT_INIT_H_
#define BOOT_INIT_H_

//Структуры данных
#include "pm.sig"
//#include "direct.sig"
//#include "interface.sig"
#include "../debug_lib/err_lib.h"
//#include "stdint.h"
//#include "string.h"
#include "stdio.h"
#include "shell.h"
#include "../sig_exchange_lib/pm_module.h"
#include "../debug_lib/debug.h"

#define MANUAL_START 0
#define AUTO_START 1

int16_t boot_pm_install(const char*, const char*);
int16_t init_pm(struct PM_INFO*, int8_t);
PROCESS boot_pm_create(const char *);
int16_t boot_pm_start(PROCESS);

#endif /* BOOT_INIT_H_ */
