#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <math.h>
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
  float distance_mm;
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

            continue;
          }

          status = VL53L0X_StaticInit(&peripherals[i].dev);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }

          uint8_t VhvSettings;
          uint8_t PhaseCal;
          status = VL53L0X_PerformRefCalibration(&peripherals[i].dev, &VhvSettings, &PhaseCal);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }


          uint32_t refSpadCount;
          uint8_t isApertureSpads;
          status = VL53L0X_PerformRefSpadManagement(&peripherals[i].dev, &refSpadCount, &isApertureSpads);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }

          status = VL53L0X_SetDeviceMode(&peripherals[i].dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }

          status = VL53L0X_StartMeasurement(&peripherals[i].dev);
          if(status != VL53L0X_ERROR_NONE)
          {
            continue;
          }

          peripherals[i].flags &= ~VL53L0X_FLAGS_IN_ERROR;
        }
        else
        {
          if(!(peripherals[i].flags & VL53L0X_FLAGS_IN_ERROR))
          {
            uint8_t ready;
            VL53L0X_Error status = VL53L0X_GetMeasurementDataReady(&peripherals[i].dev, &ready);
            if(status == VL53L0X_ERROR_NONE)
            {
              if(ready) 
              {
                VL53L0X_RangingMeasurementData_t rangingMeasurementData;
                status = VL53L0X_GetRangingMeasurementData(&peripherals[i].dev, &rangingMeasurementData);
                if(status == VL53L0X_ERROR_NONE)
                {
                  peripherals[i].distance_mm = rangingMeasurementData.RangeMilliMeter;
                }
              }
            }
          }
        }
      }
    }
  }
}

float vl53l0x_get_distance_mm(int id)
{
  if((id > 0) && (id < CONFIG_VL53L0X_MAX_PERIPHERALS))
  {
    if(peripherals[id].flags & VL53L0X_FLAGS_INITIALIZED)
    {
      if(!(peripherals[id].flags & VL53L0X_FLAGS_IN_ERROR))
      {
        return peripherals[id].distance_mm;
      }
    }
  }

  return NAN;
}

void vl53l0x_init(int vl53l0x_monitor)
{
  unsigned int i;
  for(i = 0; i < sizeof(peripherals)/sizeof(peripherals[0]); i += 1)
  {
    peripherals[i].flags = 0;
    peripherals[i].distance_mm = NAN;
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
