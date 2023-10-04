#pragma once

#include <MicroBit.h>

#include <Core/Comm.h>

#include "../Drivers/DFR0548.h"

namespace Firmware::BLE
{

/*! \brief BLE Service for manually controlling the robot
 *
 * Provides the ability to write the value of motor values directly.
 */
class MotorService : public MicroBitBLEService
{
public:
    MotorService(Drivers::DFR0548 *driver);

    //! Callback for when BLE data has been written
    void onDataWritten(const microbit_ble_evt_write_t *params);

protected:
    inline int characteristicCount() { return CHARACTERISTICS_COUNT(MotorService); };
    inline MicroBitBLEChar *characteristicPtr(int idx) { return &chars[idx]; };

private:
    MicroBitBLEChar chars[CHARACTERISTICS_COUNT(MotorService)];
    Drivers::DFR0548 *driver;
    BLE_STRUCTURE(MotorService, Motors) motors;
};

}; // namespace Firmware::BLE
