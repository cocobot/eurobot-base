#ifndef MECA_H
#define MECA_H

#define MECA_DROP_FLOOR 9
#define MECA_DROP_BALANCE 10
#define MECA_TAKE_FLOOR 5
#define MECA_TAKE_ACCEL 3
#define MECA_TAKE_ACCEL_END   (0x103)
#define MECA_PUSH_ACCEL       (0x203)
#define MECA_TAKE_DISTRIB 2
#define MECA_TAKE_DISTRIB_SPECIAL (0x102)
#define MECA_STOP 5 //reuse take floor

void meca_init(void);
void meca_action(uint8_t arm_id, uint16_t req);

#endif// MECA_H

