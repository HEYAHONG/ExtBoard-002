#include "led.h"
#include "luat_rtos.h"
#include "luat_gpio.h"
#include "platform_define.h"
#include "common_api.h"

static uint64_t led0_table=0x5555555555555555;
static uint64_t led1_table=0xAAAAAAAAAAAAAAAA;

uint64_t led_get_table(uint8_t index)
{
    switch(index)
    {
    case 0:
        return led0_table;
    case 1:
        return led1_table;
    default:
        break;
    }
    return 0;
}

void led_set_table(uint8_t index,uint64_t table)
{
    switch(index)
    {
    case 0:

    {
        led0_table=table;
    }
    break;
    case 1:
    {
        led1_table=table;
    }
    break;
    default:
        break;
    }
}


static uint8_t led_index=0;
static luat_rtos_timer_t led_timer=NULL;

static void led_time_callback(void *param)
{
    (void)param;
    luat_gpio_set(LED0_PIN,(led0_table&(1ULL << led_index))!=0?1:0);
    luat_gpio_set(LED1_PIN,(led1_table&(1ULL << led_index))!=0?1:0);
    led_index++;
    if(led_index > 63)
    {
        led_index=0;
    }
}

static void led_init(void)
{
    luat_gpio_cfg_t gpio_cfg;
    luat_gpio_set_default_cfg(&gpio_cfg);
    gpio_cfg.pin = LED0_PIN;
    luat_gpio_open(&gpio_cfg);
    gpio_cfg.pin = LED1_PIN;
    luat_gpio_open(&gpio_cfg);

    luat_rtos_timer_create(&led_timer);
    if(led_timer!=NULL)
    {
        luat_rtos_timer_start(led_timer,100,1,led_time_callback,NULL);
    }
}


INIT_DRV_EXPORT(led_init,"9");
