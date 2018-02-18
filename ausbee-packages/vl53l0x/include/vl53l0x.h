#ifndef VL53L0X_H
#define VL53L0X_H

void vl53l0x_init(int vl53l0x_monitor);
int vl53l0x_register_device(uint8_t addr);
float vl53l0x_get_distance_mm(int id);

#endif// VL53L0X_H
