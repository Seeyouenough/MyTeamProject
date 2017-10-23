#ifndef  __ULTRASOUND_H
#define  __ULTRASOUND_H
#include "stm32f10x.h"
#include "sys.h"

#define ULTRASOUND_TRIG PCout(5)
#define ULTRASOUND_ECHO PCin(4)
#define MISS_WEIGHT_CNT 9

extern uint32_t ultra_cycle,ultra_cnt;
extern float ultra_distance;
extern uint32_t ultra_sys_runtime;
extern float ultra_angle,straight_angle;
extern uint8_t ultra_miss,ultra_index;
extern float ultra_buff[10],ultra_temp;


void ultra_timer_init(u16 arr,u16 psc); 
uint32_t ultra_system_time(void);
void ultrasound_init(void);
void set_ultra_edge(uint8_t edge);
uint32_t get_ultra_counter(void);
void senor_loop(void);

#endif
