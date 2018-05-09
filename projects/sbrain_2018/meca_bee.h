#ifndef MECA_BEE_H
#define MECA_BEE_H

void meca_bee_init(void);
void meca_bee_prepare(void);
void meca_bee_action(void);

void com_bee_handler(uint16_t pid, uint8_t * data, uint32_t len);

#endif

