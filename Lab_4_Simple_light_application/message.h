#ifndef MESSAGE_H
#define MESSAGE_H

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_secur.h"
#include "zb_secur_api.h"
typedef enum command_id_e 
{
ENABLE, TOGGING, TOGGING_COLOR, BRIGHTNESS_UP, BRIGHTNESS_DOWN, BRIGHTNESS
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

void send_toggling_color_command(zb_uint8_t param);
void send_toggling_command(zb_uint8_t param);
void send_brightness_up_command(zb_uint8_t param);
void send_brightness_down_command(zb_uint8_t param);
void send_brightness_to_level_command(zb_uint8_t param);
void send_enabling_command(zb_uint8_t param);

#endif