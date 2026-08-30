// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

extern "C"
{
    __declspec(dllexport) void __cdecl Init(const char* path,
        const HelperFunctions& helperFunctions)
    {
        // Startup logic - runs once when the mod loads
    
    }

    __declspec(dllexport) void __cdecl OnFrame()
    {
        // Runs every frame
        
    }

    __declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}

