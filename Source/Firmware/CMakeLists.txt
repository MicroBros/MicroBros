add_executable(Firmware
    src/BLE/MotorService.cpp src/BLE/MotorService.h
    src/BLE/MouseService.cpp src/BLE/MouseService.h
    src/Drivers/DFR0548.cpp src/Drivers/DFR0548.h
    src/Drivers/HCSR04.cpp src/Drivers/HCSR04.h
    src/Drivers/IR.cpp src/Drivers/IR.h
    src/Filters.cpp src/Filters.h
    src/main.cpp
    src/Mouse2.cpp src/Mouse2.h
    src/PID.cpp src/PID.h
    src/Timer.cpp src/Timer.h
    src/Utils.h
)
target_link_libraries(Firmware PUBLIC Core)

microbit_executable(Firmware)
