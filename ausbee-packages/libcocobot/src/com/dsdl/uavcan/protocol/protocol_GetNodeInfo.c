/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: /home/damien/prog/rcocobot/ausbee-packages/libcocobot/dsdl/uavcan/protocol/1.GetNodeInfo.uavcan
 */
#include "uavcan/protocol/GetNodeInfo.h"
#include "canard.h"

#ifndef CANARD_INTERNAL_SATURATE
#define CANARD_INTERNAL_SATURATE(x, max) ( ((x) > max) ? max : ( (-(x) > max) ? (-max) : (x) ) );
#endif

#ifndef CANARD_INTERNAL_SATURATE_UNSIGNED
#define CANARD_INTERNAL_SATURATE_UNSIGNED(x, max) ( ((x) > max) ? max : (x) );
#endif

#define CANARD_INTERNAL_ENABLE_TAO  ((uint8_t) 1)
#define CANARD_INTERNAL_DISABLE_TAO ((uint8_t) 0)

#if defined(__GNUC__)
# define CANARD_MAYBE_UNUSED(x) x __attribute__((unused))
#else
# define CANARD_MAYBE_UNUSED(x) x
#endif

uint32_t uavcan_protocol_GetNodeInfoRequest_encode_internal(uavcan_protocol_GetNodeInfoRequest* CANARD_MAYBE_UNUSED(source),
  void* CANARD_MAYBE_UNUSED(msg_buf),
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    return offset;
}

uint32_t uavcan_protocol_GetNodeInfoRequest_encode(uavcan_protocol_GetNodeInfoRequest* CANARD_MAYBE_UNUSED(source), void* CANARD_MAYBE_UNUSED(msg_buf))
{
    return 0;
}

int32_t uavcan_protocol_GetNodeInfoRequest_decode_internal(const CanardRxTransfer* CANARD_MAYBE_UNUSED(transfer),
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_GetNodeInfoRequest* CANARD_MAYBE_UNUSED(dest),
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(tao))
{
    return offset;
}

int32_t uavcan_protocol_GetNodeInfoRequest_decode(const CanardRxTransfer* CANARD_MAYBE_UNUSED(transfer),
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_GetNodeInfoRequest* CANARD_MAYBE_UNUSED(dest),
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf))
{
    return 0;
}

/**
  * @brief uavcan_protocol_GetNodeInfoResponse_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_protocol_GetNodeInfoResponse_encode_internal(uavcan_protocol_GetNodeInfoResponse* source,
  void* msg_buf,
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    uint32_t c = 0;

    // Compound
    offset = uavcan_protocol_NodeStatus_encode_internal((void*)&source->status, msg_buf, offset, 0);

    // Compound
    offset = uavcan_protocol_SoftwareVersion_encode_internal((void*)&source->software_version, msg_buf, offset, 0);

    // Compound
    offset = uavcan_protocol_HardwareVersion_encode_internal((void*)&source->hardware_version, msg_buf, offset, 0);

    // Dynamic Array (name)
    if (! root_item)
    {
        // - Add array length
        canardEncodeScalar(msg_buf, offset, 7, (void*)&source->name.len);
        offset += 7;
    }

    // - Add array items
    for (c = 0; c < source->name.len; c++)
    {
        canardEncodeScalar(msg_buf,
                           offset,
                           8,
                           (void*)(source->name.data + c));// 255
        offset += 8;
    }

    return offset;
}

/**
  * @brief uavcan_protocol_GetNodeInfoResponse_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_protocol_GetNodeInfoResponse_encode(uavcan_protocol_GetNodeInfoResponse* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_protocol_GetNodeInfoResponse_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_protocol_GetNodeInfoResponse_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_GetNodeInfoResponse dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @param tao: is tail array optimization used
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_GetNodeInfoResponse_decode_internal(
  const CanardRxTransfer* transfer,
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_GetNodeInfoResponse* dest,
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(tao))
{
    int32_t ret = 0;
    uint32_t c = 0;

    // Compound
    offset = uavcan_protocol_NodeStatus_decode_internal(transfer, 0, (void*)&dest->status, dyn_arr_buf, offset, tao);
    if (offset < 0)
    {
        ret = offset;
        goto uavcan_protocol_GetNodeInfoResponse_error_exit;
    }

    // Compound
    offset = uavcan_protocol_SoftwareVersion_decode_internal(transfer, 0, (void*)&dest->software_version, dyn_arr_buf, offset, tao);
    if (offset < 0)
    {
        ret = offset;
        goto uavcan_protocol_GetNodeInfoResponse_error_exit;
    }

    // Compound
    offset = uavcan_protocol_HardwareVersion_decode_internal(transfer, 0, (void*)&dest->hardware_version, dyn_arr_buf, offset, tao);
    if (offset < 0)
    {
        ret = offset;
        goto uavcan_protocol_GetNodeInfoResponse_error_exit;
    }

    // Dynamic Array (name)
    //  - Last item in struct & Root item & (Array Size > 8 bit), tail array optimization
    if (payload_len && tao == CANARD_INTERNAL_ENABLE_TAO)
    {
        //  - Calculate Array length from MSG length
        dest->name.len = ((payload_len * 8) - offset ) / 8; // 8 bit array item size
    }
    else
    {
        // - Array length 7 bits
        ret = canardDecodeScalar(transfer,
                                 offset,
                                 7,
                                 false,
                                 (void*)&dest->name.len); // 255
        if (ret != 7)
        {
            goto uavcan_protocol_GetNodeInfoResponse_error_exit;
        }
        offset += 7;
    }

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->name.data = (uint8_t*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->name.len; c++)
    {
        if (dyn_arr_buf)
        {
            ret = canardDecodeScalar(transfer,
                                     offset,
                                     8,
                                     false,
                                     (void*)*dyn_arr_buf); // 255
            if (ret != 8)
            {
                goto uavcan_protocol_GetNodeInfoResponse_error_exit;
            }
            *dyn_arr_buf = (uint8_t*)(((uint8_t*)*dyn_arr_buf) + 1);
        }
        offset += 8;
    }
    return offset;

uavcan_protocol_GetNodeInfoResponse_error_exit:
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return -CANARD_ERROR_INTERNAL;
    }
}

/**
  * @brief uavcan_protocol_GetNodeInfoResponse_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_GetNodeInfoResponse dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_GetNodeInfoResponse_decode(const CanardRxTransfer* transfer,
  uint16_t payload_len,
  uavcan_protocol_GetNodeInfoResponse* dest,
  uint8_t** dyn_arr_buf)
{
    const int32_t offset = 0;
    int32_t ret = 0;

    /* Backward compatibility support for removing TAO
     *  - first try to decode with TAO DISABLED
     *  - if it fails fall back to TAO ENABLED
     */
    uint8_t tao = CANARD_INTERNAL_DISABLE_TAO;

    while (1)
    {
        // Clear the destination struct
        for (uint32_t c = 0; c < sizeof(uavcan_protocol_GetNodeInfoResponse); c++)
        {
            ((uint8_t*)dest)[c] = 0x00;
        }

        ret = uavcan_protocol_GetNodeInfoResponse_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset, tao);

        if (ret >= 0)
        {
            break;
        }

        if (tao == CANARD_INTERNAL_ENABLE_TAO)
        {
            break;
        }
        tao = CANARD_INTERNAL_ENABLE_TAO;
    }

    return ret;
}
