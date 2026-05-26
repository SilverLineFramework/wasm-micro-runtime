#ifndef _WALI_INIT_H
#define _WALI_INIT_H

#include "wasm_export.h"

void wali_init_native();

extern bool invoked_wali;
extern int wali_app_argc;
extern char **wali_app_argv;
extern char *wali_app_env_file;


#endif
