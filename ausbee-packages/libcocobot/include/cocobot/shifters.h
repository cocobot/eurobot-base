#ifndef SHIFTERS_H
#define SHIFTERS_H

void cocobot_shifters_init(void);
void cocobot_shifters_set(uint32_t bit, uint8_t value);
void cocobot_shifters_update(void);

#endif // SHIFTERS_H
