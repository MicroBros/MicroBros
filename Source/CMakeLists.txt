add_subdirectory(Core)

# Omit desktop targets when building Firmware
if(NOT FIRMWARE)
    add_subdirectory(Simulator)
endif()
