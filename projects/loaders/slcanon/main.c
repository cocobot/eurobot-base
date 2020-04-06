#include <platform.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>


/**
 * @brief Handle packet from Cocoui or other boards if libcocobot didn't want it
 * @param pid   PID of the packet
 * @param data  buffer for packet data
 * @param len   len of packet data
 */
void com_handler(uint16_t pid, uint8_t * data, uint32_t len)
{
  (void)pid;
  (void)data;
  (void)len;
}
 

int main(int argc, char *argv[]) 
{
#ifdef AUSBEE_SIM
  mcual_arch_main(argc, argv);
#else
  (void)argc;
  (void)argv;
#endif
  platform_init();

  cocobot_com_init(PLATFORM_USART_USER, 1, 1, com_handler);
  cocobot_loader_init();
  vTaskStartScheduler();

  return 0;
}
