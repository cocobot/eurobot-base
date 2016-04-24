#ifndef MECA_FISH_H
#define MECA_FISH_H

void meca_fish_init(void);
void meca_fish_close(int wait);

int meca_fish_console_handler(const char * command);

#endif// MECA_FISH_H
