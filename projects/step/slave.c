#include <avr/io.h>
#include <avr/interrupt.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include "slave.h"

#define SLAVE_NO_DATA 0x42
#define SLAVE_WRITE   0xF0

typedef enum 
{
  SLAVE_STATE_WAIT_CS,
  SLAVE_STATE_WAIT_ORDER,
  SLAVE_STATE_WRITE_WAIT_ADDR,
  SLAVE_STATE_READ_WAIT_ADDR,
  SLAVE_STATE_WRITE_DATA,
  SLAVE_STATE_READ_DATA,
} slave_state_t;

static volatile board_state_t _memory;
static volatile uint8_t * _ptrmemory;
static volatile slave_state_t _state;
static volatile unsigned int _addr;
static SemaphoreHandle_t _xSemaphore;

void slave_init(void)
{
  _ptrmemory = (uint8_t *) &_memory;
  _state = SLAVE_STATE_WAIT_CS;
  _addr = 0;
  _xSemaphore = xSemaphoreCreateBinary();

  // port
  PORTD.DIRCLR = (1 << 4) | (1 << 5) | (1 << 7);
  PORTD.DIRSET = 1 << 6;
  PORTD.PIN4CTRL = PORT_ISC_BOTHEDGES_gc;
  PORTD.INT0MASK = (1 << 4);
  PORTD.INTCTRL = PORT_INT0LVL_HI_gc;
  
  //init spi
  SPID.CTRL = SPI_ENABLE_bm | SPI_MODE_3_gc;
  SPID.INTCTRL = SPI_INTLVL_HI_gc;
}

//CS down
ISR(PORTD_INT0_vect)
{
  if(PORTD.IN & (1 << 4))
  {
    //static BaseType_t xHigherPriorityTaskWoken;
    //xSemaphoreGiveFromISR(_xSemaphore, &xHigherPriorityTaskWoken);
  }
  else
  {
    SPID.DATA = SLAVE_NO_DATA;
    _state = SLAVE_STATE_WAIT_ORDER;
  }
}

//SPI Data
ISR(SPID_INT_vect)
{
  static volatile uint8_t read;
  read = SPID.DATA;
  switch(_state)
  {
    case SLAVE_STATE_WAIT_ORDER:
      SPID.DATA = SLAVE_NO_DATA;
      if(read == SLAVE_WRITE)
      {
        _state = SLAVE_STATE_WRITE_WAIT_ADDR;
      }
      else
      {
        _state = SLAVE_STATE_READ_WAIT_ADDR;
      }
      break;

    case SLAVE_STATE_WRITE_WAIT_ADDR:
      SPID.DATA = SLAVE_NO_DATA;
      _state = SLAVE_STATE_WRITE_DATA;
      _addr = read;
      break;

    case SLAVE_STATE_WRITE_DATA:
      SPID.DATA = SLAVE_NO_DATA;
      if(_addr < sizeof(_memory))
      {
        _ptrmemory[_addr] = read;
      }
      _state = SLAVE_STATE_WRITE_DATA;
      _addr += 1;
      break;

    case SLAVE_STATE_READ_WAIT_ADDR:
      SPID.DATA = SLAVE_NO_DATA;
      _state = SLAVE_STATE_READ_DATA;
      _addr = read;
      break;

    case SLAVE_STATE_READ_DATA:
      if(_addr < sizeof(_memory))
      {
        SPID.DATA = _ptrmemory[_addr];
      }
      else
      {
        SPID.DATA = SLAVE_NO_DATA;
      }
      _state = SLAVE_STATE_READ_DATA;
      _addr += 1;
      break;

    default:
      SPID.DATA = SLAVE_NO_DATA;
  }
}
