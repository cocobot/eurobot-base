#include <stdio.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
//#include <position.h>
//#include <asserv.h>
//#include <pcm9685.h>
//#include <max11628.h>
//#include <max7317.h>
//#include <gyro.h>
//#include <trajectory.h>
//#include <usir.h>
//#include <step.h>
//#include <pathfinder.h>
//#include "meca.h"
//#include "strat.h"

void blink(void * arg)
{
  (void)arg;
  while(1)
  {
    platform_led_toggle(PLATFORM_LED2 | PLATFORM_LED1);
    vTaskDelay(500 / portTICK_PERIOD_MS);


    cocobot_console_send_asynchronous("test", "plop");
    printf("test\n");
  }
}

int console_handler(const char * cmd)
{
  (void)cmd;
  return 0;
}

int main(void) 
{
  platform_init();
 // cocobot_console_init(MCUAL_USART1, 1, console_handler);
  printf("test3\n");

  xTaskCreate(blink, "blink", 200, NULL, 1, NULL );
  printf("test4\n");

  vTaskStartScheduler();
  printf("test5\n");

  return 0;
  /*
  //modules and peripherals initialization
  dcmInit();
  pcm9685Init();
  max11628Init();
  max7317Init();
  lcdInit();
  fieldInit();
  posInit(position_computed_hook);
  trajectoryInit();
  asservInit();
  mecaInit();
  stepInit();
  usirInit();
  pathfinderInit();
  //gyroInit();
  stratInit();

  lcdPrintln(LCD_WARNING, "Start: robot principal");


  trajectorySetSafetymm(0);


  switch(stratGetColor())
  {
    case STRAT_COLOR_GREEN:
      stepAction(STEP_ACTION_RESET_GREEN);
      break;

    case STRAT_COLOR_YELLOW:
      stepAction(STEP_ACTION_RESET_YELLOW);
      break;
  }
  if(0) 
  {
    stepWait();
    stepAction(STEP_ACTION_PRETAKE_FIRST_BALL_RIGHT);
    stepWait();
    asservSetEnable(1);
    TRAJECTORY_D_MM(20);
    TRAJECTORY_D_MM(-20);
    trajectoryWait();
    stepAction(STEP_ACTION_TAKE_FIRST_BALL_RIGHT);
    stepWait();
  //  stepAction(STEP_ACTION_PREP_SPOT_LEFT);
    stepWait();

    while(1)
    {
      while((max7317Read() & (1 << IO_SWITCH_BACK_LEFT)))
      {
        chThdSleepMilliseconds(100);
      }
      lcdPrintln(LCD_INFO, "Click !");
      stepAction(STEP_ACTION_PRETAKE_SPOT_LEFT);
      stepWait();
      TRAJECTORY_D_MM(50);
      trajectoryWait();

      TRAJECTORY_D_MM(-50);
      trajectoryWait();

      stepAction(STEP_ACTION_TAKE_SPOT_LEFT);
      stepWait();
      lcdPrintln(LCD_INFO, "Done !");

      while(!(max7317Read() & (1 << IO_SWITCH_BACK_LEFT)))
      {
        chThdSleepMilliseconds(100);
      }
    }
  }

  trajectorySetSafetymm(0);
  lcdPrintln(LCD_INFO, "Attente tirette (mise en place)");
  while(!(max7317Read() & (1 << IO_SWITCH_STARTUP)))
  {
    chThdSleepMilliseconds(100);
  }

  lcdPrintln(LCD_INFO, "Asserv: go");
  //asservSetEnable(1);
  //stratWedging();
  stepWait();

  posSetAdeg(180);
  posSetYmm(1000);
  posSetXmm(-1500 + 105 + 90);

  lcdPrintln(LCD_INFO, "Attente du depart");

  while(max7317Read() & (1 << IO_SWITCH_STARTUP))
  {
    chThdSleepMilliseconds(100);
  }
  trajectorySetSafetymm(470);
  stratStart();





  


  //chThdSleepMilliseconds(3000);
  //  dcmSetWidth(0, 500);
  //  dcmSetWidth(1, -500);
  //while (true)
  //{
  //  lcdPrintln(LCD_INFO,"l %ld, r %ld", posGetLeftTick(), posGetRightTick());
  //  chThdSleepMilliseconds(500);
  //}

 
  //int i = 0;
  //int d = 400;
  //while (true)
  //{
  //  lcdPrintln(LCD_INFO, "tour: %d", i);
  //  i += 1;
  //  TRAJECTORY_XY_MM(d, 0);
  //  TRAJECTORY_XY_MM(d, d);
  //  TRAJECTORY_XY_MM(0, d);
  //  TRAJECTORY_XY_MM(0, 0);
  //  trajectoryWait();
  //}

  while(true)
  {
    chThdSleepMilliseconds(3000);
  }
  */
}
