#ifndef STRAT_HUT_H
#define STRAT_HUT_H

#define STRAT_HUT_VIOLET_LEFT     0
#define STRAT_HUT_VIOLET_RIGHT    1
#define STRAT_HUT_GREEN_LEFT      2
#define STRAT_HUT_GREEN_RIGHT     3

unsigned int strat_hut_get_score(int hut);
float strat_hut_get_x(int hut);
float strat_hut_get_y(int hut);
float strat_hut_get_a(int hut);
float strat_hut_get_exec_time(int hut);
float strat_hut_get_success_proba(int hut);
int strat_hut_action(int hut);

#endif// STRAT_HUT_H
