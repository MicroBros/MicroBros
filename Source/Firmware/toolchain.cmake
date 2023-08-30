# Setup system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

find_program(ARM_NONE_EABI_AR arm-none-eabi-ar)
find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc)
find_program(ARM_NONE_EABI_GXX arm-none-eabi-g++)
find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy)
find_program(ARM_NONE_EABI_RANLIB arm-none-eabi-ranlib)

# Setup toolchain and target compilers
set(TOOLCHAIN arm-none-eabi)
set(CMAKE_C_COMPILER ${ARM_NONE_EABI_GCC} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${ARM_NONE_EABI_GXX} CACHE INTERNAL "C++ Compiler")
set(CMAKE_OBJCOPY ${ARM_NONE_EABI_OBJCOPY})

# Setup toolchain flags
set(COMMON_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -g
    -Os
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -ffunction-sections
    -fdata-sections
    #-nostdlib
    -Wall
    --param max-inline-insns-single=500
    # Defines
    -DF_CPU=64000000
    -DARDUINO=10607
    -DARDUINO_BBC_MICROBIT_V2
    -DARDUINO_ARCH_NRF5
    -DNRF52833_XXAA
    -DNRF5
    -DUSE_LFSYNT
)

set(CFLAGS ${COMMON_FLAGS}
    -std=gnu11
)
list(JOIN CFLAGS " " CMAKE_C_FLAGS)

set(CXXFLAGS ${COMMON_FLAGS}
    -fno-rtti
    -fno-exceptions
    -std=gnu++11
    -fno-threadsafe-statics
)
list(JOIN CXXFLAGS " " CMAKE_CXX_FLAGS)

SET(ASM_OPTIONS
    -x assembler-with-cpp
    ${CFLAGS}
)

list(JOIN ASM_OPTIONS " " CMAKE_ASM_FLAGS)

set(LINKER_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -Os
    -L${CMAKE_SOURCE_DIR}/ThirdParty/arduino-nRF5/cores/nRF5/SDK/components/toolchain/gcc/
    -T${CMAKE_SOURCE_DIR}/ThirdParty/arduino-nRF5/cores/nRF5/SDK/components/toolchain/gcc/nrf52833_xxaa.ld
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--cref
    -Wl,--check-sections
    -Wl,--gc-sections
    -Wl,--unresolved-symbols=report-all
    -Wl,--warn-common
    -Wl,--warn-section-align
    -lm
)
list(JOIN LINKER_FLAGS " " CMAKE_EXE_LINKER_FLAGS)

function(microbit_executable TARGET)
    target_link_libraries(${TARGET} PUBLIC ThirdParty::arduino-nRF5)
    set_target_properties(${TARGET} PROPERTIES SUFFIX "" ENABLE_EXPORTS ON)
    target_link_options(${TARGET} PRIVATE
        -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.map
    )

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> $<TARGET_FILE:${TARGET}>.bin
    VERBATIM)

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET}> $<TARGET_FILE:${TARGET}>.hex
    VERBATIM)
endfunction()
