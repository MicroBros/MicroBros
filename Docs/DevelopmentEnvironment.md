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

NOTE: On Windows there seems to be issues building the Simulator with GCC, if this arrises one can make CMake pick clang instead globally by setting the following environment variables:
- Open the Run dialouge by Win+R or right clicking the Windows symbol and selecting Run
- Paste in `"C:\Windows\system32\rundll32.exe" sysdm.cpl,EditEnvironmentVariables` and press Enter
- Add the following variables under User variables (these paths are based on that you used Scoop to install LLVM/clang, use other path if installed by other means)
    - `ASM` to `%UserProfile%\scoop\apps\llvm\current\bin\clang.exe`
    - `CC` to `%UserProfile%\scoop\apps\llvm\current\bin\clang.exe`
    - `CXX` to `%UserProfile%\scoop\apps\llvm\current\bin\clang++.exe`
    - `RC` to `%UserProfile%\scoop\apps\llvm\current\bin\llvm-rc.exe`

## IDE / Editor

As per now, only Visual Studio Code is being actively used.

The following extensions are recommended to install:
- `EditorConfig.EditorConfig` (.editorconfig integration)
- `ms-vscode.cpptools` (C/C++ integration)
- `ms-vscode.cmake-tools` (VSCode CMake integration)
- `twxs.cmake` (CMake syntax highlighting)
- `vadimcn.vscode-lldb` (Natibe debugger using LLDB from LLVM)

## Flashing firmware
Using pyocd one can flash the firmware to the micro:bit v2.

If you are missing pyocd, it can be installed (after installing Python 3) by:
```
pip install -U pyocd
```

After this one can flash the firmware after building the `Firmware` target in the `Firmware` preset by running
```
pyocd load --target nrf52833 build/Firmware/Source/Firmware/Firmware.hex
```