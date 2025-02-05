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
#include "stdatomic.h"

hdefaults_tick_t hbox_tick_get(void)
{
    return luat_mcu_tick64()/(1000*luat_mcu_us_period());
}

//中断嵌套层数
static atomic_int   hbox_critical_interrupt_nested=INT_MIN/2;
//中断初始化，若未初始化时,类似于中断状态，加锁直接返回。
static void hbox_critical_interrupt_init()
{
    hbox_critical_interrupt_nested-=INT_MIN/2;
}
//中断进入,对于不处于任务中的代码(尤其是没有机会让出控制权的代码),一定要使用中断进入与退出
void hbox_critical_interrupt_enter()
{
    hbox_critical_interrupt_nested++;
}
//中断退出，对于不处于任务中的代码(尤其是没有机会让出控制权的代码),一定要使用中断进入与退出。
void hbox_critical_interrupt_leave()
{
    hbox_critical_interrupt_nested--;
}
//临界区嵌套的层数
static atomic_int       hbox_critical_nested=0;
static void _Atomic *  current_task=NULL;
void hbox_enter_critical()
{
    //在中断嵌套过程中，默认锁容易出问题，用户尽量不要使用默认锁
    if(hbox_critical_interrupt_nested==0)
    {
        luat_rtos_task_suspend_all();
        while(hbox_critical_nested!=0 && (current_task!=NULL && current_task != luat_get_current_task()))
        {
            //交出控制权
            luat_rtos_task_resume_all();
            luat_rtos_task_sleep(1);
            luat_rtos_task_suspend_all();
        }
        current_task=luat_get_current_task();
        hbox_critical_nested++;
        luat_rtos_task_resume_all();
        return;
    }
    hbox_critical_nested++;
}

void hbox_exit_critical()
{
    hbox_critical_nested--;
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
 * 运行环境应当在任务启动之前完成,优先级"z"低于"9"。
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
    luat_debug_print("hbox task init!");
}

#ifndef CONFIG_WATCHDOG_TIMEOUT
#define CONFIG_WATCHDOG_TIMEOUT 5
#endif // CONFIG_WATCHDOG_TIMEOUT
static luat_rtos_task_handle task_handle=NULL;
static void hbox_task_entry(void * param)
{
    (void)param;
    display_banner();
    //设置好看门狗
    hwatchdog_set_hardware_dog_feed(hw_feed);
    hwatchdog_setup_software_dog(sys_reset,hbox_tick_get);
    luat_wdt_set_timeout(CONFIG_WATCHDOG_TIMEOUT);
    while(true)
    {
        luat_rtos_task_sleep(1);
        HWATCHDOG_FEED();//喂狗
        hbox_loop_process_events();

        {
            static hdefaults_tick_t last_tick=0;
            if(hdefaults_tick_get()-last_tick > 5000)
            {
                last_tick=hdefaults_tick_get();
                static size_t last_max_used=0;
                size_t total=0,max_used=0,used=0;
                luat_meminfo_sys(&total,&used,&max_used);
                if(((last_max_used>max_used)?(last_max_used-max_used):(max_used-last_max_used))> 1024)
                {
                    //最大内存变动超过1k时
                    last_max_used=max_used;
                    luat_debug_print("mem:total=%d,max_used=%d,used=%d",total,max_used,used);
                }
            }
        }
    }
}

static void hbox_task_init(void)
{
    hbox_critical_interrupt_init();
    luat_rtos_task_create(&task_handle, 8*1024, 0, "hbox_task",hbox_task_entry, NULL, 0);
}

/*
 * hbox_task 需要比其它用户任务先启动,优先级"!"高于"0"。
 */
INIT_TASK_EXPORT(hbox_task_init,"!");

