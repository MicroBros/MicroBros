#include <cmath>

#include <imgui.h>

#include <Core/Comm.h>

#include "../Application.h"
#include "../Services/BLE.h"
#include "Window.h"

using namespace Core;

namespace Simulator::Windows
{

//! UI for enabling remote controlling of motors
class RemoteMotors : public Window
{
public:
    RemoteMotors(Application *application) : application{application} {}

    WINDOW(RemoteMotors);

    void Draw()
    {
        auto ble{application->GetService<Services::BLE>()};

        ImGui::SetNextWindowSize(ImVec2(200.0f, 160.0f));
        ImGui::Begin("Remote motor control", NULL, ImGuiWindowFlags_NoResize);

        bool connected{ble->GetActive().has_value()};

        ImGui::Text("Status:");
        ImGui::SameLine();
        ImGui::Text(connected ? "Connected" : "Disconnected");

        ImGui::Checkbox("Enable", &enable);

        ImGui::Checkbox("Keyboard (WASD+QE)", &keyboard);
        ImGui::DragInt("Speed", &speed, 10, 100, 4095, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Checkbox("Gamepad", &gamepad);

        ImGui::End();
    }

    // This could technically have been a Service, but it will only be used here
    void Tick()
    {
        if (!enable)
            return;

        auto ble{application->GetService<Services::BLE>()};

        auto active_device{ble->GetActive()};
        if (!active_device.has_value())
            return;

        float forward{0};
        float right{0};
        float rot{0};

        // Gamepad input
        if (gamepad)
        {
            if (sdl_gamepad != nullptr)
            {

#define CONVERT_AXIS(OUT, AXIS)                                                                    \
    {                                                                                              \
        auto axis{SDL_GetGamepadAxis(sdl_gamepad, AXIS)};                                          \
        float normalised{axis > 0 ? axis / 32767.0f : axis / 32768.0f};                            \
        if (normalised > 0.1 || normalised < -0.1)                                                 \
            OUT = (normalised * 0.9) + 0.1;                                                        \
    }
                float backward{0};

                CONVERT_AXIS(forward, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
                CONVERT_AXIS(backward, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)
                CONVERT_AXIS(right, SDL_GAMEPAD_AXIS_RIGHTX)
                CONVERT_AXIS(rot, SDL_GAMEPAD_AXIS_LEFTX)

                forward -= backward;
            }
            else
            {
                int count{0};
                auto joysticks{SDL_GetJoysticks(&count)};
                if (count > 0)
                    sdl_gamepad = SDL_OpenGamepad(joysticks[0]);
                if (joysticks != nullptr)
                    SDL_free(joysticks);
            }
        }

        // Keyboard input
        if (keyboard)
        {
            int n;
            auto kb_state{SDL_GetKeyboardState(&n)};

            float speed{this->speed / 4095.0f};

            // Directions
            if (kb_state[SDL_SCANCODE_W])
                forward += speed;
            if (kb_state[SDL_SCANCODE_A])
                right -= speed;
            if (kb_state[SDL_SCANCODE_S])
                forward -= speed;
            if (kb_state[SDL_SCANCODE_D])
                right += speed;
            // Rotation
            if (kb_state[SDL_SCANCODE_Q])
                rot -= speed;
            if (kb_state[SDL_SCANCODE_E])
                rot += speed;
        }

        // Ensure everything stays within limits
        float denominator{std::max(std::abs(forward) + std::abs(right) + std::abs(rot), 1.0f)};

        BLE_STRUCTURE(MotorService, Motors)
        motors = {.rf = static_cast<int16_t>((forward - right - rot) / denominator * 4095.0f),
                  .rb = static_cast<int16_t>((forward + right - rot) / denominator * 4095.0f),
                  .lf = static_cast<int16_t>((forward + right + rot) / denominator * 4095.0f),
                  .lb = static_cast<int16_t>((forward - right + rot) / denominator * 4095.0f)};

        std::string data{std::string((char *)&motors, sizeof(BLE_STRUCTURE(MotorService, Motors)))};

        try
        {
            // Send it off :salute:
            active_device->write_command(MICROBIT_BLE_SERVICE_UUID(MotorService),
                                         MICROBIT_BLE_CHARACTERISTIC_UUID(MotorService, Motors),
                                         data);
        }
        catch (const std::exception &e)
        {
            application->Error(e.what());
            enable = false;
        }
    }

private:
    Application *application{nullptr};
    SDL_Gamepad *sdl_gamepad{nullptr};
    bool enable{false};
    bool keyboard{false};
    bool gamepad{true};
    int speed{2048};
};

REGISTER_WINDOW(RemoteMotors)

} // namespace Simulator::Windows
