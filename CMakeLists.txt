cmake_minimum_required(VERSION 3.26)

# Init project
project(MicroBros LANGUAGES C CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Firmware")
    set(FIRMWARE ON)
endif()

# Configure C++
set(CMAKE_CXX_STANDARD 17)

# Enable DWARF 4 symbols on UNIX platforms
if(UNIX AND NOT FIRMWARE)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-4")
endif()

add_subdirectory(Source)
add_subdirectory(ThirdParty)
