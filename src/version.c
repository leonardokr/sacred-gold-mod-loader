#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "mod_loader.h"

static HMODULE g_real_version_dll = NULL;

typedef BOOL (WINAPI *Type_GetFileVersionInfoA)(LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI *Type_GetFileVersionInfoByHandle)(int, HANDLE, LPVOID*, LPDWORD);
typedef BOOL (WINAPI *Type_GetFileVersionInfoExA)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI *Type_GetFileVersionInfoExW)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI *Type_GetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
typedef DWORD (WINAPI *Type_GetFileVersionInfoSizeExA)(DWORD, LPCSTR, LPDWORD);
typedef DWORD (WINAPI *Type_GetFileVersionInfoSizeExW)(DWORD, LPCWSTR, LPDWORD);
typedef DWORD (WINAPI *Type_GetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
typedef BOOL (WINAPI *Type_GetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI *Type_VerFindFileA)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT, LPSTR, PUINT);
typedef DWORD (WINAPI *Type_VerFindFileW)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
typedef DWORD (WINAPI *Type_VerInstallFileA)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT);
typedef DWORD (WINAPI *Type_VerInstallFileW)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT);
typedef DWORD (WINAPI *Type_VerLanguageNameA)(DWORD, LPSTR, DWORD);
typedef DWORD (WINAPI *Type_VerLanguageNameW)(DWORD, LPWSTR, DWORD);
typedef BOOL (WINAPI *Type_VerQueryValueA)(LPCVOID, LPCSTR, LPVOID*, PUINT);
typedef BOOL (WINAPI *Type_VerQueryValueW)(LPCVOID, LPCWSTR, LPVOID*, PUINT);

static Type_GetFileVersionInfoA pfn_GetFileVersionInfoA = NULL;
static Type_GetFileVersionInfoByHandle pfn_GetFileVersionInfoByHandle = NULL;
static Type_GetFileVersionInfoExA pfn_GetFileVersionInfoExA = NULL;
static Type_GetFileVersionInfoExW pfn_GetFileVersionInfoExW = NULL;
static Type_GetFileVersionInfoSizeA pfn_GetFileVersionInfoSizeA = NULL;
static Type_GetFileVersionInfoSizeExA pfn_GetFileVersionInfoSizeExA = NULL;
static Type_GetFileVersionInfoSizeExW pfn_GetFileVersionInfoSizeExW = NULL;
static Type_GetFileVersionInfoSizeW pfn_GetFileVersionInfoSizeW = NULL;
static Type_GetFileVersionInfoW pfn_GetFileVersionInfoW = NULL;
static Type_VerFindFileA pfn_VerFindFileA = NULL;
static Type_VerFindFileW pfn_VerFindFileW = NULL;
static Type_VerInstallFileA pfn_VerInstallFileA = NULL;
static Type_VerInstallFileW pfn_VerInstallFileW = NULL;
static Type_VerLanguageNameA pfn_VerLanguageNameA = NULL;
static Type_VerLanguageNameW pfn_VerLanguageNameW = NULL;
static Type_VerQueryValueA pfn_VerQueryValueA = NULL;
static Type_VerQueryValueW pfn_VerQueryValueW = NULL;

static void LoadOriginalVersionDll(void) {
    if (g_real_version_dll) return;

    char sys_dir[MAX_PATH];
    GetSystemDirectoryA(sys_dir, MAX_PATH);
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s\\version.dll", sys_dir);

    g_real_version_dll = LoadLibraryA(path);
    if (!g_real_version_dll) {
        ModLoader_Log("[CRITICAL] Failed to load system version.dll from %s", path);
        return;
    }

    pfn_GetFileVersionInfoA = (Type_GetFileVersionInfoA)GetProcAddress(g_real_version_dll, "GetFileVersionInfoA");
    pfn_GetFileVersionInfoByHandle = (Type_GetFileVersionInfoByHandle)GetProcAddress(g_real_version_dll, "GetFileVersionInfoByHandle");
    pfn_GetFileVersionInfoExA = (Type_GetFileVersionInfoExA)GetProcAddress(g_real_version_dll, "GetFileVersionInfoExA");
    pfn_GetFileVersionInfoExW = (Type_GetFileVersionInfoExW)GetProcAddress(g_real_version_dll, "GetFileVersionInfoExW");
    pfn_GetFileVersionInfoSizeA = (Type_GetFileVersionInfoSizeA)GetProcAddress(g_real_version_dll, "GetFileVersionInfoSizeA");
    pfn_GetFileVersionInfoSizeExA = (Type_GetFileVersionInfoSizeExA)GetProcAddress(g_real_version_dll, "GetFileVersionInfoSizeExA");
    pfn_GetFileVersionInfoSizeExW = (Type_GetFileVersionInfoSizeExW)GetProcAddress(g_real_version_dll, "GetFileVersionInfoSizeExW");
    pfn_GetFileVersionInfoSizeW = (Type_GetFileVersionInfoSizeW)GetProcAddress(g_real_version_dll, "GetFileVersionInfoSizeW");
    pfn_GetFileVersionInfoW = (Type_GetFileVersionInfoW)GetProcAddress(g_real_version_dll, "GetFileVersionInfoW");
    pfn_VerFindFileA = (Type_VerFindFileA)GetProcAddress(g_real_version_dll, "VerFindFileA");
    pfn_VerFindFileW = (Type_VerFindFileW)GetProcAddress(g_real_version_dll, "VerFindFileW");
    pfn_VerInstallFileA = (Type_VerInstallFileA)GetProcAddress(g_real_version_dll, "VerInstallFileA");
    pfn_VerInstallFileW = (Type_VerInstallFileW)GetProcAddress(g_real_version_dll, "VerInstallFileW");
    pfn_VerLanguageNameA = (Type_VerLanguageNameA)GetProcAddress(g_real_version_dll, "VerLanguageNameA");
    pfn_VerLanguageNameW = (Type_VerLanguageNameW)GetProcAddress(g_real_version_dll, "VerLanguageNameW");
    pfn_VerQueryValueA = (Type_VerQueryValueA)GetProcAddress(g_real_version_dll, "VerQueryValueA");
    pfn_VerQueryValueW = (Type_VerQueryValueW)GetProcAddress(g_real_version_dll, "VerQueryValueW");
}

BOOL WINAPI FakeGetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoA ? pfn_GetFileVersionInfoA(lptstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

BOOL WINAPI FakeGetFileVersionInfoByHandle(int hMem, HANDLE hFile, LPVOID* lplpData, LPDWORD lpdwLen) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoByHandle ? pfn_GetFileVersionInfoByHandle(hMem, hFile, lplpData, lpdwLen) : FALSE;
}

BOOL WINAPI FakeGetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoExA ? pfn_GetFileVersionInfoExA(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

BOOL WINAPI FakeGetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoExW ? pfn_GetFileVersionInfoExW(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

DWORD WINAPI FakeGetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoSizeA ? pfn_GetFileVersionInfoSizeA(lptstrFilename, lpdwHandle) : 0;
}

DWORD WINAPI FakeGetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoSizeExA ? pfn_GetFileVersionInfoSizeExA(dwFlags, lpwstrFilename, lpdwHandle) : 0;
}

DWORD WINAPI FakeGetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoSizeExW ? pfn_GetFileVersionInfoSizeExW(dwFlags, lpwstrFilename, lpdwHandle) : 0;
}

DWORD WINAPI FakeGetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoSizeW ? pfn_GetFileVersionInfoSizeW(lptstrFilename, lpdwHandle) : 0;
}

BOOL WINAPI FakeGetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    LoadOriginalVersionDll();
    return pfn_GetFileVersionInfoW ? pfn_GetFileVersionInfoW(lptstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

DWORD WINAPI FakeVerFindFileA(DWORD uFlags, LPCSTR szFileName, LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen) {
    LoadOriginalVersionDll();
    return pfn_VerFindFileA ? pfn_VerFindFileA(uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen) : 0;
}

DWORD WINAPI FakeVerFindFileW(DWORD uFlags, LPCWSTR szFileName, LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen) {
    LoadOriginalVersionDll();
    return pfn_VerFindFileW ? pfn_VerFindFileW(uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen) : 0;
}

DWORD WINAPI FakeVerInstallFileA(DWORD uFlags, LPCSTR szSrcFileName, LPCSTR szDestFileName, LPCSTR szSrcDir, LPCSTR szDestDir, LPCSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen) {
    LoadOriginalVersionDll();
    return pfn_VerInstallFileA ? pfn_VerInstallFileA(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen) : 0;
}

DWORD WINAPI FakeVerInstallFileW(DWORD uFlags, LPCWSTR szSrcFileName, LPCWSTR szDestFileName, LPCWSTR szSrcDir, LPCWSTR szDestDir, LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen) {
    LoadOriginalVersionDll();
    return pfn_VerInstallFileW ? pfn_VerInstallFileW(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen) : 0;
}

DWORD WINAPI FakeVerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD nSize) {
    LoadOriginalVersionDll();
    return pfn_VerLanguageNameA ? pfn_VerLanguageNameA(wLang, szLang, nSize) : 0;
}

DWORD WINAPI FakeVerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD nSize) {
    LoadOriginalVersionDll();
    return pfn_VerLanguageNameW ? pfn_VerLanguageNameW(wLang, szLang, nSize) : 0;
}

BOOL WINAPI FakeVerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    LoadOriginalVersionDll();
    return pfn_VerQueryValueA ? pfn_VerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen) : FALSE;
}

BOOL WINAPI FakeVerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    LoadOriginalVersionDll();
    return pfn_VerQueryValueW ? pfn_VerQueryValueW(pBlock, lpSubBlock, lplpBuffer, puLen) : FALSE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        ModLoader_Init();
    } else if (fdwReason == DLL_PROCESS_DETACH) {
        ModLoader_Shutdown();
    }
    return TRUE;
}

