/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*          Copyright (c) 2011 ClarIDy Solutions, Inc., Taipei, Taiwan.     *
*                       http://www.claridy.com/                            *
*                                                                          *
*          Copyright (c) 2011 Uniband Electronic Corporation (UBEC),       *
*                             Hsinchu, Taiwan.                             *
*                       http://www.ubec.com.tw/                            *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack, you*
* may choose which license to receive this code under (except as noted in  *
* per-module LICENSE files).                                               *
*                                                                          *
* ZBOSS is a registered trademark of DSR Corporation AKA Data Storage      *
* Research LLC.                                                            *
*                                                                          *
* GNU General Public License Usage                                         *
* This file may be used under the terms of the GNU General Public License  *
* version 2.0 as published by the Free Software Foundation and appearing   *
* in the file LICENSE.GPL included in the packaging of this file.  Please  *
* review the following information to ensure the GNU General Public        *
* License version 2.0 requirements will be met:                            *
* http://www.gnu.org/licenses/old-licenses/gpl-2.0.html.                   *
*                                                                          *
* Commercial Usage                                                         *
* Licensees holding valid ClarIDy/UBEC/DSR Commercial licenses may use     *
* this file in accordance with the ClarIDy/UBEC/DSR Commercial License     *
* Agreement provided with the Software or, alternatively, in accordance    *
* with the terms contained in a written agreement between you and          *
* ClarIDy/UBEC/DSR.                                                        *
*                                                                          *
****************************************************************************
PURPOSE: Test for ZC application written using ZDO.
*/

#include "message.h"
#include "stm_lib.h"

#define ZB_TEST_DUMMY_DATA_SIZE 10

zb_ieee_addr_t g_zc_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif

/*
  The test is: ZC starts PAN, ZR joins to it by association and send APS data packet, when ZC
  received packet, it sends packet to ZR, when ZR received packet, it sends
  packet to ZC etc.
 */
static zb_uint8_t current_red;
static zb_uint8_t current_blue;
static zb_uint8_t current_green;
static int light;
void receive_task(zb_uint8_t param) ZB_CALLBACK;
static void set_color_command(zb_uint8_t param);
static void set_toggling_command(void);
static void set_brightness_command(void);
static void start_leds(void);

MAIN()
{
  ARGV_UNUSED;
  init_leds();
#if !(defined KEIL || defined SDCC || defined ZB_IAR)
  if ( argc < 3 )
  {
    //printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif


  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif
#ifdef ZB_SECURITY
  ZG->nwk.nib.security_level = 0;
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;
  ZB_AIB().aps_channel_mask = (1l << 13);

  if (zdo_dev_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zdo_dev_start failed", (FMT__0));
  }
  else
  {
    zdo_main_loop();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  start_leds();
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_af_set_data_indication(receive_task);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  zb_free_buf(buf);
}
static void start_leds(void)
{
  current_red = 0;
  current_green = 0;
  current_blue = 255;
  light = 30000;
  set_color(current_red*light, current_green*light, current_blue*light);
}
void receive_task(zb_uint8_t param) ZB_CALLBACK
{
  zb_uint8_t *ptr;
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  ZB_APS_HDR_CUT_P(asdu, ptr);
  switch(ptr[0])
  {
     case TOGGLING: 
     {
	set_toggling_command();
	break;
     }
     case BRIGHTNESS: 
     {
	set_brightness_command();
	TRACE_MSG(TRACE_APS2, "Change brightness", (FMT__0));
	break;
     }
     case SET_COLOR: 
     {
	set_color_command(ptr[1]);
	break;
     }
  }
  zb_free_buf(asdu);
}
static void set_brightness_command(void)
{
  light += brightness_step;
  if(light > period)
  {
	light = 0;
  }
  set_color(current_red*light, current_green*light, current_blue*light);
}
static void set_color_command(zb_uint8_t color)
{
  switch(color)
  {
     case BLUE: 
     {
	current_red = 0;
	current_green = 0;
	current_blue = 255;
	break;
     }
     case GREEN: 
     {
	current_red = 0;
	current_green = 255;
	current_blue = 0;
	break;
     }
     case RED: 
     {
	current_red = 255;
	current_green = 0;
	current_blue = 0;
	break;
     }
  }
  set_color(current_red*light, current_green*light, current_blue*light);
  TRACE_MSG(TRACE_APS2, "Toggling  the color (%d)", (FMT__D, param));
}

static void set_toggling_command(void)
{   
  static zb_bool_t condition = ZB_FALSE;
  if (condition)
  {
	set_color(current_red*light, current_green*light, current_blue*light);
	TRACE_MSG(TRACE_APS2, "Enabling a light bulb", (FMT__0));  
  }
  else 
  {
	turn_off();
	TRACE_MSG(TRACE_APS2, "Disabling a light bulb", (FMT__0));
  }
 condition = !condition;
}


/*! @} */
