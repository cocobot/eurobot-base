#ifndef RF_H
#define RF_H

#include <canard.h>

void cocobot_com_rf_init(void);
int16_t cocobot_com_rf_receive(CanardCANFrame* const frame, uint64_t timestamp_us);
int16_t cocobot_com_rf_transmit(const CanardCANFrame* const frame, uint64_t timestamp_us);

//low level api
typedef enum
{
  COM_RF_PACKET_STATUS,
  COM_RF_PACKET_DATA,
} com_rf_packet_type_t;

typedef struct __attribute((packed))
{
  uint8_t type;
  union
  {
    CanardCANFrame frame;
  };
} com_rf_packet_t;

void cocobot_com_rf_low_level_init(void);
void cocobot_com_rf_low_level_begin_tx(void);
void cocobot_com_rf_low_level_end_tx(void);
void cocobot_com_rf_recv(uint8_t src_id, int8_t rssi, com_rf_packet_t * p);

#endif //RF_H
