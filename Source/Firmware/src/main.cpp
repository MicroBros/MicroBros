#include <MicroBit.h>
#include <memory>

#include <Core/Log.h>

#include "BLE/MotorService.h"
#include "BLE/MouseService.h"
#include "Drivers/DFR0548.h"
#include "Mouse2.h"

// TEMP
#include "Drivers/HCSR04.h"
#include "Drivers/IR.h"
MicroBit uBit;

int main()
{
    uBit.init();

    // Create the DFR0548 motor driver
    auto dfr0548{std::make_unique<Firmware::Drivers::DFR0548>(uBit, uBit.i2c, false)};
    // Firmware::Mouse mouse(uBit, dfr0548);

    // Create mouse impl
    auto mouse{std::make_unique<Firmware::Mouse2>(uBit, dfr0548.get())};

    // Setup BLE services
    // auto motor_service{std::make_unique<Firmware::BLE::MotorService>(dfr0548.get())};
    // auto mouse_service{std::make_unique<Firmware::BLE::MouseService>(mouse.get())};

    LOG_INFO("Initialised MicroMouse!");

    // Used for button A toggle
    // bool last_pressed{false};

    CODAL_TIMESTAMP prev_time{uBit.timer.getTime()};
    while (1)
    {
        CODAL_TIMESTAMP now{uBit.timer.getTime()};

        // Run mouse
        mouse->Run(now, now - prev_time);

        prev_time = now;

        // Give at least 1ms
        fiber_sleep(1);
    }

    /*
        while (1)
        {
            if (mouse->IsRunning() || mouse->IsMoving())
            {
                mouse->Run();
            }

            // Send update over BLE
            mouse_service->Update();

            // Simple toggle of running by pressing A
            if (!last_pressed && uBit.buttonA.isPressed())
            {
                mouse->SetRunning(!mouse->IsRunning());
                last_pressed = true;
            }
            else
                last_pressed = false;
        }
    */

    /*while (1)
    {
        mouse.Run();

        dfr0548.SetMotors(-BASE_SPEED, -BASE_SPEED, -BASE_SPEED, -BASE_SPEED);
        uBit.sleep(2000);

        dfr0548.StopMotors();
        uBit.sleep(100);

        dfr0548.SetMotors(BASE_SPEED, BASE_SPEED, BASE_SPEED, BASE_SPEED);
        uBit.sleep(2000);

        // Left-back
        dfr0548.SetMotors(BASE_SPEED, 0, 0, 0);
        uBit.sleep(2000);

        // Left-front
        dfr0548.SetMotors(0, BASE_SPEED, 0, 0);
        uBit.sleep(2000);

        // Right-back
        dfr0548.SetMotors(0, 0, BASE_SPEED, 0);
        uBit.sleep(2000);

        // Right-front
        dfr0548.SetMotors(0, 0, 0, BASE_SPEED);
        uBit.sleep(2000);

        // Stop the motors for 8 sec
        dfr0548.SetMotors(0, 0, 0, 0);
        uBit.sleep(8000);

    }*/

    return 0;
}

/* Stubs to make linker stop throwing warnings for missing irrelevant functions */
extern "C" void _fstat(void) {}
extern "C" void _isatty(void) {}
