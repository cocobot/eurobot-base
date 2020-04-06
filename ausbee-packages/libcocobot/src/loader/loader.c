#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_LOADER

#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <platform.h>
#include <stdio.h>


/**
 * @brief Bootloader mode
 */
typedef enum {
  LOADER_MODE_IDLE,     ///< no order. autoboot in 2s
  LOADER_MODE_LOADING,  ///< firmware uploading
} loader_mode_t;


static loader_mode_t _mode; ///< Bootloader mode, if IDLE -> autoboot in 2s


/**
 * @brief Handle packet from Cocoui
 * @param pid   PID of the packet
 * @param data  buffer for packet data
 * @param len   len of packet data
 */
void cocobot_loader_handle_packet(uint16_t pid, uint8_t * data, uint16_t len)
{
  //read board id
  uint8_t board_id;
  cocobot_com_read_B(data, len, 0, &board_id);

  //only load fimware is board valid
  if(board_id != COCOBOT_COM_ID)
  {
    return;
  }

  switch(pid)
  {
    case COCOBOT_COM_RESET_PID:
      //no reset needed, we already are in bootloader mode
      cocobot_com_send(COCOBOT_COM_BOOTLOADER_PID, "B", COCOBOT_COM_ID);

      //prevent autoboot
      _mode = LOADER_MODE_LOADING;
      break;

    case COCOBOT_COM_BOOTLOADER_CRC_PID:
      {
        //addr
        uint32_t addr;
        cocobot_com_read_D(data, len, 2, (int32_t *)&addr);

        //send current CRC
        cocobot_com_send(COCOBOT_COM_BOOTLOADER_CRC_PID, "BBDD", COCOBOT_COM_ID, 1, addr, mcual_loader_compute_crc_16k(addr));

        //prevent autoboot
        _mode = LOADER_MODE_LOADING;
      }
      break;

    case COCOBOT_COM_BOOTLOADER_ERASE_PID:
      mcual_loader_erase_pgm();
      //send packet when done
      cocobot_com_send(COCOBOT_COM_BOOTLOADER_ERASE_PID, "BB", COCOBOT_COM_ID, 1);
      break;

    case COCOBOT_COM_BOOTLOADER_FLASH_PID:
      {
        uint32_t addr;
        uint16_t dlen;

        //read packet info
        cocobot_com_read_D(data, len, 2, (int32_t*) &addr);
        cocobot_com_read_H(data, len, 6, (int16_t *)&dlen);

        if(len < 1024) {
          uint8_t * buf = pvPortMalloc(dlen); 
          for(int i = 0; i < dlen; i += 1) {
            cocobot_com_read_B(data, dlen, 8 + i, &buf[i]);
          }
          mcual_loader_flash_pgm(addr, buf, dlen);
          vPortFree(buf);
          cocobot_com_send(COCOBOT_COM_BOOTLOADER_FLASH_PID, "BBDH", COCOBOT_COM_ID, 1, addr, 0);
        }
      }
      break;

    case COCOBOT_COM_BOOTLOADER_BOOT_PID:
      mcual_loader_boot();
      break;
  }
}

/**
 * @brief Main task for bootloader
 * @param arg   Not used
 */
void cocobot_loader_task(void * arg)
{
  (void)arg;
  int signalisation = 0;
  for(;;)
  {
    TickType_t  timestamp_ms = xTaskGetTickCount();

    if(_mode == LOADER_MODE_IDLE)
    {
      //2s and no request -> start pgm
      if(timestamp_ms > 2000)
      {
        mcual_loader_boot();
      }

      //500ms and no request -> signal bootloader condition
      if((timestamp_ms > 500) && !signalisation)
      {
        signalisation = 1;
        cocobot_com_send(COCOBOT_COM_BOOTLOADER_PID, "");
      }
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Bootloader initialization function
 */
void cocobot_loader_init(void)
{
  //init static memory
  _mode = LOADER_MODE_IDLE;
  //_request_id = 0;
  //_offset = 0;
  //_src_node_id = 0;
  //_last_activity_us = 0;

  xTaskCreate(cocobot_loader_task, "loader", 600, NULL, 2, NULL );
}

#endif
