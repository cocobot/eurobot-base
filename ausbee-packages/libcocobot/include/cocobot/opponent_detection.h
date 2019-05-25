#ifndef OPPONENT_DETECTION_H
#define OPPONENT_DETECTION_H

#include <canard.h>

#define COCOBOT_OPPONENT_DETECTION_DEACTIVATED  0
#define COCOBOT_OPPONENT_DETECTION_ACTIVATED    1

#define COCOBOT_OPPONENT_DETECTION_FRONT_LEFT 0
#define COCOBOT_OPPONENT_DETECTION_FRONT_RIGHT 1
#define COCOBOT_OPPONENT_DETECTION_BACK_LEFT 2
#define COCOBOT_OPPONENT_DETECTION_BACK_RIGHT 3

typedef struct
{
  float x;
  float y;
  int activated;
} cocobot_opponent_detection_fake_robot_t;

/* Initialization of the opponent detection module. Need to be called before any other action 
 * Argument:
 *  - task_priority: FreeRTOS task priority for detection thread
 */
void cocobot_opponent_detection_init(unsigned int task_priority);

void cocobot_opponent_detection_set_enable(int id, int status);

int cocobot_opponent_detection_is_in_alert(void);

void cocobot_opponent_detection_com_async(uint64_t timestamp_us);

uint8_t cocobot_opponent_detection_should_accept_transfer(uint64_t* out_data_type_signature,
                                              uint16_t data_type_id,
                                              CanardTransferType transfer_type,
                                              uint8_t source_node_id);

uint8_t cocobot_opponent_detection_on_transfer_received(CanardRxTransfer* transfer);


/**
 * Set an obstacle on map if detected
 */
void cocobot_opponent_detection_set_on_map(void);

#define COCOBOT_OPPONENT_DETECTION_ENABLE_FRONT() do \
                {\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_FRONT_LEFT, COCOBOT_OPPONENT_DETECTION_ACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_FRONT_RIGHT, COCOBOT_OPPONENT_DETECTION_ACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_BACK_LEFT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_BACK_RIGHT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                } while(0)

#define COCOBOT_OPPONENT_DETECTION_ENABLE_BACK() do \
                {\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_FRONT_LEFT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_FRONT_RIGHT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_BACK_LEFT, COCOBOT_OPPONENT_DETECTION_ACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_BACK_RIGHT, COCOBOT_OPPONENT_DETECTION_ACTIVATED);\
                } while(0)

#define COCOBOT_OPPONENT_DETECTION_DISABLE() do \
                {\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_FRONT_LEFT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_FRONT_RIGHT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_BACK_LEFT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                  cocobot_opponent_detection_set_enable(COCOBOT_OPPONENT_DETECTION_BACK_RIGHT, COCOBOT_OPPONENT_DETECTION_DEACTIVATED);\
                } while(0)

#endif// OPPONENT_DETECTION_H
