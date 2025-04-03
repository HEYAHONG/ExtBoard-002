#include "hbox_config.h"
#include "hbox.h"
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

static luat_rtos_mutex_t hbox_global_lock=NULL;
static void hbox_critical_init()
{
    if(hbox_global_lock==NULL)
    {
        luat_rtos_mutex_create(&hbox_global_lock);
    }
}
void hbox_enter_critical()
{
    hbox_critical_init();
    luat_rtos_mutex_lock(hbox_global_lock,LUAT_WAIT_FOREVER);
}

void hbox_exit_critical()
{
    luat_rtos_mutex_unlock(hbox_global_lock);
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
    {
        //初始化hshell组件
        hshell_command_name_shortcut_set(NULL,true);
        HSHELL_COMMANDS_REGISTER(NULL);	//注册命令

    }
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

static void hbox_memory_check_slot(void *sig,void *usr)
{
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

#ifndef CONFIG_WATCHDOG_TIMEOUT
#define CONFIG_WATCHDOG_TIMEOUT 60
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
    {
        //注册循环槽
        heventslots_t *slots_loop=heventslots_get_slots_from_table(HEVENTSLOTS_SYSTEM_SLOTS_LOOP);
        if(slots_loop==NULL)
        {
            heventslots_set_slots_to_table(HEVENTSLOTS_SYSTEM_SLOTS_LOOP,NULL);
        }
        slots_loop=heventslots_get_slots_from_table(HEVENTSLOTS_SYSTEM_SLOTS_LOOP);
        if(slots_loop!=NULL)
        {
            //检查内存
            heventslots_register_slot(slots_loop,NULL,hbox_memory_check_slot,NULL);
        }
    }
    while(true)
    {
        luat_rtos_task_sleep(1);
        HWATCHDOG_FEED();//喂狗
        hcpprt_loop();
    }
}

static void hbox_task_init(void)
{
    hbox_critical_init();

    luat_rtos_task_create(&task_handle, 8*1024, 0, "hbox_task",hbox_task_entry, NULL, 0);
}

/*
 * hbox_task 需要比其它用户任务先启动,优先级"!"高于"0"。
 */
INIT_TASK_EXPORT(hbox_task_init,"!");


/*
 * 系统复位
 */
static int cmd_reboot_entry(int argc,const char *argv[])
{
    sys_reset();
    return 0;
}
HSHELL_COMMAND_EXPORT(reboot,cmd_reboot_entry,reboot system);

/*
 * 内存信息
 */
static int cmd_free_entry(int argc,const char *argv[])
{
    hshell_context_t * hshell_ctx=hshell_context_get_from_main_argv(argc,argv);
    {
        size_t total=0,max_used=0,used=0;
        luat_meminfo_sys(&total,&used,&max_used);
        hshell_printf(hshell_ctx,"total:%d bytes,used:%d bytes,free: %d bytes,max_used: %d bytes\r\n",(int)total,(int)used,(int)(total-used),(int)max_used);
    }
    return 0;
}
HSHELL_COMMAND_EXPORT(free,cmd_free_entry,show meminfo);

#include "time.h"
static int cmd_datetime_entry(int argc,const char *argv[])
{
    hshell_context_t * hshell_ctx=hshell_context_get_from_main_argv(argc,argv);
    time_t time_now=time(NULL);
    hshell_printf(hshell_ctx,"%s",asctime(localtime(&time_now)));
    return 0;
};
HSHELL_COMMAND_EXPORT(datetime,cmd_datetime_entry,show datetime);
