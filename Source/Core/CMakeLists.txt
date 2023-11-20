add_library(Core OBJECT
    include/Core/Bitflags.h
    include/Core/Comm.h
    include/Core/Inline.h

    src/Algorithm.cpp include/Core/Algorithm.h
    src/Log.cpp include/Core/Log.h
    src/Maze.cpp include/Core/Maze.h
    src/Mouse.cpp include/Core/Mouse.h
    # Algorithms
    src/Algorithms/FloodFill.cpp src/Algorithms/FloodFill.h
    src/Algorithms/WallFollower.cpp
    src/Algorithms/WallFollower2.cpp
)
target_include_directories(Core PUBLIC include/)

target_link_libraries(Core
    ThirdParty::fmt
)

if(FIRMWARE)
    target_link_libraries(Core ThirdParty::codal-microbit-v2)
endif()
