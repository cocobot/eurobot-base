#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_GAME_STATE

#include <stdlib.h>
#include <cocobot.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#ifdef AUSBEE_SIM
# include <time.h> //for random seed
#endif
#include <platform.h>
#include "uavcan/cocobot/GameState.h"

#define USER_DATA_SIZE 16
#define SCORE_DIGIT 3

static cocobot_game_state_funny_action_t _funny_action;
static TimerHandle_t _end_match_timer;
static cocobot_game_state_color_t _color;
static void * _userdata[USER_DATA_SIZE];
static uint8_t _starter_removed;
static TickType_t _start_time = 0;
static int _score = 0;
static uint64_t _next_1hz_service_at;

void cocobot_game_state_add_points_to_score(int _toAdd)
{
    _score += _toAdd;
}

int cocobot_game_state_getScore()
{
    return _score;
}

void cocobot_game_state_set_score(int score)
{
   _score = score;
}


static void cocobot_game_state_match_ended_event(TimerHandle_t xTimer)
{
  (void)xTimer;

  cocobot_asserv_set_state(COCOBOT_ASSERV_DISABLE);

  if(_funny_action != NULL)
  {
    _funny_action();
  }
}

void cocobot_game_state_init(cocobot_game_state_funny_action_t funny_action)
{
  _funny_action = funny_action;

  _starter_removed = 0;
  _next_1hz_service_at = 0;

#ifdef AUSBEE_SIM
  //random color in simu
  srand(time(NULL));
  _color = COCOBOT_GAME_STATE_COLOR_NEG;
  _color = rand() % 2; 
#else
  if(platform_gpio_get(PLATFORM_GPIO_COLOR))
  {
    _color = COCOBOT_GAME_STATE_COLOR_POS;
  }
  else
  {
    _color = COCOBOT_GAME_STATE_COLOR_NEG;
  }
#endif

  //create timer for the game duration
  _end_match_timer = xTimerCreate("end_match", COCOBOT_GAME_DURATION / portTICK_PERIOD_MS, pdFALSE, NULL, cocobot_game_state_match_ended_event);
}

uint8_t cocobot_game_state_is_starter_removed(void)
{
  return _starter_removed;
}

void cocobot_game_state_wait_for_starter_removed(void)
{
#ifdef AUSBEE_SIM
  //no starter on simu. Just wait 3s
  vTaskDelay(3000 / portTICK_PERIOD_MS);
#else
  while(platform_gpio_get(PLATFORM_GPIO_STARTER))
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  _starter_removed = 1;
#endif

  _start_time = xTaskGetTickCount();

  xTimerStart(_end_match_timer, 0);

  cocobot_asserv_set_state(COCOBOT_ASSERV_ENABLE);
}

cocobot_game_state_color_t cocobot_game_state_get_color(void)
{
  return _color;
}

int32_t cocobot_game_state_get_elapsed_time(void)
{
  if(_start_time == 0)
  {
    return 0;
  }

  return (xTaskGetTickCount() - _start_time) * portTICK_PERIOD_MS;
}

void cocobot_game_state_set_userdata(unsigned int id, void * data)
{
  _userdata[id] = data;
}

void * cocobot_game_state_get_userdata(unsigned int id)
{
  return _userdata[id];
}

void cocobot_game_state_com_async(uint64_t timestamp_us)
{
  if (timestamp_us >= _next_1hz_service_at)
  {
    _next_1hz_service_at = timestamp_us + 1000000;

    uavcan_cocobot_GameState gs;

    gs.battery = platform_adc_get_mV(PLATFORM_ADC_VBAT);
    gs.time = cocobot_game_state_get_elapsed_time();
    gs.color = cocobot_game_state_get_color() == COCOBOT_GAME_STATE_COLOR_POS;
    gs.score = cocobot_game_state_getScore();

    void * buf = pvPortMalloc(UAVCAN_COCOBOT_GAMESTATE_MAX_SIZE); 
    if(buf != NULL) 
    {
      static uint8_t transfer_id;

      const int size = uavcan_cocobot_GameState_encode(&gs, buf);
      cocobot_com_broadcast(UAVCAN_COCOBOT_GAMESTATE_SIGNATURE,
                            UAVCAN_COCOBOT_GAMESTATE_ID,
                            &transfer_id,
                            CANARD_TRANSFER_PRIORITY_LOW,
                            buf,
                            (uint16_t)size);
      vPortFree(buf);
    }
  }
}
#endif
