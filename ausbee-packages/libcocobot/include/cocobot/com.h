#ifndef COCOBOT_COM_H
#define COCOBOT_COM_H

#include <canard.h>

void cocobot_com_init(void);
void cocobot_com_run(void);
uint64_t cocobot_com_process_event(void);
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

#endif// COCOBOT_COM_H
