# fmt
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_library(ThirdParty::fmt ALIAS fmt)

if(FIRMWARE)
    include(codal/utils/cmake/toolchains/ARM_GCC/compiler-flags.cmake)

    # codal-core
    file(GLOB_RECURSE SOURCE_FILES
        "codal-core/source/*.cpp"
    )
    add_library(codal-core STATIC
        ${SOURCE_FILES}
    )
    target_include_directories(codal-core PUBLIC
        .
        codal/utils/cmake/toolchains/ARM_GCC
        codal-core/inc/core
        codal-core/inc/driver-models
        codal-core/inc/drivers
        codal-core/inc/streams
        codal-core/inc/types
    )
    add_subdirectory(codal-microbit-nrf5sdk)

    # codal-nrf52
    file(GLOB_RECURSE SOURCE_FILES
        "codal-nrf52/source/*.cpp"
    )
    add_library(codal-nrf52 STATIC
        ${SOURCE_FILES}
        codal-nrf52/asm/CortexContextSwitch.s
        codal-nrf52/nrfx/mdk/system_nrf52833.c
        codal-nrf52/nrfx/mdk/gcc_startup_nrf52833.S
    )
    target_compile_definitions(codal-nrf52 PUBLIC -DNRFX_PRS_ENABLED=0)
    target_include_directories(codal-nrf52 PUBLIC
        codal-nrf52/inc
        codal-nrf52/inc
        codal-nrf52/inc/cmsis
        codal-nrf52/nrfx/mdk
        codal-nrf52/nrfx
        codal-nrf52/nrfx/templates/
        codal-nrf52/nrfx/templates/nRF52833
        codal-nrf52/nrfx/drivers/include
        codal-nrf52/nrfx/drivers/src
    )
    target_link_libraries(codal-nrf52 PUBLIC codal-core codal-microbit-nrf5sdk)

    # codal-microbit-v2
    file(GLOB_RECURSE SOURCE_FILES
        "codal-microbit-v2/source/*.cpp"
        "codal-microbit-v2/model/*.cpp"
    )
    add_library(codal-microbit-v2 STATIC
        ${SOURCE_FILES}
    )
    target_include_directories(codal-microbit-v2 PUBLIC
        codal-microbit-v2/inc
        codal-microbit-v2/inc/bluetooth
        codal-microbit-v2/inc/compat
        codal-microbit-v2/model
    )
    target_link_libraries(codal-microbit-v2 PUBLIC codal-core codal-nrf52)
    add_library(ThirdParty::codal-microbit-v2 ALIAS codal-microbit-v2)
else()
    # imgui
    #find_package(OpenGL REQUIRED)
    add_library(imgui STATIC
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_sdl3.cpp imgui/backends/imgui_impl_sdl3.h
        imgui/backends/imgui_impl_sdlrenderer3.cpp imgui/backends/imgui_impl_sdlrenderer3.h
    )
    target_include_directories(imgui PUBLIC imgui imgui/backends)
    target_link_libraries(imgui PUBLIC ThirdParty::SDL3)
    target_compile_definitions(imgui PUBLIC -DIMGUI_DEFINE_MATH_OPERATORS)

    if(UNIX)
        target_link_libraries(imgui PUBLIC GL)
    else()
        target_link_libraries(imgui PUBLIC opengl32.lib)
    endif()

    add_library(ThirdParty::imgui ALIAS imgui)

    # nativefiledialog
    add_subdirectory(nativefiledialog-extended)
    add_library(ThirdParty::nfd ALIAS nfd)

    # SDL3
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
    add_library(ThirdParty::SDL3 ALIAS SDL3-static)

    # stb
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE stb)
    add_library(ThirdParty::stb ALIAS stb)
endif()
