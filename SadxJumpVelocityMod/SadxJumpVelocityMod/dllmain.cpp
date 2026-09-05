// dllmain.cpp : Defines the entry point for the DLL application.
//
// Player Jump Velocity Boost
// --------------------------
// Increases jump height by adding a small constant to the player's vertical
// velocity every frame during the rising phase of a jump.
//
// The rising-phase gravity calculation lives at 0x004977EE:
//
//     fstp dword ptr [ebx+3C]   ; 3 bytes  -- store the new velocity
//     pop  ebx                  ; 1 byte
//     add  esp, 24              ; 3 bytes
//                              ; = 7 bytes total, overwritten by our JMP (5) + 2 pad
//
// We redirect that instruction to JumpBoostDetour(), which runs the original
// store, adds boostAmount on top, then replays the two stack-cleanup
// instructions and jumps back to the continuation point (0x004977EE + 7).
//
// This mirrors the Cheat Engine auto-assemble script that was validated in-game.

#include "pch.h"

// Continuation point: 0x004977EE + 7 bytes of overwritten instructions.
#define JUMP_BOOST_HOOK_ADDR    0x004977EE
#define JUMP_BOOST_RETURN_ADDR  0x004977F5

// Additive velocity boost applied per frame while the player is ascending.
// 0.3 produces a fun, noticeably large jump (confirmed in-game).
static float boostAmount = 0.3f;

// MSVC inline asm can't `jmp` a bare immediate; jump indirectly through this.
static const void* const jumpBoostReturnAddr = (void*)JUMP_BOOST_RETURN_ADDR;

static void __declspec(naked) JumpBoostDetour()
{
    __asm
    {
        // --- original instruction we replaced ---
        fstp dword ptr [ebx+0x3C]

        // --- our addition: velocity += boostAmount ---
        fld  dword ptr [ebx+0x3C]
        fadd dword ptr [boostAmount]
        fstp dword ptr [ebx+0x3C]

        // --- remaining overwritten instructions ---
        pop ebx
        add esp, 0x24

        jmp jumpBoostReturnAddr
    }
}

extern "C"
{
    __declspec(dllexport) void __cdecl Init(const char* path,
        const HelperFunctions& helperFunctions)
    {
        // Patch the rising-phase gravity calc to jump into our detour.
        // Applied once at mod load; permanent for the process lifetime.
        WriteJump((void*)JUMP_BOOST_HOOK_ADDR, (void*)JumpBoostDetour);

        // WriteJump only writes 5 bytes; NOP the 2 trailing bytes of the
        // 7-byte overwritten region so no stale partial instruction is left
        // (mirrors the "nop 2" in the validated Cheat Engine script).
        WriteData<2>((void*)(JUMP_BOOST_HOOK_ADDR + 5), (uint8_t)0x90);
    }

    __declspec(dllexport) void __cdecl OnFrame()
    {
        // Nothing to do per-frame -- the hook does all the work.
    }

    __declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}
