# SadxModPlayerJumpVelocity

A Sonic Adventure DX (2011 Steam) mod that increases jump height by adding a
small constant to the player's vertical velocity every frame during the rising
phase of a jump.

## How it works

The rising-phase gravity calculation at `0x004977EE` is:

```
fstp dword ptr [ebx+3C]   ; store the newly computed vertical velocity
pop  ebx
add  esp, 24
```

At mod load, `Init()` uses `WriteJump()` (from `MemAccess.h`) to redirect
`0x004977EE` into `JumpBoostDetour()`, a `__declspec(naked)` function that:

1. Runs the original velocity store.
2. Reloads the velocity, adds `boostAmount` (`0.3f`), and stores it back.
3. Replays the two overwritten stack-cleanup instructions.
4. Jumps back to `0x004977F5` (the hook address + the 7 overwritten bytes).

The 2 bytes past the 5-byte `WriteJump` are NOP'd so no stale partial
instruction remains, mirroring the `nop 2` in the Cheat Engine script this was
translated from.

This approach (additive-per-frame during a continuous phase) was chosen after
testing showed that boosting the one-time jump impulse or hooking the
continuously-overwritten position write had no visible effect. See
`SADX_MODDING_Plan.md` in the parent directory for the full RE journey.

## Building

Open `SadxJumpVelocityMod/SadxJumpVelocityMod.sln` in Visual Studio (C++ Desktop
workload) and build **Debug | x86** — SADX is 32-bit, so Win32/x86 is required.

Command line:

```
msbuild SadxJumpVelocityMod/SadxJumpVelocityMod.sln -p:Configuration=Debug -p:Platform=x86
```

Output: `SadxJumpVelocityMod/Debug/SadxJumpVelocityMod.dll`.

## Installing

1. Create a folder in the game's `mods/` directory (e.g. `mods/SadxJumpVelocity/`).
2. Copy `mod.ini` and the built `SadxJumpVelocityMod.dll` into it.
3. Enable the mod in SADXModManager and launch.

## Tuning

Change `boostAmount` in `dllmain.cpp` and rebuild. Higher = higher jumps.
