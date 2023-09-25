#include <MicroBit.h>

#include "Drivers/DFR0548.h"

MicroBit uBit;

int main()
{
    uBit.init();

    // Create the DFR0548 motor driver
    Firmware::Drivers::DFR0548 dfr0548{uBit, uBit.i2c};

    const int BASE_SPEED{2048};

    while (1)
    {
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
    }

    return 0;
}
