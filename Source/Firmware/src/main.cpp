#include <MicroBit.h>

#include "Drivers/DFR0548.h"

MicroBit uBit;

int main()
{
    // Creae the DFR0548 motor driver
    Firmware::Drivers::DFR0548 dfr0548{uBit.i2c};

    while (1)
    {

        dfr0548.SetMotors(4095, 4095, 4095, 4095);
        uBit.sleep(2000);

        dfr0548.SetMotors(-4095, -4095, -4095, -4095);
        uBit.sleep(2000);

        // Left-back
        dfr0548.SetMotors(4095, 0, 0, 0);
        uBit.sleep(2000);

        // Left-front
        dfr0548.SetMotors(0, 4095, 0, 0);
        uBit.sleep(2000);

        // Right-back
        dfr0548.SetMotors(0, 0, 4095, 0);
        uBit.sleep(2000);

        // Right-front
        dfr0548.SetMotors(0, 0, 0, 4095);
        uBit.sleep(2000);

        // Stop the motors for 8 sec
        dfr0548.SetMotors(0, 0, 0, 0);
        uBit.sleep(8000);
    }

    return 0;
}
