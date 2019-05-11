#ifndef COCOBOT_COM_H
#define COCOBOT_COM_H

#include <canard.h>
#include <uavcan/protocol/NodeStatus.h>

void cocobot_com_init(void);
void cocobot_com_run(void);
uint64_t cocobot_com_process_event(void);
void cocobot_com_flush(void);
void cocobot_com_set_mode(uint8_t mode);
int16_t cocobot_com_request_or_respond(uint8_t destination_node_id,
                                       uint64_t data_type_signature,
                                       uint8_t data_type_id,
                                       uint8_t* inout_transfer_id,
                                       uint8_t priority,
                                       CanardRequestResponse kind,
                                       const void* payload,
                                       uint16_t payload_len);
int16_t cocobot_com_broadcast(uint64_t data_type_signature,
                              uint16_t data_type_id,
                              uint8_t* inout_transfer_id,
                              uint8_t priority,
                              const void* payload,
                              uint16_t payload_len);
void cocobot_com_release_rx_transfer_payload(CanardRxTransfer* transfer);

//user defined functions
uint8_t com_should_accept_transfer(uint64_t* out_data_type_signature,
                                   uint16_t data_type_id,
                                   CanardTransferType transfer_type,
                                   uint8_t source_node_id);
uint8_t com_on_transfer_received(CanardRxTransfer* transfer);
uint8_t cocobot_com_get_node_id(void);


#define IF_RESPONSE_RECEIVED(NAME, name, action)\
  if ((transfer->transfer_type == CanardTransferTypeResponse) && \
      (transfer->data_type_id == NAME ## _ID)\
     )\
  {\
    name data;\
    void * dynbuf = NULL;\
\
    dynbuf = pvPortMalloc(NAME ## _RESPONSE_MAX_SIZE);\
    if(dynbuf != NULL)\
    {\
      uint8_t * pdynbuf = dynbuf;\
      if(name ## _decode(transfer, transfer->payload_len, &data, &pdynbuf) >= 0)\
      {\
        cocobot_com_release_rx_transfer_payload(transfer);\
        {\
          action;\
        }\
      }\
    }\
    if(dynbuf != NULL)\
    {\
      vPortFree(dynbuf);\
    }\
    return 1;\
  }\

#define IF_SMPLREQ_RECEIVED(NAME, name, action)\
  if ((transfer->transfer_type == CanardTransferTypeResponse) && \
      (transfer->data_type_id == NAME ## _ID)\
     )\
  {\
    name data;\
    void * dynbuf = NULL;\
\
    dynbuf = pvPortMalloc(NAME ## _MAX_SIZE);\
    if(dynbuf != NULL)\
    {\
      uint8_t * pdynbuf = dynbuf;\
      if(name ## _decode(transfer, transfer->payload_len, &data, &pdynbuf) >= 0)\
      {\
        cocobot_com_release_rx_transfer_payload(transfer);\
        {\
          action;\
        }\
      }\
    }\
    if(dynbuf != NULL)\
    {\
      vPortFree(dynbuf);\
    }\
    return 1;\
  }\


#endif// COCOBOT_COM_H
