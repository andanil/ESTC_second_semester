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

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef APS_RETRANSMIT_TEST
#endif

/*
  ZR joins to ZC, then sends APS packet.
 */
static void send(zb_uint8_t param) ZB_CALLBACK; 
static void set_color_command(zb_uint8_t param) ZB_CALLBACK ;
static void set_brightness_command(zb_uint8_t param) ZB_CALLBACK ;
static void set_toggling_command(zb_uint8_t param) ZB_CALLBACK ;
static volatile zb_bool_t button0 = ZB_FALSE;
static volatile zb_bool_t button1 = ZB_FALSE;
zb_ieee_addr_t g_zr_addr = {0x01, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};


MAIN()
{
  ARGV_UNUSED;
  init_buttons();
#if !(defined KEIL || defined SDCC|| defined ZB_IAR)
  if ( argc < 3 )
  {
    //printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zr", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif
#ifdef ZB_SECURITY
  ZG->nwk.nib.security_level = 0;
#endif

  ZB_AIB().aps_channel_mask = (1l << 13); 
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zr_addr); 

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

void EXTI0_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line0)!= RESET)
        {
	 button0 = ZB_TRUE;
	 ZB_SCHEDULE_ALARM_CANCEL(send, ZB_ALARM_ALL_CB);
	 ZB_SCHEDULE_ALARM (send,0,ZB_TIME_ONE_SECOND);
         EXTI_ClearITPendingBit(EXTI_Line0);
        }
}
void EXTI1_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line1)!= RESET)
        {
	 button1 = ZB_TRUE;
	 ZB_SCHEDULE_ALARM_CANCEL(send, ZB_ALARM_ALL_CB);
	 ZB_SCHEDULE_ALARM (send,0,ZB_TIME_ONE_SECOND);
         EXTI_ClearITPendingBit(EXTI_Line1);
        }
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}
static void send(zb_uint8_t param) ZB_CALLBACK 
{
	if(button0 && button1)
	{
		ZB_SCHEDULE_CALLBACK(set_color_command,0);
	}
	else 	
	{
		if(button0)
		{
		  ZB_SCHEDULE_CALLBACK(set_toggling_command,0);
		}
           	else
		{
		  ZB_SCHEDULE_CALLBACK(set_brightness_command,0);
		}
	}
	button0 = ZB_FALSE;
	button1 = ZB_FALSE;
}

static void set_toggling_command(zb_uint8_t param) ZB_CALLBACK 
{
  if (param==0)
  {
	ZB_GET_OUT_BUF_DELAYED(set_toggling_command);
        return;
  }
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  zb_message_params_t *ind = ZB_GET_BUF_PARAM(buf, zb_message_params_t);
  ind->addr = 0;
  send_toggling_command(ZB_REF_FROM_BUF(buf));
}

static void set_brightness_command(zb_uint8_t param)
{
  if (param==0)
  {
	ZB_GET_OUT_BUF_DELAYED(set_brightness_command);
        return;
  }
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  zb_message_params_t *ind = ZB_GET_BUF_PARAM(buf, zb_message_params_t);
  ind->addr = 0;
  send_brightness_command(ZB_REF_FROM_BUF(buf));
}

static void set_color_command(zb_uint8_t param) ZB_CALLBACK 
{
  if (param==0)
  {
	ZB_GET_OUT_BUF_DELAYED(set_color_command);
        return;
  }
  static zb_uint8_t color = 0;
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  zb_message_params_t *ind = ZB_GET_BUF_PARAM(buf, zb_message_params_t);
  ind->addr = 0;
  ind->params = color;
  send_set_color_command(ZB_REF_FROM_BUF(buf));
  color++;
  color %= 3;
}




/*! @} */
