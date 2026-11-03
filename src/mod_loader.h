#ifndef MOD_LOADER_H
#define MOD_LOADER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void ModLoader_Log(const char* fmt, ...);
void ModLoader_Init(void);
void ModLoader_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif // MOD_LOADER_H

