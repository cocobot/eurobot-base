void meca_init(void);

void meca_prepare(void);
void meca_take(void);

void meca_com_handler(uint16_t pid, uint8_t * data, uint32_t len);
