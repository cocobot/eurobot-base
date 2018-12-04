/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: /home/damien/prog/rcocobot/ausbee-packages/libcocobot/dsdl/uavcan/protocol/5.RestartNode.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_RESTARTNODE
#define __UAVCAN_PROTOCOL_RESTARTNODE

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# Restart the node.
#
# Some nodes may require restart before the new configuration will be applied.
#
# The request should be rejected if magic_number does not equal MAGIC_NUMBER.
#

uint40 MAGIC_NUMBER = 0xACCE551B1E
uint40 magic_number

---

bool ok
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.RestartNode
saturated uint40 magic_number
---
saturated bool ok
******************************************************************************/

#define UAVCAN_PROTOCOL_RESTARTNODE_ID                     5
#define UAVCAN_PROTOCOL_RESTARTNODE_NAME                   "uavcan.protocol.RestartNode"
#define UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE              (0x569E05394A3017F0ULL)

#define UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAX_SIZE       ((40 + 7)/8)

// Constants
#define UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAGIC_NUMBER             0xACCE551B1E // 0xACCE551B1E

typedef struct
{
    // FieldTypes
    uint64_t   magic_number;                  // bit len 40

} uavcan_protocol_RestartNodeRequest;

extern
uint32_t uavcan_protocol_RestartNodeRequest_encode(uavcan_protocol_RestartNodeRequest* source, void* msg_buf);

extern
int32_t uavcan_protocol_RestartNodeRequest_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_RestartNodeRequest* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_protocol_RestartNodeRequest_encode_internal(uavcan_protocol_RestartNodeRequest* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_protocol_RestartNodeRequest_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_RestartNodeRequest* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#define UAVCAN_PROTOCOL_RESTARTNODE_RESPONSE_MAX_SIZE      ((1 + 7)/8)

// Constants

typedef struct
{
    // FieldTypes
    bool       ok;                            // bit len 1

} uavcan_protocol_RestartNodeResponse;

extern
uint32_t uavcan_protocol_RestartNodeResponse_encode(uavcan_protocol_RestartNodeResponse* source, void* msg_buf);

extern
int32_t uavcan_protocol_RestartNodeResponse_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_RestartNodeResponse* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_protocol_RestartNodeResponse_encode_internal(uavcan_protocol_RestartNodeResponse* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_protocol_RestartNodeResponse_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_RestartNodeResponse* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_RESTARTNODE