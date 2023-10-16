#include <filesystem>
#include <stdexcept>

#include <fmt/format.h>

#include "../Application.h"
#include "MainWindow.h"
#include "Simulation.h"

namespace Simulator::Services
{

// SDL3/imgui window intialisation (mostly derived from the official example:
// https://github.com/ocornut/imgui/blob/master/examples/example_SDL3_opengl2/main.cpp)
MainWindow::MainWindow(Application *application, std::string executable) : application{application}
{
#ifdef LINUX
    setenv("SDL_VIDEODRIVER", "x11", 1);
#endif
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
    {
        throw std::runtime_error(fmt::format("Error initialising SDL: {}", SDL_GetError()));
    }

    // Create window
    SDL_WindowFlags window_flags{(SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED)};
    window = SDL_CreateWindowWithPosition("MicroBros Simulator", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 1600, 900, window_flags);
    if (window == nullptr)
        throw std::runtime_error(fmt::format("Error creating window: {}", SDL_GetError()));

    // Setup renderer
    renderer =
        SDL_CreateRenderer(window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
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

    // Load mouse sprite
    std::filesystem::path mouse_sprite_path{executable};
    mouse_sprite_path.replace_filename("robot.png");
    mouse_sprite = std::make_unique<Utils::Texture>(renderer, mouse_sprite_path.string());

    // Init OpenGL backend for imgui
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void MainWindow::Tick()
{
    ImGuiIO &io = ImGui::GetIO();

    // Poll events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT) // || (event.type == SDL_EVENT_WINDOW &&
                                          // event.window.event == SDL_EVENT_WINDOW_CLOSE &&
                                          // event.window.windowID == SDL_GetWindowID(window)))
            application->Quit();
    }

    // Draw ImGui
    Draw();
} // namespace Simulator

void MainWindow::Draw()
{
    ImGuiIO &io = ImGui::GetIO();

    // Start frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    DrawMenuBar();

    // Error dialog
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::PushOverrideID(error_popup);
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (!application->errors.empty())
            ImGui::TextUnformatted(application->errors.front().c_str());
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            application->errors.pop();
            if (application->errors.empty())
                ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    // Force-open popup on errors
    if (!application->errors.empty())
        ImGui::OpenPopup("Error");
    ImGui::PopID();

    // Draw Windows
    for (const auto &[_, window] : application->windows)
    {
        if (window->IsOpen())
            window->Draw();
    }

    // Render
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

void MainWindow::DrawMenuBar()
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open Maze"))
            application->GetService<Simulation>()->OpenMaze();
        ImGui::Separator();
        if (ImGui::MenuItem("Quit"))
            application->Quit();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Remote"))
    {
        ImGui::MenuItem("Connect to device...", NULL,
                        &application->GetWindow(Windows::WindowId::RemoteConnections)->GetOpen());
        ImGui::MenuItem("Remote motor control", NULL,
                        &application->GetWindow(Windows::WindowId::RemoteMotors)->GetOpen());
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

MainWindow::~MainWindow()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
} // namespace Simulator::Services
