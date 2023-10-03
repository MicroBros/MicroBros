#pragma once

#include <MicroBit.h>

#include "../Drivers/DFR0548.h"

namespace Firmware::BLE
{

/*! \brief BLE Service for manually controlling the robot
 *
 * Provides the ability to write the value of high-level forward, sideways or rotation movement and
 * motor values directly.
 */
class ControllerService : public MicroBitBLEService
{
public:
    ControllerService(Drivers::DFR0548 &driver);

protected:
    inline int characteristicCount() { return 0; };
    inline MicroBitBLEChar *characteristicPtr(int idx) { return nullptr; };

private:
    Drivers::DFR0548 &driver;
};

}; // namespace Firmware::BLE
