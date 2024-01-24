#include "multitimeout.h"
#include <iostream>

void timeout_handler(Multitimeout* multitimeout, int timer_id) {
    (void)multitimeout;
    std::cout << "Timeout event: " << int(timer_id) << std::endl;
}


int main()
{
    std::cout << "Hello World!\n";
    Multitimeout ip_connection_timeout;
    ip_connection_timeout.bind_event_handler(timeout_handler);
    ip_connection_timeout.add(0, std::chrono::milliseconds(5000), true); //timer id, duration, repeat
    ip_connection_timeout.add(1, std::chrono::milliseconds(10000), false); //will remove itself upon expiry
    std::this_thread::sleep_for(std::chrono::seconds(11));
    ip_connection_timeout.remove(0);
    ip_connection_timeout.add(1, std::chrono::milliseconds(1000), true);

    while (69);
}