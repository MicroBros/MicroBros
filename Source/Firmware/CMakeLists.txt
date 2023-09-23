add_executable(Firmware
    src/Drivers/DFR0548.cpp
    src/Drivers/DFR0548.h
    src/main.cpp
)
target_link_libraries(Firmware PUBLIC Core)

microbit_executable(Firmware)
