#include <iostream>

#include "multitimeout.h"

void multitimeout_event_handler(Multitimeout* mt, int tmr, void* arg0, int arg1) {
    (void)mt;
    (void)tmr;
    (void)arg0;
    (void)arg1;
    std::cout << "Timer expired: " << int(tmr) << std::endl;
}

int main()
{
    Multitimeout mt;
    mt.bind_event_handler(multitimeout_event_handler);
    Multitimeout::ms_t tmr[128];
    Multitimeout::ms_t tmr_reload[128];
    bool mem0[128];
    bool mem1[128];
    bool mem2[128];
    bool mem3[128];
    mt.bind_memory(128, tmr, tmr_reload, mem0, mem1, mem2, mem3);
    for (int i = 0; i < 128; i++) {
        tmr[i] = std::chrono::milliseconds(0);
        tmr_reload[i] = std::chrono::milliseconds(0);
        mem0[i] = 0;
        mem1[i] = 0;
        mem2[i] = 0;
        mem3[i] = 0;
    }
    std::cout << "Hello World!\n";
    mt.add(0, std::chrono::milliseconds(10000), false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    mt.add(1, std::chrono::milliseconds(10000), false);
    while (69);
}
