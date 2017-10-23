#ifndef __DRIVER_DATA_TRANS_H
#define __DRIVER_DATA_TRANS_H
#include "stm32f10x.h"
#include "stdbool.h"
#include "device_data_trans.h"

#define TransBuffSize  20
#define rx_ok    0x01
#define rx_fail  0x02
#define dealed   0x03


enum
{
  acc_calib=0x01,
	gyro_calib=0x02,
	mag_calib=0x04,
	read_pid=0x01,
	read_fly_mode_setting=0x02,
	read_fly_point_num=0x21,
};

enum
{
  calib = 0x01,
	ask_read = 0x02,
	pid1 = 0x10,
  pid2 = 0x11,
	pid3 = 0x12,
};

typedef struct
{
  uint8_t head1;
	uint8_t head2;
	uint8_t fonc;
	uint8_t len;
	uint8_t data[TransBuffSize];
	bool is_ack;
	uint8_t sum_check;
	uint8_t sta;
}data_trans_frame;
#define IS_FOUNC_WORD(word) (word==calib)||(word==ask_read)||(word==pid1)||(word==pid2)||(word==pid3)


extern data_trans_frame tx_frame;
extern data_trans_frame rx_frame;

void trans_params_init(uint32_t bound);
void send_frame(void);
#endif


