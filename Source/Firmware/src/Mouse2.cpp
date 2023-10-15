#include "Mouse2.h"

namespace Firmware
{

Mouse::Mouse(MicroBit &uBit, Firmware::Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, front_pid(uBit.timer, "f", 20, 0, 10),
      left_right_pid(uBit.timer, "lr", 20, 0, 10)
{
}

void Mouse2::GoToCenter() {}
} // namespace Firmware
