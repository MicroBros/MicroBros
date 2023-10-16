#include <cmath>

#include <imgui.h>

#include "../Application.h"
#include "../Services/Simulation.h"
#include "Window.h"

using namespace Core;

namespace Simulator::Windows
{

//! \brief Various controls for SimulatorMouse
class Controls : public Window
{
public:
    Controls(Application *application) : application{application} { SetOpen(true); }

    WINDOW(Controls);

    void Draw()
    {
        auto simulator_mouse{application->GetSimulatorMouse()};
        if (!simulator_mouse || !simulator_mouse->GetMouse())
            return;

        ImGui::SetNextWindowSize(ImVec2(250.0f, 600.0f));
        ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize);

        // Mouse
        ImGui::SeparatorText("Mouse");
        if (ImGui::Button(simulator_mouse->IsRunning() ? "Pause" : "Start")) // F6 : F5
            simulator_mouse->SetRunning(!simulator_mouse->IsRunning());
        ImGui::SameLine();
        if (ImGui::Button("Step")) // F10
        {
            simulator_mouse->SetRunning(false); // Stop running is manually stepping
            simulator_mouse->Step();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) // Shift+F5
            simulator_mouse->Reset();
        ImGui::SameLine();
        ImGui::Checkbox("Return", &simulator_mouse->GetMouse()
                                       ->ReturnStart()); // TODO: Ensure this also works over remote

        auto algorithms{simulator_mouse->GetAlgorithms()};
        if (!algorithms.empty())
        {
            size_t current_algorithm{simulator_mouse->GetAlgorithm()};
            if (ImGui::BeginCombo("Algorithm", algorithms[current_algorithm].c_str(),
                                  ImGuiComboFlags_None))
            {
                for (size_t i{0}; i < algorithms.size(); i++)
                {
                    bool selected{i == current_algorithm};
                    if (ImGui::Selectable(algorithms[i].c_str(), selected))
                    {
                        simulator_mouse->SetAlgorithm(i);
                    }
                }
                ImGui::EndCombo();
            }
        }

        // Simulation
        if (simulator_mouse->IsSimulation())
        {
            auto simulation{dynamic_cast<Services::Simulation *>(simulator_mouse)};

            ImGui::SeparatorText("Simulation");
            ImGui::DragFloat("Cycles/s", &simulation->speed, 0.1f, 0.0f, 20.0f);
        }

        ImGui::End();
    }

private:
    Application *application{nullptr};
};

REGISTER_WINDOW(Controls)

} // namespace Simulator::Windows
