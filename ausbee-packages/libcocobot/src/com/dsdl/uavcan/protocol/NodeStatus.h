/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: /home/damien/prog/rcocobot/ausbee-packages/libcocobot/dsdl/uavcan/protocol/341.NodeStatus.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_NODESTATUS
#define __UAVCAN_PROTOCOL_NODESTATUS

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# Abstract node status information.
#
# Any UAVCAN node is required to publish this message periodically.
#

#
# Publication period may vary within these limits.
# It is NOT recommended to change it at run time.
#
uint16 MAX_BROADCASTING_PERIOD_MS = 1000
uint16 MIN_BROADCASTING_PERIOD_MS = 2

#
# If a node fails to publish this message in this amount of time, it should be considered offline.
#
uint16 OFFLINE_TIMEOUT_MS = 3000

#
# Uptime counter should never overflow.
# Other nodes may detect that a remote node has restarted when this value goes backwards.
#
uint32 uptime_sec

#
# Abstract node health.
#
uint2 HEALTH_OK         = 0     # The node is functioning properly.
uint2 HEALTH_WARNING    = 1     # A critical parameter went out of range or the node encountered a minor failure.
uint2 HEALTH_ERROR      = 2     # The node encountered a major failure.
uint2 HEALTH_CRITICAL   = 3     # The node suffered a fatal malfunction.
uint2 health

#
# Current mode.
#
# Mode OFFLINE can be actually reported by the node to explicitly inform other network
# participants that the sending node is about to shutdown. In this case other nodes will not
# have to wait OFFLINE_TIMEOUT_MS before they detect that the node is no longer available.
#
# Reserved values can be used in future revisions of the specification.
#
uint3 MODE_OPERATIONAL      = 0         # Normal operating mode.
uint3 MODE_INITIALIZATION   = 1         # Initialization is in progress; this mode is entered immediately after startup.
uint3 MODE_MAINTENANCE      = 2         # E.g. calibration, the bootloader is running, etc.
uint3 MODE_SOFTWARE_UPDATE  = 3         # New software/firmware is being loaded.
uint3 MODE_OFFLINE          = 7         # The node is no longer available.
uint3 mode

#
# Not used currently, keep zero when publishing, ignore when receiving.
#
uint3 sub_mode

#
# Optional, vendor-specific node status code, e.g. a fault code or a status bitmask.
#
uint16 vendor_specific_status_code
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.NodeStatus
saturated uint32 uptime_sec
saturated uint2 health
saturated uint3 mode
saturated uint3 sub_mode
saturated uint16 vendor_specific_status_code
******************************************************************************/

#define UAVCAN_PROTOCOL_NODESTATUS_ID                      341
#define UAVCAN_PROTOCOL_NODESTATUS_NAME                    "uavcan.protocol.NodeStatus"
#define UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE               (0xF0868D0C1A7C6F1ULL)

#define UAVCAN_PROTOCOL_NODESTATUS_MAX_SIZE                ((56 + 7)/8)

// Constants
#define UAVCAN_PROTOCOL_NODESTATUS_MAX_BROADCASTING_PERIOD_MS              1000 // 1000
#define UAVCAN_PROTOCOL_NODESTATUS_MIN_BROADCASTING_PERIOD_MS                 2 // 2
#define UAVCAN_PROTOCOL_NODESTATUS_OFFLINE_TIMEOUT_MS                      3000 // 3000
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK                                  0 // 0
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING                             1 // 1
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR                               2 // 2
#define UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL                            3 // 3
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL                           0 // 0
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION                        1 // 1
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_MAINTENANCE                           2 // 2
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_SOFTWARE_UPDATE                       3 // 3
#define UAVCAN_PROTOCOL_NODESTATUS_MODE_OFFLINE                               7 // 7

typedef struct
{
    // FieldTypes
    uint32_t   uptime_sec;                    // bit len 32
    uint8_t    health;                        // bit len 2
    uint8_t    mode;                          // bit len 3
    uint8_t    sub_mode;                      // bit len 3
    uint16_t   vendor_specific_status_code;   // bit len 16

} uavcan_protocol_NodeStatus;

extern
uint32_t uavcan_protocol_NodeStatus_encode(uavcan_protocol_NodeStatus* source, void* msg_buf);

extern
int32_t uavcan_protocol_NodeStatus_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_NodeStatus* dest, uint8_t** dyn_arr_buf);

extern
uint32_t uavcan_protocol_NodeStatus_encode_internal(uavcan_protocol_NodeStatus* source, void* msg_buf, uint32_t offset, uint8_t root_item);

extern
int32_t uavcan_protocol_NodeStatus_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_NodeStatus* dest, uint8_t** dyn_arr_buf, int32_t offset, uint8_t tao);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_NODESTATUS