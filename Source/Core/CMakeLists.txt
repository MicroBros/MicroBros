add_library(Core STATIC
    src/Maze.cpp
    src/Mouse.cpp
)
target_include_directories(Core PUBLIC include/)

target_link_libraries(Core
    ThirdParty::fmt
)
