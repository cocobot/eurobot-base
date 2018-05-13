#ifndef MECA_WATER_H
#define MECA_WATER_H

void meca_water_init(void);
void meca_water_prepare(void);
void meca_water_take_from_distributor(void);
void meca_water_activate(void);
void meca_water_shoot_left(void);
void meca_water_shoot_all(void);
void meca_water_release_bad_water(void);

void com_water_handler(uint16_t pid, uint8_t * data, uint32_t len);

#endif
