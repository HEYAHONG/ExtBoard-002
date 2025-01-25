#include "hbox_loop.h"

hloop<hlock,hcmemory> hbox_loop;

void hbox_loop_process_events()
{
    hbox_loop.process_event();
}

