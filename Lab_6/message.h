#ifndef MESSAGE_H
#define MESSAGE_H
#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
typedef enum command_id_e 
{
TOGGLING, SET_COLOR, BRIGHTNESS
} command_id_t;

typedef enum color_id_e 
{
BLUE, RED, GREEN
} color_id_t;

typedef struct zb_message_params_e
{
	zb_uint8_t addr;
	zb_uint8_t params;
} zb_message_params_t;

void send_set_color_command(zb_uint8_t param);
void send_toggling_command(zb_uint8_t param);
void send_brightness_command(zb_uint8_t param);

#endif