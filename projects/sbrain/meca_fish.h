#ifndef MECA_FISH_H
#define MECA_FISH_H

void meca_fish_init(void);
void meca_fish_close(int wait);


void meca_fish_prepare(int wait);
void meca_fish_walk(int wait);

void meca_fish_disable(void);

int meca_fish_console_handler(const char * command);

void meca_fish_sweep_left(int wait, int high);
void meca_fish_sweep_right(int wait, int high);
int meca_fish_is_catch(void);


#endif// MECA_FISH_H
