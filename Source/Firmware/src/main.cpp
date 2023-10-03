#include <MicroBit.h>

#include <Core/Log.h>

#include "BLE/ControllerService.h"
#include "Drivers/DFR0548.h"
#include "Mouse.h"

MicroBit uBit;

int main()
{
    uBit.init();
    // Turn on BLE advertising
    uBit.ble->init(ManagedString(microbit_friendly_name()), uBit.getSerial(), uBit.messageBus,
                   uBit.storage, false);
    uBit.ble->advertise();
    uBit.ble->setAdvertiseOnDisconnect(true);

    // Create the DFR0548 motor driver
    Firmware::Drivers::DFR0548 dfr0548{uBit, uBit.i2c};
    Firmware::Mouse mouse(uBit, dfr0548);

    // Setup BLE services
    Firmware::BLE::ControllerService controller_service(dfr0548);

    while (1)
    {
        mouse.Run();
        /*
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
        */
    }

    return 0;
}
