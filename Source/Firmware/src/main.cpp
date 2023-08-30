#include <MicroBit.h>

const char *const heart =
    "000,255,000,255,000\n"
    "255,255,255,255,255\n"
    "255,255,255,255,255\n"
    "000,255,255,255,000\n"
    "000,000,255,000,000\n";
static const MicroBitImage HEART(heart);

MicroBit uBit;

int main()
{
    while (true)
    {
        uBit.display.print(HEART);
    }

    return 0;
}
