add_executable(Simulator
    src/Main.cpp
    src/SimulatorMaze.cpp src/SimulatorMaze.h
    src/Utils.cpp src/Utils.h
    src/Window.cpp src/Window.h
)

target_link_libraries(Simulator
    Core
    ThirdParty::fmt
    ThirdParty::imgui
    ThirdParty::nfd
    ThirdParty::SimpleBLE
    ThirdParty::stb
)

# Copy assets to out
add_custom_command(TARGET Simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_SOURCE_DIR}/ThirdParty/imgui/misc/fonts/Roboto-Medium.ttf
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(TARGET Simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_SOURCE_DIR}/Data/robot.png
        ${CMAKE_CURRENT_BINARY_DIR}
)
