#include <fmt/format.h>
#include <nfd.h>
#include <iostream>

#include "Window.h"

// sdl2/imgui window intialisation (mostly derived from the official example: https://github.com/ocornut/imgui/blob/master/examples/example_sdl2_opengl2/main.cpp)
Window::Window()
{
#ifdef LINUX
    setenv("SDL_VIDEODRIVER", "x11", 1);
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        throw std::runtime_error(fmt::format("Error initialising SDL: {}", SDL_GetError()));
    }

    // Setup GL attributes for imgui
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create window
    SDL_WindowFlags window_flags{(SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)};
    window = SDL_CreateWindow("MicroBros Simulator", 0, 0, 1280, 720, window_flags);
    if (window == nullptr)
        throw std::runtime_error(fmt::format("Error creating window: {}", SDL_GetError()));

    // Setup OpenGL context
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
        throw std::runtime_error(fmt::format("Error creating gl context: {}", SDL_GetError()));

    SDL_GL_MakeCurrent(window, gl_context);
    // Setup vsync
    SDL_GL_SetSwapInterval(1);

    // Setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    // Init OpenGL backend for imgui
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    bool done{false};
    while (!done)
    {
        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)))
                done = true;
        }

        // Start frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        DrawMenuBar(done);

        // Error dialog
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::PushOverrideID(error_popup);
        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (error.has_value())
                ImGui::TextUnformatted(error.value().c_str());
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                error = std::nullopt;
            }

            ImGui::EndPopup();
        }
        ImGui::PopID();

        // Test window
        ImGui::Begin("Hello, world!");
        ImGui::End();

        // Render
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        // glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
}

void Window::DrawMenuBar(bool &done)
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open Maze"))
            OpenMaze();
        ImGui::Separator();
        if (ImGui::MenuItem("Quit"))
            done = true;
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void Window::OpenMaze()
{
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = {{"Maze files", "txt"}};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);

    if (result == NFD_OKAY)
    {
        OpenMaze(std::string(outPath));
        NFD_FreePath(outPath);
    }
    else if (result != NFD_CANCEL)
    {
        auto err{fmt::format("{}", NFD_GetError())};
        Error(err);
    }
}

void Window::OpenMaze(std::string path)
{
    std::cout << path << std::endl;
    Error(path);
}

void Window::Error(std::string err)
{
    error = err;
    ImGui::PushOverrideID(error_popup);
    ImGui::OpenPopup("Error");
    ImGui::PopID();
}

Window::~Window()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
