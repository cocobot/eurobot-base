/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: /home/damien/prog/cocobot/ausbee-packages/libcocobot/dsdl/uavcan/cocobot/200.RequestBrushlessConfig.uavcan
 */

#ifndef __UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG
#define __UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <uavcan/cocobot/BrushlessConfig.h>

/******************************* Source text **********************************
---

BrushlessConfig config
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.cocobot.RequestBrushlessConfig
---
uavcan.cocobot.BrushlessConfig config
******************************************************************************/

#define UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG_ID           200
#define UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG_NAME         "uavcan.cocobot.RequestBrushlessConfig"
#define UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG_SIGNATURE    (0x30B39469BA0A8FFFULL)

#define UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG_REQUEST_MAX_SIZE ((0 + 7)/8)

typedef struct
{
    uint8_t empty;
} uavcan_cocobot_RequestBrushlessConfigRequest;

extern
uint32_t uavcan_cocobot_RequestBrushlessConfigRequest_encode(uavcan_cocobot_RequestBrushlessConfigRequest* source, void* msg_buf);

extern
int32_t uavcan_cocobot_RequestBrushlessConfigRequest_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_cocobot_RequestBrushlessConfigRequest* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_cocobot_RequestBrushlessConfigRequest_encode_internal(uavcan_cocobot_RequestBrushlessConfigRequest* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_cocobot_RequestBrushlessConfigRequest_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_cocobot_RequestBrushlessConfigRequest* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#define UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG_RESPONSE_MAX_SIZE ((128 + 7)/8)

// Constants

typedef struct
{
    // FieldTypes
    uavcan_cocobot_BrushlessConfig config;                        //

} uavcan_cocobot_RequestBrushlessConfigResponse;

extern
uint32_t uavcan_cocobot_RequestBrushlessConfigResponse_encode(uavcan_cocobot_RequestBrushlessConfigResponse* source, void* msg_buf);

extern
int32_t uavcan_cocobot_RequestBrushlessConfigResponse_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_cocobot_RequestBrushlessConfigResponse* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_cocobot_RequestBrushlessConfigResponse_encode_internal(uavcan_cocobot_RequestBrushlessConfigResponse* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_cocobot_RequestBrushlessConfigResponse_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_cocobot_RequestBrushlessConfigResponse* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_COCOBOT_REQUESTBRUSHLESSCONFIG