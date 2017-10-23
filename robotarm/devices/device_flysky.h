#ifndef __FLYSKY_H
#define __FLYSKY_H
#include "sys.h"
void RC_GPIO_Init(void);
u8 EEPROM_read(u8 addr);
void EEPROM_write(u8 addr,u8 dat);
void EEPROM_clean(u8 addr);

void TIM7_Int_Init(void);
void RC_Init(void);
void flysky_loop(void);
#endif

