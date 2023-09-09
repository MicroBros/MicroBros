add_executable(Simulator
    src/Main.cpp
    src/SimulatorMaze.cpp src/SimulatorMaze.h
    src/Window.cpp src/Window.h
)

target_link_libraries(Simulator Core ThirdParty::fmt ThirdParty::imgui ThirdParty::nfd)
