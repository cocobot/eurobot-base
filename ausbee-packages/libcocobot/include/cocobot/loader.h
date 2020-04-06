#ifndef COCOBOT_LOADER_H
#define COCOBOT_LOADER_H

void cocobot_loader_init(void);
void cocobot_loader_handle_packet(uint16_t pid, uint8_t * data, uint16_t len);

#endif// COCOBOT_LOADER_H
