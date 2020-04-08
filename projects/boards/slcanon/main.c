#include <FreeRTOS.h>
#include <task.h>
#include <platform.h>
#include <cocobot.h>
#include <stdio.h>
#include "motor_control.h"

#define DEBUG_OFF 0x0FFFFFFF

static uint8_t _master_board_id;
static uint32_t volatile _debug_counter_ms;

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
  switch(pid)
  {
    case COCOBOT_COM_SET_MOTOR_CFG_PID:
      {
        uint8_t board_id;
        float imax;
        float max_speed_rpm;

        //decode packet
        cocobot_com_read_B(data, len, 0, &board_id);
        cocobot_com_read_F(data, len, 1, &imax);
        cocobot_com_read_F(data, len, 5, &max_speed_rpm);

        if(board_id == COCOBOT_COM_ID)
        {
          motor_control_set_config(imax, max_speed_rpm);

          //send data back
          motor_control_get_config(&imax, &max_speed_rpm);
          cocobot_com_send(COCOBOT_COM_SET_MOTOR_CFG_PID, "BFF", COCOBOT_COM_ID, (double)imax, (double)max_speed_rpm);

        }
      }
      break;

    case COCOBOT_COM_REQ_MOTOR_CFG_PID:
      {
        float imax;
        float max_speed_rpm;

        motor_control_get_config(&imax, &max_speed_rpm);
        cocobot_com_send(COCOBOT_COM_SET_MOTOR_CFG_PID, "BFF", COCOBOT_COM_ID, (double)imax, (double)max_speed_rpm);
      }
      break;

    case COCOBOT_COM_SET_MOTOR_PID:
      //only accept order from brain (or cocoui)
      if(cocobot_com_get_src() == _master_board_id)
      {
        uint8_t enable;
        float left;
        float right;

        //decode packet
        cocobot_com_read_B(data, len, 0, &enable);
        cocobot_com_read_F(data, len, 1, &left);
        cocobot_com_read_F(data, len, 5, &right);

        //assign set point
        switch(COCOBOT_COM_ID)
        {
          case COCOBOT_SLCANON_ID:
          case COCOBOT_PLCANON_ID:
            //left motor
            motor_control_set_setpoint(enable, left);
            break;

          case COCOBOT_SRCANON_ID:
          case COCOBOT_PRCANON_ID:
            //right motor
            motor_control_set_setpoint(enable, right);
            break;
        }
      }
      break;

    case COCOBOT_COM_REQ_MOTOR_DBG_PID:
      {
        int i;
        uint8_t board_id;
        
        //active debug if requested id is ours
        for(i = 0; i < 2; i += 1) 
        {
          cocobot_com_read_B(data, len, i, &board_id);
          if(board_id == COCOBOT_COM_ID)
          {
            _debug_counter_ms = 0;
          }
        }
      }
      break;

    case COCOBOT_COM_SET_MOTOR_MST_PID:
      {
        int i;
        uint8_t board_id;
        
        //change canon master for setpoint orders
        for(i = 0; i < 2; i += 1) 
        {
          cocobot_com_read_B(data, len, i, &board_id);
          if(board_id == COCOBOT_COM_ID)
          {
            cocobot_com_read_B(data, len, 2, &_master_board_id);
          }
        }
      }
      break;
  }
}

/**
 * @brief Main task for anon  board
 * @param arg   Not used
 */
void main_loop(void * arg)
{
  (void)arg;

  uint64_t timestamp_us = 0;
  TickType_t tick = xTaskGetTickCount();

  //set debug to OFF
  _debug_counter_ms = DEBUG_OFF;
  int debug_cnt = 0;

  while(1)
  {
    //compute timestamp in us
    TickType_t ntick = xTaskGetTickCount();
    uint64_t ms = (((uint64_t)(ntick - tick)) & 0xFFFFUL) * 1000UL;
    timestamp_us += ms;
    tick = ntick;

    //send debug data if requested
    if(_debug_counter_ms < 1500) {
      if(debug_cnt > 50) {
        debug_cnt = 0;

        fprintf(stderr, "\ntest dbg\n");
        cocobot_com_send(COCOBOT_COM_MOTOR_DBG_PID, "BBBFD", 
                         COCOBOT_COM_ID, 
                         _master_board_id,
                         motor_control_is_enabled(),
                         (double)motor_control_get_setpoint(),
                         motor_control_get_pwm()
                        );
      }
      else {
        debug_cnt += 1;
      }
      _debug_counter_ms += ms / 1000;
    }

    //run motor control algorithm
		motor_control_process_event(timestamp_us);
    vTaskDelay(5 / portTICK_PERIOD_MS); 
  }
}

int main(int argc, char *argv[]) 
{
#ifdef AUSBEE_SIM
  mcual_arch_main(argc, argv);
#else
  (void)argc;
  (void)argv;
#endif

  //init statis variables
  switch(COCOBOT_COM_ID)
  {
    case COCOBOT_SLCANON_ID:
    case COCOBOT_SRCANON_ID:
      //secondary robot, only accept orders from Sbrain
      _master_board_id = COCOBOT_SBRAIN_ID;
      break;

    case COCOBOT_PLCANON_ID:
    case COCOBOT_PRCANON_ID:
      //main robot, only accept orders from Pbrain
      _master_board_id = COCOBOT_PBRAIN_ID;
      break;
  }

	//initialisations of mcual and libcocobot
	platform_init();

  cocobot_com_init(PLATFORM_USART_USER, 2, 2, com_handler);
	motor_control_init();
	motor_control_set_config(0.0025,400);
	motor_control_set_setpoint(0, 0);

  //run main loop
  xTaskCreate(main_loop, "mloop", 600, NULL, 1, NULL );
  vTaskStartScheduler();

	return 0;
}
