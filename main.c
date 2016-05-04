#include <stdio.h>
#include <stdlib.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mcual.h>
#include <cocobot.h>
#include <pcm9685.h>
#include "meca_seashell.h"
#include "meca_fish.h"
#include "strat_hut.h"
#include "strat_shell.h"
#include "strat_sand.h"

static unsigned int _shell_configuration;

void display_shell_configuration(void)
{
  switch(_shell_configuration)
  {
    case 0:
      platform_gpio_clear(PLATFORM_GPIO2);
      platform_gpio_clear(PLATFORM_GPIO3);
      platform_gpio_clear(PLATFORM_GPIO4);
      platform_gpio_clear(PLATFORM_GPIO5);
      platform_gpio_clear(PLATFORM_GPIO6);
      break;

    case 1:
      platform_gpio_set(PLATFORM_GPIO2);
      platform_gpio_clear(PLATFORM_GPIO3);
      platform_gpio_clear(PLATFORM_GPIO4);
      platform_gpio_clear(PLATFORM_GPIO5);
      platform_gpio_clear(PLATFORM_GPIO6);
      break;

    case 2:
      platform_gpio_set(PLATFORM_GPIO2);
      platform_gpio_set(PLATFORM_GPIO3);
      platform_gpio_clear(PLATFORM_GPIO4);
      platform_gpio_clear(PLATFORM_GPIO5);
      platform_gpio_clear(PLATFORM_GPIO6);
      break;

    case 3:
      platform_gpio_set(PLATFORM_GPIO2);
      platform_gpio_set(PLATFORM_GPIO3);
      platform_gpio_set(PLATFORM_GPIO4);
      platform_gpio_clear(PLATFORM_GPIO5);
      platform_gpio_clear(PLATFORM_GPIO6);
      break;

    case 4:
      platform_gpio_set(PLATFORM_GPIO2);
      platform_gpio_set(PLATFORM_GPIO3);
      platform_gpio_set(PLATFORM_GPIO4);
      platform_gpio_set(PLATFORM_GPIO5);
      platform_gpio_clear(PLATFORM_GPIO6);
      break;

    case 5:
      platform_gpio_set(PLATFORM_GPIO2);
      platform_gpio_set(PLATFORM_GPIO3);
      platform_gpio_set(PLATFORM_GPIO4);
      platform_gpio_set(PLATFORM_GPIO5);
      platform_gpio_set(PLATFORM_GPIO6);
      break;
  }
}

void update_lcd(void * arg)
{
  (void)arg;

  //init leds and button
  platform_gpio_set_direction(PLATFORM_GPIO7, MCUAL_GPIO_INPUT);
  platform_gpio_set_direction(PLATFORM_GPIO2, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO3, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO4, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO5, MCUAL_GPIO_OUTPUT);
  platform_gpio_set_direction(PLATFORM_GPIO6, MCUAL_GPIO_OUTPUT);

#ifndef AUSBEE_SIM
  int vbat = platform_adc_get_mV(PLATFORM_ADC_VBAT);
  if(vbat < COCOBOT_LOW_BAT_THRESHOLD)
  {
    while(1)
    {
      platform_gpio_toggle(PLATFORM_GPIO2);
      platform_gpio_toggle(PLATFORM_GPIO3);
      platform_gpio_toggle(PLATFORM_GPIO4);
      platform_gpio_toggle(PLATFORM_GPIO5);
      platform_gpio_toggle(PLATFORM_GPIO6);

      //disable everything
      meca_fish_disable();
      meca_seashell_disable();
      cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
#endif

  //blink for the fun
  int i;
  for(i = 0; i < 20; i += 1)
  {
    platform_gpio_toggle(PLATFORM_GPIO2);
    platform_gpio_toggle(PLATFORM_GPIO3);
    platform_gpio_toggle(PLATFORM_GPIO4);
    platform_gpio_toggle(PLATFORM_GPIO5);
    platform_gpio_toggle(PLATFORM_GPIO6);

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  //set shell configuration
  uint32_t last = platform_gpio_get(PLATFORM_GPIO7);
  while(!cocobot_game_state_is_starter_removed())
  {
    uint32_t current = platform_gpio_get(PLATFORM_GPIO7);

    if(current && (current != last))
    {
      _shell_configuration = (_shell_configuration + 1) % 6;
    }
    last = current;

    platform_led_toggle(PLATFORM_LED0);
    display_shell_configuration();

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  
  while(1)
  {
    //toggle led
    vTaskDelay(100 / portTICK_PERIOD_MS);
    platform_led_toggle(PLATFORM_LED0);
  }
}

void run_strategy(void * arg)
{
  (void)arg;

  meca_seashell_init();
  meca_fish_init();

  strat_hut_register();
  strat_sand_register();

  cocobot_game_state_wait_for_starter_removed();

  //strat shell must be registered after "starter removed" event because shell configuration can be changed before
  strat_shell_register();

  cocobot_trajectory_goto_d(100, -1);
  cocobot_trajectory_wait();

  cocobot_action_scheduler_start();
}

int console_handler(const char * command)
{
  if(strcmp(command,"info") == 0)
  {
    cocobot_console_send_answer("Robot secondaire");
    cocobot_console_send_answer("%ld", platform_adc_get_mV(PLATFORM_ADC_VBAT));
    if(cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_NEG)
    {
      cocobot_console_send_answer("Violet");
    }
    else
    {
      cocobot_console_send_answer("Green");
    }
    void * ptr = cocobot_game_state_get_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION);
    cocobot_console_send_answer("%d", *((unsigned int *)ptr));

    cocobot_console_send_answer("%ld", cocobot_game_state_get_elapsed_time() / 1000);
    return 1;
  }

  int handled = 0;
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_seashell_console_handler);
  COCOBOT_CONSOLE_TRY_HANDLER_IF_NEEDED(handled, command, meca_fish_console_handler);
  return handled;
}

void funny_action(void)
{
  platform_gpio_set(PLATFORM_GPIO2);
  platform_gpio_clear(PLATFORM_GPIO3);
  platform_gpio_clear(PLATFORM_GPIO4);
  platform_gpio_clear(PLATFORM_GPIO5);
  platform_gpio_set(PLATFORM_GPIO6);

}

int main(void) 
{
  platform_init();
  cocobot_console_init(MCUAL_USART1, 1, 1, console_handler);
  cocobot_lcd_init();
  cocobot_position_init(4);
  cocobot_action_scheduler_init();
  cocobot_asserv_init();
  cocobot_trajectory_init(4);
  cocobot_opponent_detection_init(3);
  cocobot_game_state_init(funny_action);
  //cocobot_pathfinder_init(300, 300);

#ifdef AUSBEE_SIM
  //random shell config in simu
  _shell_configuration = rand() % 6; 
#else
  _shell_configuration = 0;
#endif

  cocobot_game_state_set_userdata(COCOBOT_GS_UD_SHELL_CONFIGURATION, &_shell_configuration); 

  //set initial position
  switch(cocobot_game_state_get_color())
  {
    case COCOBOT_GAME_STATE_COLOR_NEG:
      cocobot_position_set_x(-1300);
      cocobot_position_set_y(-20);
      cocobot_position_set_angle(-90);
      break;

    case COCOBOT_GAME_STATE_COLOR_POS:
      cocobot_position_set_x(1300);
      cocobot_position_set_y(-20);
      cocobot_position_set_angle(-90);
      break;
  }

  xTaskCreate(run_strategy, "strat", 200, NULL, 2, NULL );
  xTaskCreate(update_lcd, "blink", 200, NULL, 1, NULL );

  vTaskStartScheduler();

  return 0;
}
