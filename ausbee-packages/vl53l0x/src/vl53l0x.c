#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include "generated/autoconf.h"
#include "vl53l0x.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

#define VL53L0X_FLAGS_REGISTERED  (1 << 0)
#define VL53L0X_FLAGS_INITIALIZED (1 << 1)
#define VL53L0X_FLAGS_IN_ERROR    (1 << 2)

typedef struct {
  uint32_t flags;
  VL53L0X_Dev_t dev;
} vl53l0x_peripheral_t;

vl53l0x_peripheral_t peripherals[CONFIG_VL53L0X_MAX_PERIPHERALS];

static void vl53l0x_thread()
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while(pdTRUE)
  {
    vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_PERIOD_MS);

    unsigned int i;
    for(i = 0; i < sizeof(peripherals)/sizeof(peripherals[0]); i += 1)
    {
      if((peripherals[i].flags & VL53L0X_FLAGS_REGISTERED))
      {
        if(!(peripherals[i].flags & VL53L0X_FLAGS_INITIALIZED))
        {
          peripherals[i].flags |= VL53L0X_FLAGS_INITIALIZED;
          peripherals[i].flags |= VL53L0X_FLAGS_IN_ERROR;
          VL53L0X_Error status = VL53L0X_DataInit(&peripherals[i].dev);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }

          VL53L0X_DeviceInfo_t deviceInfo;
          status = VL53L0X_GetDeviceInfo(&peripherals[i].dev, &deviceInfo);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }
          printf("VL53L0X_GetDeviceInfo:\n");
          printf("Device Name : %s\n", deviceInfo.Name);
          printf("Device Type : %s\n", deviceInfo.Type);
          printf("Device ID : %s\n", deviceInfo.ProductId);
          printf("ProductRevisionMajor : %d\n", deviceInfo.ProductRevisionMajor);
          printf("ProductRevisionMinor : %d\n", deviceInfo.ProductRevisionMinor);

          if ((deviceInfo.ProductRevisionMinor != 1) && (deviceInfo.ProductRevisionMinor != 1)) {
            printf("Error expected cut 1.1 but found cut %d.%d\n",
                   deviceInfo.ProductRevisionMajor, deviceInfo.ProductRevisionMinor);
          }
          else
          {
            peripherals[i].flags &= ~VL53L0X_FLAGS_IN_ERROR;
          }
        }
      }
    }
  }
}

void vl53l0x_init(int vl53l0x_monitor)
{
  unsigned int i;
  for(i = 0; i < sizeof(peripherals)/sizeof(peripherals[0]); i += 1)
  {
    peripherals[i].flags = 0;
  }

  xTaskCreate(vl53l0x_thread, "vl53l0x", 512, NULL, vl53l0x_monitor, NULL );
}

int vl53l0x_register_device(uint8_t addr)
{
  unsigned int i;
  for(i = 0; i < sizeof(peripherals)/sizeof(peripherals[0]); i += 1)
  {
    if(!(peripherals[i].flags & VL53L0X_FLAGS_REGISTERED))
    {
      peripherals[i].flags |= VL53L0X_FLAGS_REGISTERED;
      peripherals[i].dev.I2cDevAddr = addr;
      peripherals[i].dev.comms_type = 1;
      peripherals[i].dev.comms_speed_khz = 400;
      return i;
    }
  }

  return -1;
}
