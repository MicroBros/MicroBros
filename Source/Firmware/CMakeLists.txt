add_executable(Firmware
    src/BLE/MotorService.cpp src/BLE/MotorService.h
    src/Drivers/DFR0548.cpp src/Drivers/DFR0548.h
    src/Drivers/HCSR04.cpp src/Drivers/HCSR04.h
    src/main.cpp
    src/Mouse2.cpp src/Mouse2.h
    src/PID.cpp src/PID.h
    src/Timer.cpp src/Timer.h
    src/Utils.h
)
target_link_libraries(Firmware PUBLIC Core)

microbit_executable(Firmware)
