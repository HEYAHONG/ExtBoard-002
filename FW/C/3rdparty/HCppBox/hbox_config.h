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

#endif // __HBOX_CONFIG_H__
