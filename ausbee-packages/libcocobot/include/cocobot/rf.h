#ifndef RF_H
#define RF_H

void cocobot_com_rf_init(void);
int16_t cocobot_com_rf_receive(CanardCANFrame* const frame, uint64_t timestamp_us);
int16_t cocobot_com_rf_transmit(const CanardCANFrame* const frame, uint64_t timestamp_us);

#endif //RF_H
