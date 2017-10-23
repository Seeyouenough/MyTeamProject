#ifndef __FLYSKY_H
#define __FLYSKY_H
#include "sys.h"

#define AUX3  6
#define AUX4  7
#define VRA   4
#define VRB   5
#define YAW   3
#define THR   2
#define PITCH 1
#define ROLL  0
extern  u16 CH[8];

extern  u16 mss;
extern  u8 hch,cb;

void RC_GPIO_Init(void);
u8 EEPROM_read(u8 addr);
void EEPROM_write(u8 addr,u8 dat);
void EEPROM_clean(u8 addr);
void IDWR(u8 wr);

void RC_Init(void);
void flysky_loop(void);
#endif

