#include "hbox.h"
#ifndef __HBOX_LOOP_H__
#ifdef __cplusplus

/*
 * hbox线程的事件循环
 */
extern hloop<hlock,hcmemory> hbox_loop;

#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
 * hbox线程事件处理，由hbox线程调用
 */
void hbox_loop_process_events();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HBOX_LOOP_H__
