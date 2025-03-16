#ifndef __LED_H_INCLUDED__
#define __LED_H_INCLUDED__
#include "stdint.h"
#include "inttypes.h"
#include "stdlib.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/*
    本文件中LED0指STATUS灯(GPIO26)
*/
#define LED0_PIN 26 /**< LED引脚编号 */
/*
    本文件中LED1指NET_STATUS灯(GPIO27)
*/
#define LED1_PIN 27 /**< LED引脚编号 */

/** \brief LED获取亮灭表
 *
 * \param index uint8_t 灯编号
 * \return uint64_t 亮灭表，每bits表示100ms，共64bits
 *
 */
uint64_t led_get_table(uint8_t index);

/** \brief LED设置亮灭表
 *
 * \param index uint8_t 灯编号
 * \param table uint64_t 亮灭表，每bits表示100ms，共64bits
 *
 */
void led_set_table(uint8_t index,uint64_t table);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __LED_H_INCLUDED__
