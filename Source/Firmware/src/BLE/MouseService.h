#pragma once

#include <MicroBit.h>

#include <Core/Comm.h>

#include "../Mouse2.h"

#define MAX_ALGORITHM_NAME 20

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
    //! Callback for when BLE data is being read
    void onDataRead(microbit_onDataRead_t *params);

    //! Update the values of the mouse
    void Update();

    //! Run a notify on Maze
    void MazeUpdate();

protected:
    inline int characteristicCount() { return CHARACTERISTICS_COUNT(MouseService); };
    inline MicroBitBLEChar *characteristicPtr(int idx) { return &chars[idx]; };

private:
    void UpdateTiles();

    MicroBitBLEChar chars[CHARACTERISTICS_COUNT(MouseService)];
    Mouse2 *mouse;

    BLE_STRUCTURE(MouseService, MouseControl) control;
    BLE_STRUCTURE(MouseService, MousePosition) position;
    BLE_STRUCTURE(MouseService, AlgorithmCount) algorithm_index;
    std::array<Core::MazeTile::ValueType, 16 * 16> tile_values{};
    std::array<char, MAX_ALGORITHM_NAME + 1> algorithm_name_buffer;
    int last_iter{0};
};

}; // namespace Firmware::BLE
