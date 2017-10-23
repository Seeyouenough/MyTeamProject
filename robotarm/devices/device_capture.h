#ifndef __DEVICE_CAPTURE_H
#define __DEVICE_CAPTURE_H
#include "stm32f10x.h"
#include "founction.h"

#define POSEDGE_CAPED  0x80
#define NEGEDGE_CAPED  0x40


extern capture_ch_t capture;

void capture_init(void);
void capture_filter(void);
#endif
