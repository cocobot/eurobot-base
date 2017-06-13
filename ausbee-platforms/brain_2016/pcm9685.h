#ifndef PCM9685_H
#define PCM9685_H

#include <stdint.h>

void pcm9685_init(void);
void pcm9685_set_channel(uint8_t id, uint16_t delay, uint16_t width);


#endif// PCM9685_H
