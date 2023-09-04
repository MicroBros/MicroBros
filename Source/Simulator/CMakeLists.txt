add_executable(Simulator
    src/Main.cpp
    src/Window.cpp
    src/Window.h
)

target_link_libraries(Simulator Core ThirdParty::fmt ThirdParty::imgui ThirdParty::nfd)
