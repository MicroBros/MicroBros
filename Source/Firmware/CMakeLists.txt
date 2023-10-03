add_executable(Firmware
    src/BLE/ControllerService.cpp src/BLE/ControllerService.h
    src/Drivers/DFR0548.cpp src/Drivers/DFR0548.h
    src/main.cpp
    src/Mouse.cpp src/Mouse.h
    src/Utils.h
)
target_link_libraries(Firmware PUBLIC Core)

microbit_executable(Firmware)
