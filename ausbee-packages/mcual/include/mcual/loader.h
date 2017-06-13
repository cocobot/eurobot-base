#ifndef MCUAL_LOADER_H
#define MCUAL_LOADER_H

void mcual_loader_init(mcual_usart_id_t usart_id, void (*event)(void));
void mcual_loader_run(void);

#endif // MCUAL_LOADER_H
