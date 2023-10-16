add_executable(Simulator
    # Services
    src/Services/BLE.cpp src/Services/BLE.h
    src/Services/MainWindow.cpp src/Services/MainWindow.h
    src/Services/Service.h
    src/Services/Simulation.cpp src/Services/Simulation.h
    # Windows
    src/Windows/Controls.cpp
    src/Windows/Maze.cpp
    src/Windows/RemoteConnections.cpp
    src/Windows/RemoteMotors.cpp
    src/Windows/Window.cpp src/Windows/Window.h
    # Misc
    src/Application.cpp src/Application.h
    src/Main.cpp
    src/SimulatorMouse.h
    src/Utils.cpp src/Utils.h
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
