#ifndef MECA_CRIMP_H
#define MECA_CRIMP_H

typedef enum
{
  MECA_CRIMP_VERTICAL_GROUND,
  MECA_CRIMP_VERTICAL_FENCE,
  MECA_CRIMP_VERTICAL_F1,
} meca_crimp_vertical_t;

void meca_crimp_init(void);
void meca_crimp_open(void);
void meca_crimp_close(void);
void meca_crimp_disable(void);

//void meca_crimp_open(unsigned int number);
//void meca_crimp_hold(unsigned int number);
//void meca_crimp_vertical(meca_crimp_vertical_t v);

int meca_crimp_console_handler(const char * command);

#endif// MECA_CRIMP_H
