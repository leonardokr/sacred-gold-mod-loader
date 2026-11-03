#include "mod_loader.h"
#include <stdio.h>
#include <stdarg.h>

#define MAX_LOADED_MODS 64

static HMODULE g_loaded_mods[MAX_LOADED_MODS];
static int g_loaded_count = 0;
static const char* LOG_FILE = "sacred_mod_loader.log";

void ModLoader_Log(const char* fmt, ...) {
    FILE* f = fopen(LOG_FILE, "a");
    if (f) {
        va_list args;
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        va_end(args);
        fprintf(f, "\n");
        fclose(f);
    }
}

void ModLoader_Init(void) {
    DeleteFileA(LOG_FILE);
    ModLoader_Log(" Sacred Gold Mod Loader v1.0.0");

    if (!CreateDirectoryA("mods", NULL)) {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS) {
            ModLoader_Log("[WARN] Could not create 'mods' directory (Error: %lu)", err);
        }
    }

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA("mods\\*.dll", &find_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        ModLoader_Log("[INFO] No mods found in 'mods\\' directory.");
        ModLoader_Log("[INFO] To install mods, drop .dll files into the 'mods\\' folder.");
        return;
    }

    ModLoader_Log("[INFO] Scanning 'mods\\' folder for plugins...");

    do {
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        char mod_path[MAX_PATH];
        snprintf(mod_path, sizeof(mod_path), "mods\\%s", find_data.cFileName);

        ModLoader_Log("[INFO] Loading mod: %s", find_data.cFileName);

        HMODULE hMod = LoadLibraryA(mod_path);
        if (hMod) {
            ModLoader_Log("       -> Success (Handle: 0x%p)", hMod);
            if (g_loaded_count < MAX_LOADED_MODS) {
                g_loaded_mods[g_loaded_count++] = hMod;
            }
        } else {
            DWORD err = GetLastError();
            ModLoader_Log("       -> FAILED (GetLastError: %lu)", err);
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);

    ModLoader_Log("[INFO] Initialization complete. %d mod(s) loaded.", g_loaded_count);
}

void ModLoader_Shutdown(void) {
    ModLoader_Log("[INFO] Sacred Gold shutting down. Unloading mods...");
    for (int i = g_loaded_count - 1; i >= 0; i--) {
        if (g_loaded_mods[i]) {
            FreeLibrary(g_loaded_mods[i]);
            g_loaded_mods[i] = NULL;
        }
    }
    g_loaded_count = 0;
    ModLoader_Log("[INFO] Mod Loader cleanly stopped.");
}

