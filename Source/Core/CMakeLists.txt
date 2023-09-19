add_library(Core OBJECT
    src/Algorithm.cpp
    src/Maze.cpp
    src/Mouse.cpp
    # Algorithms
    src/Algorithms/WallFollower.cpp
    src/Algorithms/WallFollower2.cpp
)
target_include_directories(Core PUBLIC include/)

target_link_libraries(Core
    ThirdParty::fmt
)
