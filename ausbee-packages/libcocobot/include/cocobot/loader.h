#ifndef COCOBOT_LOADER_H
#define COCOBOT_LOADER_H


#include <canard_stm32.h>

void cocobot_loader_init(void);

uint8_t cocobot_loader_should_accept_transfer(uint64_t* out_data_type_signature,
                                              uint16_t data_type_id,
                                              CanardTransferType transfer_type,
                                              uint8_t source_node_id);

uint8_t cocobot_loader_on_transfer_received(CanardRxTransfer* transfer);

#endif// COCOBOT_LOADER_H
