# Development environment

## Dependencies

Third-party dependencies are managed using git submodules, fetch/update them with the following command:
```
git submodule update --init --recursive
```
It is recommended to do this after pulling changes from others as submodules may have been added or updated.

The following software/dependencies are required to work on MicroBros.
- Base dependencies
    - git (Version control software)
    - python3 (Needed for scripts & pyocd)
    - cmake (Project generation tool)
    - ninja (Build tool)
    - pkg-config (Tool to locate pkg-config manifests)
    - gcc (Compiler)
    - llvm (Compiler suite and more, contains clang-format and more)
    - doxygen (Documentation generator)
- Firmware dependencies
    - gcc-arm-none-eabi (Compiler needed to compile for the embedded ARM MCU in the micro:bit v2)
    - pyocd (Used to flash the micro:bit v2)

If missing these dependencies can easily be installed with a package manager, on Windows you can choose from options like [WinGet](https://learn.microsoft.com/en-us/windows/package-manager/winget/) or [Scoop](https://scoop.sh/).

NOTE: On Windows one needs Visual Studio installed with C++ for native development to use MSVC, it does not build with GCC or Clang at the moment.

## IDE / Editor

As per now, only Visual Studio Code is being actively used.

The following extensions are recommended to install:
- `EditorConfig.EditorConfig` (.editorconfig integration)
- `ms-vscode.cpptools` (C/C++ integration)
- `ms-vscode.cmake-tools` (VSCode CMake integration)
- `twxs.cmake` (CMake syntax highlighting)
- `vadimcn.vscode-lldb` (Natibe debugger using LLDB from LLVM)
- `marus25.cortex-debug` (Automatic setup of PyOCD for upload and debugging)

## Flashing firmware
Using pyocd one can flash the firmware to the micro:bit v2.
The Firmware can be built as the Firmware target under the Firmware CMake preset

If you are missing pyocd, it can be installed (after installing Python 3) by:
```
pip install -U pyocd
```

One can completely erase the flash like this if needed
```
pyocd erase --mass --t nrf52833
```

After this one can flash the firmware after building the `Firmware` target in the `Firmware` preset by running
```
pyocd load --target nrf52833 build/Firmware/Source/Firmware/Firmware.elf
```

## Debugging firmware
- Select Debug (PyOCD) as Debug launch target
- Change CMake configure preset to Firmware
- Set the default Build target to Firmware
- Press F5, after Firmware is built, switch to gdb-server Terminal to see PyOCD uploading
