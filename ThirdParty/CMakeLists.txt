# fmt
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_library(ThirdParty::fmt ALIAS fmt)

if(FIRMWARE)
    # arduino-nRF5
    file (GLOB SOURCE_FILES
        "arduino-nRF5/cores/nRF5/*.cpp"
        "arduino-nRF5/cores/nRF5/*.c"
        "arduino-nRF5/cores/nRF5/SDK/components/toolchain/*.c"
    )

    set(nRF5-variant BBCmicrobitV2)

    add_library(arduino-nRF5 STATIC
        ${SOURCE_FILES}
        arduino-nRF5/variants/${nRF5-variant}/variant.cpp
    )
    target_include_directories(arduino-nRF5 PUBLIC
        arduino-nRF5/cores/nRF5
        arduino-nRF5/cores/nRF5/SDK/components/drivers_nrf/delay
        arduino-nRF5/cores/nRF5/SDK/components/device
        arduino-nRF5/cores/nRF5/SDK/components/toolchain
        arduino-nRF5/cores/nRF5/SDK/components/toolchain/CMSIS/Include
        arduino-nRF5/variants/${nRF5-variant}
    )
    target_compile_definitions(arduino-nRF5 PUBLIC
        -DNRF52833_XXAA
        -DARDUINO=10805
        -DARDUINO_ARCH_NRF5
        -DNRF5
        -DNRF52
    )
    add_library(ThirdParty::arduino-nRF5 ALIAS arduino-nRF5)
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
        imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_sdl2.h
        imgui/backends/imgui_impl_opengl2.cpp imgui/backends/imgui_impl_opengl2.h
    )
    target_include_directories(imgui PUBLIC imgui imgui/backends)
    target_link_libraries(imgui PUBLIC ThirdParty::SDL2)

    if(UNIX)
        target_link_libraries(imgui PUBLIC GL)
    else()
        target_link_libraries(imgui PUBLIC opengl32.lib)
    endif()

    add_library(ThirdParty::imgui ALIAS imgui)

    # SDL2
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
    add_library(ThirdParty::SDL2 ALIAS SDL2)
endif()
