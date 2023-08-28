add_executable(Simulator
    src/Main.cpp
    src/Window.cpp
    src/Window.h
)

target_link_libraries(Simulator ThirdParty::fmt ThirdParty::imgui)
