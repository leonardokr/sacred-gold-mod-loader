# Sacred Gold Mod Loader

A lightweight, transparent mod and plugin loader for **Sacred Gold** (Steam, GOG, and CD/DVD retail releases).

Allows loading multiple custom DLL mods seamlessly from a `mods/` directory without altering `Sacred.exe` or overwriting original game binaries.

---

## Features

- **Zero Executable Modification:** Hooks dynamically at startup via Windows `version.dll` proxy forwarding.
- **Clean Mod Management:** All mods are placed in a dedicated `mods/` subfolder.
- **Easy Toggle:** To disable a mod, simply rename it (e.g. from `.dll` to `.dll.disabled`).
- **Startup Diagnostics:** Logs mod loading status to `sacred_mod_loader.log`.

---

## Installation

1. Download `version.dll` from the latest [Releases](../../releases) tab.
2. Place `version.dll` into your Sacred Gold installation folder (where `Sacred.exe` is located).
3. (Optional) Create a `mods/` folder in the game directory if it does not already exist (the loader will also create it automatically on first launch).
4. Drop your `.dll` mods into the `mods/` folder.
5. Launch the game normally via Steam, GOG, or `Sacred.exe`.

---

## Mod Directory Structure

```
Sacred Gold/
├── Sacred.exe
├── version.dll                 <- Sacred Gold Mod Loader
├── sacred_mod_loader.log       <- Diagnostic log
└── mods/
    ├── sacred-language-loader.dll
    └── sacred-region-counter.dll
```

---

## Creating Mods for this Loader

Any standard 32-bit (x86) Windows DLL placed inside `mods/` is automatically loaded via `LoadLibraryA` when the game starts.

You do **not** need to proxy system APIs or configure export tables:
```c
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        // Your mod initialization & hooks here
    }
    return TRUE;
}
```

---

## Building from Source

Requirements:
- Visual Studio 2022 (MSVC x86 / 32-bit compiler tools)

Run:
```cmd
src\build.bat
```
The compiled DLL will be placed in `bin\version.dll` and root `version.dll`.

