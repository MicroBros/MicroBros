# Setup system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
SET(CMAKE_CROSSCOMPILING 1)

set(PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
include(${PROJECT_ROOT}/ThirdParty/codal/utils/cmake/toolchains/ARM_GCC/compiler-flags.cmake)
string(REPLACE "-lm" "-lnosys -lm" CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")

# More sane debug options than upstream CODAL
set(CMAKE_C_FLAGS_DEBUG_INIT "-g -Os -gdwarf-4")
set(CMAKE_CXX_FLAGS_DEBUG_INIT ${CMAKE_C_FLAGS_DEBUG_INIT})
set(CMAKE_ASM_FLAGS_DEBUG_INIT ${CMAKE_C_FLAGS_DEBUG_INIT})

find_program(ARM_NONE_EABI_AR arm-none-eabi-ar)
find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc)
find_program(ARM_NONE_EABI_GXX arm-none-eabi-g++)
find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy)
find_program(ARM_NONE_EABI_RANLIB arm-none-eabi-ranlib)
find_program(ARM_NONE_EABI_SIZE arm-none-eabi-size)

# Setup toolchain and target compilers
set(TOOLCHAIN arm-none-eabi)
set(CMAKE_ASM_COMPILER ${ARM_NONE_EABI_GCC} CACHE INTERNAL "ASM Compiler")
set(CMAKE_C_COMPILER ${ARM_NONE_EABI_GCC} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${ARM_NONE_EABI_GXX} CACHE INTERNAL "C++ Compiler")
set(CMAKE_OBJCOPY ${ARM_NONE_EABI_OBJCOPY})

# Setup toolchain flags
set(COMMON_FLAGS
    -Wno-expansion-to-defined
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=softfp
    -mfpu=fpv4-sp-d16
    -ffunction-sections
    -fdata-sections
    #-nostdlib
    -Wall
    --param max-inline-insns-single=500
    # Defines
    -DMBED_CONF_NORDIC_NRF_LF_CLOCK_SRC=NRF_LF_SRC_XTAL
    -DCONFIG_GPIO_AS_PINRESET=1
    -DCONFIG_NFCT_PINS_AS_GPIOS=1
    -DDEVICE_SRAM_BASE=0x20000000
    -DDEVICE_SRAM_END=0x20020000
    -DDEVICE_STACK_BASE=DEVICE_SRAM_END
    -DDEVICE_STACK_SIZE=2048
    -DDEVICE_HEAP_ALLOCATOR=1
    -DDEVICE_TAG=0
    -DSCHEDULER_TICK_PERIOD_US=4000
    -DEVENT_LISTENER_DEFAULT_FLAGS=MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY
    -DMESSAGE_BUS_LISTENER_MAX_QUEUE_DEPTH=10
    -DUSE_ACCEL_LSB=0
    -DDEVICE_DEFAULT_SERIAL_MODE=SYNC_SLEEP
    -DDEVICE_COMPONENT_COUNT=60
    -DDEVICE_DEFAULT_PULLMODE="PullMode::Down"
    -DDEVICE_PANIC_HEAP_FULL=1
    -DDMESG_SERIAL_DEBUG=0
    -DCODAL_DEBUG=CODAL_DEBUG_DISABLED
    -DDEVICE_USB=0
    -DCODAL_TIMESTAMP=uint64_t
    -DPROCESSOR_WORD_TYPE=uint32_t
    -DDEVICE_I2C_IRQ_SHARED=1
    -DCODAL_PROVIDE_PRINTF=1
    -DNRF52ADC_SOFTWARE_OVERSAMPLING=1
    -DNRF52I2C_ERRATA_219=1
    -DTOUCH_BUTTON_SENSITIVITY=5
    -DTOUCH_BUTTON_CALIBRATION_PERIOD=500
    -DTOUCH_BUTTON_DECAY_AVERAGE=2
    -DLED_MATRIX_MAXIMUM_BRIGHTNESS=245
    -DLED_MATRIX_MINIMUM_BRIGHTNESS=1
    -DCAPTOUCH_DEFAULT_CALIBRATION=3500
    -DHARDWARE_NEOPIXEL=1
    -DCODAL_TIMER_32BIT=1
    -DAPP_TIMER_V2
    -DAPP_TIMER_V2_RTC1_ENABLED
    -DNRF_DFU_TRANSPORT_BLE=1
    -DDEVICE_BLE=1
    -DMICROBIT_BLE_ENABLED=1
    -DMICROBIT_BLE_PAIRING_MODE=1
    -DMICROBIT_BLE_DFU_SERVICE=0
    -DMICROBIT_BLE_PARTIAL_FLASHING=0
    -DMICROBIT_BLE_SECURITY_LEVEL=SECURITY_MODE_ENCRYPTION_NO_MITM
    #-DDMESG_SERIAL_DEBUG=1
    #-DMICROBIT_DMESG_LEVEL=MICROBIT_DMESG_LEVEL_DEBUG
    -DNRF52833_XXAA
    -DNRF52833
    -DTARGET_MCU_NRF52833
    -DNRF5
    -DNRF52833
    -D__CORTEX_M4
    -DS113
    -DTOOLCHAIN_GCC
    -D__START=target_start
    -DSOFTDEVICE_PRESENT
)

set(CFLAGS ${COMMON_FLAGS}
    -std=c99
    -Warray-bounds
)
list(JOIN CFLAGS " " CFLAGS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")

set(CXXFLAGS ${COMMON_FLAGS}
    -fwrapv
    -fno-rtti
    -fno-threadsafe-statics
    -fno-exceptions
    -fno-unwind-tables
    -Wl,--gc-sections
    -Wl,--sort-common
    -Wl,--sort-section=alignment
    -Wno-array-bounds
    -Wno-register
)
list(JOIN CXXFLAGS " " CXXFLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS}")

SET(ASM_OPTIONS
    -fno-exceptions
    -fno-unwind-tables
    -mcpu=cortex-m4
    -mthumb
)

list(JOIN ASM_OPTIONS " " ASM_OPTIONS)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${ASM_OPTIONS}")

set(LINKER_FLAGS ${COMMON_FLAGS})
list(JOIN LINKER_FLAGS " " LINKER_FLAGS)

set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -T${PROJECT_ROOT}/ThirdParty/codal-microbit-v2/ld/nrf52833-softdevice.ld -Wl,--print-memory-usage -Wl,--no-warn-rwx-segments")

function(microbit_executable TARGET)
    target_link_libraries(${TARGET} PUBLIC ThirdParty::codal-microbit-v2)
    set_target_properties(${TARGET} PROPERTIES SUFFIX ".elf" ENABLE_EXPORTS ON)
    target_link_options(${TARGET} PRIVATE
        -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.map
    )

    # Print sizes used
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${ARM_NONE_EABI_SIZE} -G $<TARGET_FILE:${TARGET}>
    VERBATIM)

    # Unused
    #add_custom_command(TARGET ${TARGET} POST_BUILD
    #    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${TARGET}.bin
    #VERBATIM)
    #add_custom_command(TARGET ${TARGET} POST_BUILD
    #    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET}> ${TARGET}.hex
    #VERBATIM)
endfunction()
