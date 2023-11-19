# Software

All software projects uses CMake, depending on if the `Debug`/`Release` or `Firmware` preset is picked one can compile either the Simulator or Firmware as executable targets. This is done since `Firmware` needs a custom toolchain, see `Source/Firmware/toolchain.cmake` to compile for the Micro:bit v2.

## Core

The Core projected located in `Source/Core` contains all common code in the project, including data structures and enumerations for mazes, directions, and communication. It contains the maze solving algorithm implementations and it contains utilities for creating bitflag enums and cross-platform logging.

Algorithms are implemented by deriving the `Core::Algorithm` base class and implementing the `Step` function. The Algorithm instance is re-created on every Mouse state reset.

## Firmware

The `Firmware` subproject contains all the exclusive code to be ran on the Micro:bit v2. It contains the main event loop. Drivers for the DFR0548 (Motor driver), HC-SR04 (Ultrasonic distance sensor) and custom IR based distance.

It contains the main loop and state machine contained within the `Mouse2.[h,cpp]` files.

## Simulator

The `Simulator` is an utility both providing Algorithm simulation on desktop and enabling remote control and debugging.

It uses SDL and imgui for UI. The code is split into `Services` (Code that serves a specific function and contains state) and `Windows` (Imgui Window for functionality).

