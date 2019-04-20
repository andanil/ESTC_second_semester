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


/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif

#ifndef ZB_SECURITY
#error Define ZB_SECURITY
#endif


  


zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};


/*
  The test is: ZC starts PAN, ZR joins to it by association and send APS data packet, when ZC
  received packet, it sends packet to ZR, when ZR received packet, it sends
  packet to ZC etc.
 */

#ifndef APS_RETRANSMIT_TEST
#endif
#define ZB_MY_TEST_DATA_SIZE 5
void receive_task(zb_uint8_t param) ZB_CALLBACK;
static void set_brightness_up();
static void set_brightness_down();
static void set_brightness_to_level(zb_uint8_t param);
static void set_toggling_color(zb_uint8_t param);
static void set_toggling();
static void set_enable();

MAIN()
{
  ARGV_UNUSED;

#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif


  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;
  zb_secur_setup_preconfigured_key(g_key, 0);

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


/*
   Trivial test: dump all APS data received
 */


void receive_task(zb_uint8_t param) ZB_CALLBACK
{
	zb_uint8_t *ptr;
	zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);
	ZB_APS_HDR_CUT_P(asdu, ptr);
	switch(ptr[0])
	{
	   case ENABLE: 
	   {
		set_enable();
		break;
	   }
           case TOGGING: 
	   {
		set_toggling();
		break;
	   }
           case BRIGHTNESS: 
	   {
		set_brightness_to_level(ptr[1]);
		break;
	   }
	   case BRIGHTNESS_UP: 
	   {
		set_brightness_up();
		break;
	   }
	   case BRIGHTNESS_DOWN: 
	   {
		set_brightness_down();
		break;
	   }
           case TOGGING_COLOR: 
	   {
		set_toggling_color(ptr[1]);
		break;
	   }
	}
	zb_free_buf(asdu);
}

static void set_brightness_to_level(zb_uint8_t param)
{   
	TRACE_MSG(TRACE_APS2, "Brightness (%d)", (FMT__D, param));	
}

static void set_brightness_up()
{ 
	TRACE_MSG(TRACE_APS2, "Brightness_step up", (FMT__0));
}

static void set_brightness_down()
{ 
	TRACE_MSG(TRACE_APS2, "Brightness_step down", (FMT__0));
}

static void set_toggling_color(zb_uint8_t param)
{   
	TRACE_MSG(TRACE_APS2, "Toggling  the color (%d)", (FMT__D, param));
}

static void set_toggling()
{   
	TRACE_MSG(TRACE_APS2, "Toggling a light bulb", (FMT__0));
}

static void set_enable()
{   
    static zb_bool_t condition = ZB_TRUE;
    if (condition)
    {
	TRACE_MSG(TRACE_APS2, "Enabling a light bulb", (FMT__0));  
    }
    else 
    {
	TRACE_MSG(TRACE_APS2, "Disabling a light bulb", (FMT__0));
    }
    condition = !condition;
}
/*! @} */