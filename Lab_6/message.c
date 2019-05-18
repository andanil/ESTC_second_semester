#include "message.h"

static void create_buf(zb_buf_t *buf, zb_uint8_t* message, zb_uint8_t count, zb_uint8_t addr);

static void create_buf(zb_buf_t *buf, zb_uint8_t* message, zb_uint8_t count, zb_uint8_t addr)
{
	zb_apsde_data_req_t *req;
	zb_uint8_t *ptr = NULL;
	ZB_BUF_INITIAL_ALLOC(buf, count*sizeof(zb_uint8_t), ptr);
	req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
	req->dst_addr.addr_short = addr;
	req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
	req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
	req->radius = 1;
	req->profileid = 2;
	req->src_endpoint = 10;
	req->dst_endpoint = 10;
	buf->u.hdr.handle = 0x11;
	memcpy(ptr, message, count*sizeof(zb_uint8_t));
	ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

void send_brightness_command(zb_uint8_t param)
{
	zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
	zb_message_params_t *ind = ZB_GET_BUF_PARAM(buf, zb_message_params_t);
	zb_uint8_t mes[] = {BRIGHTNESS};
	create_buf(buf, mes, 1, ind->addr);
}

void send_set_color_command(zb_uint8_t param)
{
	zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
	zb_message_params_t *ind = ZB_GET_BUF_PARAM(buf, zb_message_params_t);
	zb_uint8_t mes[2]= {SET_COLOR, ind->params};
	create_buf(buf, mes, 2, ind->addr);
}	

void send_toggling_command(zb_uint8_t param)
{
	zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
	zb_message_params_t *ind = ZB_GET_BUF_PARAM(buf, zb_message_params_t);
	zb_uint8_t mes[] = {TOGGLING};
	create_buf(buf, mes, 1, ind->addr);
}