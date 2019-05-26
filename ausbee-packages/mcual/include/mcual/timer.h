#ifndef MCUAL_TIMER_H
#define MCUAL_TIMER_H

#include <stdint.h>

typedef uint32_t mcual_timer_t;
typedef uint32_t mcual_timer_channel_t;

#define MCUAL_TIMER0 ((mcual_timer_t) 0)
#define MCUAL_TIMER1 ((mcual_timer_t) 1)
#define MCUAL_TIMER2 ((mcual_timer_t) 2)
#define MCUAL_TIMER3 ((mcual_timer_t) 3)
#define MCUAL_TIMER4 ((mcual_timer_t) 4)
#define MCUAL_TIMER5 ((mcual_timer_t) 5)
#define MCUAL_TIMER6 ((mcual_timer_t) 6)
#define MCUAL_TIMER7 ((mcual_timer_t) 7)
#define MCUAL_TIMER8 ((mcual_timer_t) 8)
#define MCUAL_TIMER9 ((mcual_timer_t) 9)

#define MCUAL_TIMER_CHANNEL0   (((mcual_timer_channel_t)1) << 0)
#define MCUAL_TIMER_CHANNEL1   (((mcual_timer_channel_t)1) << 1)
#define MCUAL_TIMER_CHANNEL2   (((mcual_timer_channel_t)1) << 2)
#define MCUAL_TIMER_CHANNEL3   (((mcual_timer_channel_t)1) << 3)
#define MCUAL_TIMER_CHANNEL4   (((mcual_timer_channel_t)1) << 4)


void mcual_timer_init(mcual_timer_t timer, int32_t freq_Hz);
void mcual_timer_init_encoder(mcual_timer_t timer);
void mcual_timer_init_servo(mcual_timer_t timer);
void mcual_timer_enable_channel(mcual_timer_t timer, mcual_timer_channel_t channel);
void mcual_timer_set_duty_cycle(mcual_timer_t timer, mcual_timer_channel_t channel, uint32_t duty_cyle);
uint32_t mcual_timer_get_value(mcual_timer_t timer);
uint32_t mcual_timer_get_timer_tick(mcual_timer_t timer);

#ifdef AUSBEE_SIM
void mcual_timer_set_value(mcual_timer_t timer, uint32_t cnt);
#endif //AUSBEE_SIM


#endif // MCUAL_TIMER_H

