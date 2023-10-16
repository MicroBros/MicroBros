#include <cmath>

#include <imgui.h>

#include "../Application.h"
#include "../Services/BLE.h"
#include "Window.h"

using namespace Core;

namespace Simulator::Windows
{

//! Manage remote BLE connections
class RemoteConnections : public Window
{
public:
    RemoteConnections(Application *application) : application{application} {}

    WINDOW(RemoteConnections);

    void Draw()
    {
        auto ble{application->GetService<Services::BLE>()};

        ImGui::SetNextWindowSize(ImVec2(500.0f, 220.0f));
        ImGui::Begin("Remote connections", NULL, ImGuiWindowFlags_NoResize);

        if (ImGui::Button("Scan"))
        {
            try
            {
                ble->Scan();
            }
            catch (const std::exception &e)
            {
                application->Error(e.what());
            }
        }
        ImGui::SameLine();

        ImGui::Checkbox("Auto-scan", &ble->Autoscan());
        if (ImGui::BeginTable("ble_devices", 3))
        {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Identifier", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();

            for (size_t i{0}; i < ble->Peripherals().size(); i++)
            {
                auto &peripheral{ble->Peripherals()[i]};
                // Peripheral row
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", peripheral.address().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", peripheral.identifier().c_str());
                ImGui::TableNextColumn();

                // Disconnect
                if (peripheral.is_connected())
                {
                    if (ImGui::Button("Disconnect"))
                    {
                        try
                        {
                            ble->Disconnect(peripheral);
                        }
                        catch (const std::exception &e)
                        {
                            application->Error(e.what());
                        }
                    }
                    // Set the BLE device as active if not
                    if (!ble->IsActive(peripheral))
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Set Active"))
                            ble->SetActive(peripheral);
                    }
                }
                // Non-connectable (greyed-out connect)
                else if (!peripheral.is_connectable())
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::Button("Connect");
                    ImGui::PopItemFlag();
                }
                // Connect
                else
                {
                    if (ImGui::Button("Connect"))
                    {
                        try
                        {
                            ble->Connect(peripheral);
                        }
                        catch (const std::exception &e)
                        {
                            application->Error(e.what());
                        }
                    }
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }

private:
    Application *application{nullptr};
};

REGISTER_WINDOW(RemoteConnections)

} // namespace Simulator::Windows
