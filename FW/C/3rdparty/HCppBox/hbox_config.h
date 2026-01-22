#ifndef __HBOX_CONFIG_H__
#define __HBOX_CONFIG_H__

#define HDEFAULTS_TICK_GET      hbox_tick_get
#define HDEFAULTS_MUTEX_LOCK    hbox_enter_critical
#define HDEFAULTS_MUTEX_UNLOCK  hbox_exit_critical
#define HDEFAULTS_MALLOC        hbox_malloc
#define HDEFAULTS_FREE          hbox_free

/*
 * EC618模块自带libmqtt，无需paho.mqtt.embedded_c
 */
#define H3RDPARTY_USING_SYSTEM_PAHO_MQTT_EMBEDDED_C_PACKET 1


/*
 * 启用C++初始化
 */
#define HCPPRT_USE_CTORS       1


/*
 * HShell缓冲大小
 */
#define HSHELL_CONTEXT_BUFFER_SIZE 512


/*
 * 启用初始化段
 */
#define HRUNTIME_USING_INIT_SECTION 1

/*
 * 启用循环段
 */
#define HRUNTIME_USING_LOOP_SECTION 1

/*
 * 启用符号段
 */
#define HRUNTIME_USING_SYMBOL_SECTION 1


/*
 * 使用自定义的putchar
 */
#define HPUTCHAR hbox_shell_putchar

/*
 * 使用自定义的getchar
 */
#define HGETCHAR hbox_shell_getchar

/*
 * 使用系统的TLSF
 */
#define H3RDPARTY_USING_SYSTEM_TLSF 1

#if defined(H3RDPARTY_TLSF_PORT)
/*
 * 恢复TLSF默认的定义(SDK中的TLSF头文件某些函数的参数较开源版本多了参数)
 */
#include "tlsf.h"
static inline void * tlsf_malloc1(tlsf_t tlsf,size_t size)
{
    return tlsf_malloc(tlsf, size, 0);
}
static inline void * tlsf_memalign1(tlsf_t tlsf,size_t align,size_t bytes)
{
    return tlsf_memalign(tlsf, align, bytes, 0);
}
static inline void * tlsf_realloc1(tlsf_t tlsf,void * ptr,size_t size)
{
    return tlsf_realloc(tlsf, ptr, size, 0);
}
typedef void (*tlsf_walker1)(void* ptr, size_t size, int used, void* user);
static inline void tlsf_walk_pool1(pool_t pool, tlsf_walker1 walker, void* user)
{
    tlsf_walk_pool(pool,(tlsf_walker)(void *)walker);
}
static inline bool  tlsf_check_pool1(pool_t pool)
{
    return  tlsf_check_pool(pool,0);
}
#define  tlsf_malloc                        tlsf_malloc1
#define  tlsf_memalign                      tlsf_memalign1
#define  tlsf_realloc                       tlsf_realloc1
#define  tlsf_walk_pool                     tlsf_walk_pool1
#define  tlsf_check_pool                    tlsf_check_pool1

#endif

#endif // __HBOX_CONFIG_H__
