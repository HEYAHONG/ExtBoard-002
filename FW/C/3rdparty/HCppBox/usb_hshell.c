#include "common_api.h"
#include "luat_rtos.h"
#include "luat_debug.h"
#include "luat_uart.h"
#include "hbox.h"

static bool usb_serial_connected=false;
static luat_rtos_task_handle usb_shell_task_handle=NULL;
static hshell_context_t usb_hshell= {0};
static uint8_t usb_rx_buffer[2048]= {0};
static hringbuf_t * usb_rx_buffer_get(void)
{
    hringbuf_t * buffer=hringbuf_get(usb_rx_buffer,sizeof(usb_rx_buffer));
    //不加锁
    hringbuf_set_lock(buffer,NULL,NULL,NULL);
    return buffer;
}
static void luat_usb_recv_cb(int uart_id, uint32_t data_len)
{
    usb_serial_connected=true;
    while(data_len > 0)
    {
        //每次读取16字节
        uint8_t buffer[16];
        size_t bytes_to_read=sizeof(buffer);
        if(bytes_to_read > data_len)
        {
            bytes_to_read=data_len;
        }
        luat_uart_read(uart_id,buffer,bytes_to_read);
        {
            //输入环形缓冲区(不管是否成功)
            hringbuf_t * ringbuffer= usb_rx_buffer_get();
            hringbuf_input(ringbuffer,buffer,bytes_to_read);
        }
        data_len-=bytes_to_read;
    }
}

static void husb_putchar(char c)
{
    luat_uart_write(LUAT_VUART_ID_0, (uint8_t*)&c, sizeof(c));
}


int hbox_shell_putchar(int ch)
{
    if(ch>0)
    {
        husb_putchar(ch&0xFF);
    }
    return ch;

}


int hbox_shell_getchar(void)
{
    int ch=EOF;
    {
        hringbuf_t * buffer= usb_rx_buffer_get();
        if(hringbuf_get_length(buffer))
        {
            uint8_t ch_val=0;
            hringbuf_output(buffer,&ch_val,sizeof(ch_val));
            ch=ch_val;
        }
    }
    return ch;
}

static void task_usb_hshell(void *param)
{
    {
        luat_uart_t uart =
        {
            .id = LUAT_VUART_ID_0,
        };
        luat_uart_setup(&uart);
        luat_uart_ctrl(LUAT_VUART_ID_0, LUAT_UART_SET_RECV_CALLBACK, luat_usb_recv_cb);
    }
    {
        //初始化hshell
        hshell_context_external_api_t api=hshell_context_default_external_api();
        api.getchar=hbox_shell_getchar;
        api.putchar=hbox_shell_putchar;
        hshell_external_api_set(&usb_hshell,api);
        hshell_command_name_shortcut_set(&usb_hshell,true);
        HSHELL_COMMANDS_REGISTER(&usb_hshell);	//注册命令

    }
    while (true)
    {
        luat_rtos_task_sleep(1);
        if(usb_serial_connected)
        {
            while(hshell_loop(&usb_hshell)==0);
        }
    }
    luat_rtos_task_delete(usb_shell_task_handle);
}

static void task_usb_shell(void)
{
    luat_rtos_task_create(&usb_shell_task_handle, 4096, 20, "usb_shell", task_usb_hshell, NULL, 0);
}

INIT_TASK_EXPORT(task_usb_shell,"1");



