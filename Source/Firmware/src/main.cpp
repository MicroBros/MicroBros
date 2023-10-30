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
    auto dfr0548{std::make_unique<Firmware::Drivers::DFR0548>(uBit, uBit.i2c)};
    // Firmware::Mouse mouse(uBit, dfr0548);

    // Create mouse impl
    // auto mouse{std::make_unique<Firmware::Mouse2>(uBit, dfr0548.get())};

    // Setup BLE services
    // auto motor_service{std::make_unique<Firmware::BLE::MotorService>(dfr0548.get())};
    // auto mouse_service{std::make_unique<Firmware::BLE::MouseService>(mouse.get())};

    LOG_INFO("Initialised MicroMouse!");

    // Used for button A toggle
    // bool last_pressed{false};

    static float f;
    static float b;

    std::vector<Firmware::Drivers::HCSR04::Sensor> hcsr04_pins = {
        {.echo_pin = uBit.io.P13, .trig_pin = uBit.io.P14, .value = &f},
        {.echo_pin = uBit.io.P15, .trig_pin = uBit.io.P16, .value = &b}};

    auto hcsr04{std::make_unique<Firmware::Drivers::HCSR04>(hcsr04_pins)};

    static float l;
    static float r;

    std::vector<Firmware::Drivers::IR::Sensor> ir_pins = {
        {.sense_pin = uBit.io.P1, .value = &l, .base = 360, .scale = 0.01f, .exp = 1.181f},
        {.sense_pin = uBit.io.P2, .value = &r, .base = 360, .scale = 0.01f, .exp = 1.181f}};

    auto ir{std::make_unique<Firmware::Drivers::IR>(ir_pins, uBit.io.P0)};

    while (1)
    {
        auto start{uBit.timer.getTime()};
        ir->RunSignalProcessing();
        LOG_INFO("front(US)={:.1f}cm, back(US)={:.1f}cm, left(IR)={:.1f}cm, right(IR)={:.1f}cm (IR "
                 "DSP time={}ms)",
                 f, b, l, r, uBit.timer.getTime() - start);
        fiber_sleep(100);
        // mouse->Run();
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
