#include <fmt/format.h>
#include <nfd.h>
#include <iostream>
#include <filesystem>

#include "Window.h"

// SDL3/imgui window intialisation (mostly derived from the official example: https://github.com/ocornut/imgui/blob/master/examples/example_SDL3_opengl2/main.cpp)
Window::Window(std::string executable)
{
#ifdef LINUX
    setenv("SDL_VIDEODRIVER", "x11", 1);
#endif
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error(fmt::format("Error initialising SDL: {}", SDL_GetError()));
    }

    // Create window
    SDL_WindowFlags window_flags{(SDL_WindowFlags)(SDL_WINDOW_RESIZABLE)};
    window = SDL_CreateWindowWithPosition("MicroBros Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, window_flags);
    if (window == nullptr)
        throw std::runtime_error(fmt::format("Error creating window: {}", SDL_GetError()));

    // Setup renderer
    renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
        throw std::runtime_error(fmt::format("Error creating SDL renderer: {}", SDL_GetError()));

    // Setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // Setup Roboto as font
    std::filesystem::path ttf_path{executable};
    ttf_path.replace_filename("Roboto-Medium.ttf");
    io.Fonts->AddFontFromFileTTF(ttf_path.string().c_str(), 16.0f);
    ImGui::StyleColorsDark();

    // Init OpenGL backend for imgui
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

// Main event-loop
void Window::Run()
{
    bool done{false};
    while (!done)
    {
        ImGuiIO &io = ImGui::GetIO();

        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) // || (event.type == SDL_EVENT_WINDOW && event.window.event == SDL_EVENT_WINDOW_CLOSE && event.window.windowID == SDL_GetWindowID(window)))
                done = true;
        }

        // Draw ImGui
        Draw(done);
    }
}

void Window::Draw(bool &done)
{
    ImGuiIO &io = ImGui::GetIO();

    // Start frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    DrawMenuBar(done);

    // Error dialog
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Ensure error_setup is ran within an ImGui frame
    if (error_setup)
    {
        error_setup.value()();
        error_setup = std::nullopt;
    }

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

    // Maze window
    if (maze)
    {
        DrawMazeWindow();
    }

    // Render
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
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

void Window::DrawMazeWindow()
{
    auto size{ImVec2(800.0f, 800.0f)};
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Maze", NULL, ImGuiWindowFlags_NoResize);
    maze->Draw();
    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(200.0f, 600.0f));
    ImGui::Begin("Simulator", NULL, ImGuiWindowFlags_NoResize);

    // Simulation
    ImGui::SeparatorText("Simulation");
    if (ImGui::Button("Step")) // F10
        maze->Step();
    if (ImGui::Button("Reset")) // Shift+F5
        maze->Reset();

    // Run
    ImGui::SeparatorText("Run");
    if (ImGui::Button(maze->IsRunning() ? "Pause" : "Start")) // F6 : F5
        maze->ToggleRunning();
    ImGui::DragFloat("Cycles/s", &maze->Speed(), 0.1f, 0.0f, 20.0f);

    ImGui::End();
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
    try
    {
        maze = std::make_unique<SimulatorMaze>(path);
    }
    catch (const std::exception &e)
    {
        Error(e.what());
    }
}

void Window::Error(std::string err)
{
    error = err;
    // These has to be ran within an ImGui frame
    error_setup = [this]()
    {
        ImGui::PushOverrideID(error_popup);
        ImGui::OpenPopup("Error");
        ImGui::PopID();
    };
}

Window::~Window()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
