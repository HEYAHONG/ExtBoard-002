#include "hbox_config.h"
#include "hbox.h"
#include "luat_rtos.h"
#include "luat_rtos_legacy.h"
#include "luat_mem.h"
#include "luat_mcu.h"

hdefaults_tick_t hbox_tick_get(void)
{
    return luat_mcu_tick64()/(1000*luat_mcu_us_period());
}

static int32_t critical_nested=0;
static uint32_t critical_data=0;
void hbox_enter_critical()
{
    if(critical_nested==0)
    {
        critical_data=luat_rtos_entry_critical();
    }
    critical_nested++;
}

void hbox_exit_critical()
{
    critical_nested--;
    if(critical_nested==0)
    {
        luat_rtos_exit_critical(critical_data);
    }
}

void * hbox_malloc(size_t bytes)
{
    return luat_heap_malloc(bytes);
}

void hbox_free(void *ptr)
{
    luat_heap_free(ptr);
}



