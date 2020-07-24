#include <bluetooth/bluetooth.h>
#include "lib/gpio.h"
#include "lib/handle.h"

//server
int main(int argc, char **argv)
{
    handle h;
    h.listen();
    while (h.notExited()) {
        std::list<uint8_t> l = h.getAnimation();
        h.animate(l);
    }
    return 0;
}
