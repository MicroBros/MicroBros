add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_library(ThirdParty::fmt ALIAS fmt)

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
