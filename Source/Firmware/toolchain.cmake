# Setup system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Toolchain extension for :b:indows
if(WIN32)
    set(EXE_EXT ".exe")
else()
    set(EXE_EXT "")
endif()

# Setup toolchain and target compilers
set(TOOLCHAIN arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc${EXE_EXT} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++${EXE_EXT} CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc${EXE_EXT} CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY ${TOOLCHAIN}-objcopy${EXE_EXT})

# Setup toolchain flags
set(COMMON_FLAGS
    -Os
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wno-register # Allow register storage class in C++17
    -mthumb
    --param max-inline-insns-single=500
)

set(CFLAGS ${COMMON_FLAGS})
list(JOIN CFLAGS " " CMAKE_C_FLAGS)

set(CXXFLAGS ${COMMON_FLAGS}
    -fno-rtti
    -fno-exceptions
    -fno-threadsafe-statics
)
list(JOIN CXXFLAGS " " CMAKE_CXX_FLAGS)

set(LINKER_FLAGS
    -Os
    -Wl,--gc-sections
    -mthumb
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--check-sections
    -Wl,--unresolved-symbols=report-all
    -Wl,--warn-common
    -Wl,--warn-section-align
)
list(JOIN LINKER_FLAGS " " CMAKE_EXE_LINKER_FLAGS)

function(microbit_executable TARGET)
    # Setup linker script
    set_target_properties(${TARGET} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/ThirdParty/arduino-nRF5/cores/nRF5/SDK/components/toolchain/gcc/nrf52833_xxaa.ld)
    target_link_libraries(${TARGET} PUBLIC ThirdParty::arduino-nRF5)

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> $<TARGET_FILE:${TARGET}>.bin
    VERBATIM)

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom $<TARGET_FILE:${TARGET}> $<TARGET_FILE:${TARGET}>.hex
    VERBATIM)
endfunction()
