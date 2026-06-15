# Building F4Online

F4Online is an [F4SE](https://f4se.silverlock.org/) plugin written in C++. It
builds against the vendored F4SE 0.7.8 SDK and the `ianpatt/common` library,
both under `libs/`.

## Target runtime

F4SE **0.7.8** targets the **Next-Gen** Fallout 4 runtimes (**1.10.980+** and
the **1.11.x** Anniversary line). It is *not* compatible with the old-gen
1.10.163 game. If you intend to target old-gen instead, you need F4SE 0.6.23
and a different SDK.

## Prerequisites

- **Visual Studio 2022/2026** with the **Desktop development with C++** workload
  (gives you MSVC, the Windows SDK, CMake, and Ninja), or standalone CMake ≥ 3.21
  plus the MSVC build tools.

## Build (command line)

Open an **x64 Native Tools Command Prompt for VS** (so `cl`, `cmake`, and
`ninja` are on `PATH`), then from the repo root:

```bat
cmake --preset msvc-x64
cmake --build --preset msvc-x64
```

The plugin is produced at `build/F4Online.dll`. Use the `msvc-x64-debug`
presets for a debug build (output under `build-debug/`).

> Not in a VS prompt? Run `vcvars64.bat` first, e.g.
> `call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"`
> (adjust the path to your VS edition/year).

## Build (Visual Studio)

`File > Open > Folder...` on the repo root. VS detects `CMakePresets.json`,
configures automatically, and you can build the `F4Online` target from the IDE.

## Installing / testing in-game

1. Build `F4Online.dll`.
2. Copy it to `<Fallout 4>\Data\F4SE\Plugins\F4Online.dll`.
3. Launch the game through **f4se_loader.exe**.
4. Confirm load via the log at
   `Documents\My Games\Fallout4\F4SE\F4Online.log` (and `f4se.log`, which lists
   loaded plugins).
