#pragma once

#include <MicroBit.h>

#include <Core/Comm.h>

#include "../Mouse2.h"

namespace Firmware::BLE
{

/*! \brief BLE Service for controlling and remote debugging the MicroMouse
 *
 * Includes functionality to:
 * - Start/stop
 * - Step manually
 * - Set algorithm
 * - Set speed factor
 * - Get tracked position
 * - Get maze data
 */
class MouseService : public MicroBitBLEService
{
public:
    MouseService(Mouse2 *mouse);

    //! Callback for when BLE data has been written
    void onDataWritten(const microbit_ble_evt_write_t *params);

    //! Update the values of the mouse
    void Update();

protected:
    inline int characteristicCount() { return CHARACTERISTICS_COUNT(MouseService); };
    inline MicroBitBLEChar *characteristicPtr(int idx) { return &chars[idx]; };

private:
    MicroBitBLEChar chars[CHARACTERISTICS_COUNT(MouseService)];
    Mouse2 *mouse;

    BLE_STRUCTURE(MouseService, MouseControl) control;
    BLE_STRUCTURE(MouseService, MousePosition) position;
};

}; // namespace Firmware::BLE
