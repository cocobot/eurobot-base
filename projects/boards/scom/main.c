#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include <queue.h>


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

  cocobot_com_init(MCUAL_USART1, 1, 1, com_handler);

  vTaskStartScheduler();

  return 0;
}
