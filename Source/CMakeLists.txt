add_subdirectory(Core)

# Add firmware exclusive targets
if(FIRMWARE)
    add_subdirectory(Firmware)

# Omit desktop targets when building Firmware
else()
    add_subdirectory(Simulator)
endif()
