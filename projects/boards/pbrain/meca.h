#ifndef MECA_H
#define MECA_H

#define MECA_DROP_FLOOR 9
#define MECA_TAKE_FLOOR 5
#define MECA_TAKE_ACCEL 3
#define MECA_TAKE_ACCEL_END   (0x103)

void meca_init(void);
void meca_action(uint8_t arm_id, uint16_t req);

#endif// MECA_H
