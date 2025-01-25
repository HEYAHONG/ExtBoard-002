#include "hbox_config.h"
#include "hbox.h"
#include "hbox_loop.h"
#ifdef HRC_ENABLED
#include "hrc.h"
#endif // HRC_ENABLED
#include "luat_rtos.h"
#include "luat_rtos_legacy.h"
#include "luat_mem.h"
#include "luat_mcu.h"
#include "luat_debug.h"
#include "luat_pm.h"
#include "luat_wdt.h"
#include "common_api.h"

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

static void hbox_init(void)
{
    luat_debug_print("hbox lowlevel init!");
    //初始化HBox C++运行环境
    hcpprt_init();
}

/*
 * 运行环境应当在任务启动之前完成。
 */
INIT_DRV_EXPORT(hbox_init,"z");

static void sys_reset()
{
    luat_pm_reboot();
    //若NVIC复位未实现，使用死循环通过硬件看门狗复位
    while(true)
    {

    }
}

static void hw_feed()
{
    luat_wdt_feed();
}

static void display_banner(void)
{
#ifdef HRC_ENABLED
    {
        //打印banner
        char * banner=(char *)RCGetHandle("banner");
        if(banner!=NULL)
        {
            luat_debug_print("\n%s\n",banner);
        }
    }
#endif // HRC_ENABLED
}

#ifndef CONFIG_WATCHDOG_TIMEOUT
#define CONFIG_WATCHDOG_TIMEOUT 5
#endif // CONFIG_WATCHDOG_TIMEOUT
static luat_rtos_task_handle task_handle=NULL;
static void hbox_task_entry(void * param)
{
    (void)param;
    display_banner();
    luat_debug_print("hbox task init!");
    //设置好看门狗
    hwatchdog_set_hardware_dog_feed(hw_feed);
    hwatchdog_setup_software_dog(sys_reset,hbox_tick_get);
    luat_wdt_set_timeout(CONFIG_WATCHDOG_TIMEOUT);
    while(true)
    {
        luat_rtos_task_sleep(1);
        HWATCHDOG_FEED();//喂狗
        hbox_loop_process_events();
    }
}

static void hbox_task_init(void)
{
    luat_rtos_task_create(&task_handle, 8*1024, 0, "hbox_task",hbox_task_entry, NULL, 0);
}

/*
 * hbox_task
 */
INIT_TASK_EXPORT(hbox_task_init,"0");
