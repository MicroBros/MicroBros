cmake_minimum_required(VERSION 3.26)

# Init project
project(MicroBros LANGUAGES ASM C CXX)

set(FIRMWARE OFF CACHE BOOL "Using Firmware targets")
if(FIRMWARE)
    set(CMAKE_BUILD_TYPE "Debug")
    add_definitions(-DFIRMWARE)
else()
    add_definitions(-DSIMULATOR)
endif()

# Configure C++
set(CMAKE_CXX_STANDARD 20)

# Enable DWARF 4 symbols on UNIX platforms
if(UNIX AND NOT FIRMWARE)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-4")
endif()

if(LINUX)
    add_definitions(-DLINUX)
endif()

add_subdirectory(Source)
add_subdirectory(ThirdParty)

# Doxygen
find_package(Doxygen)

set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_MAN YES)

if(DOXYGEN_FOUND)
    set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in ${DOXYFILE})

    add_custom_target(Docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
    )
    set_target_properties(Docs PROPERTIES EXCLUDE_FROM_ALL ON)
else()
    message("Doxygen not found, not adding Docs target")
endif()
