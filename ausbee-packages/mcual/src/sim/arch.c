#include <pthread.h>
#include <stdio.h>
#include <mcual.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


//Init mutex
static pthread_mutex_t _com = PTHREAD_MUTEX_INITIALIZER;
static char _buffer[4096];
static unsigned int _buffer_offset;

void mcual_arch_request(char * mod, int id, char * fmt, ...)
{
  //acquire lock
  pthread_mutex_lock(&_com);

  //send module ID
  printf(">%s:%d/", mod, id);

  //send data
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  //finalize request
  printf("\n");
  pthread_mutex_unlock(&_com);
}

void mcual_arch_parse_cmd(char * cmd)
{
  char * module = &cmd[1];
  char * module_id = &cmd[1];
  char * data = &cmd[1];
  int i;

  const int len = strlen(cmd);
  for(i = 0; i < len; i += 1)
  {
    if(cmd[i] == ':')
    {
      cmd[i] = 0;
      module_id = &cmd[i + 1];
    }
    if(cmd[i] == '/') 
    {
      cmd[i] = 0;
      data = &cmd[i + 1];
      break;
    }
  }

  //parse tokens
  if(strcmp(module, "TIMER") == 0)
  {
    if(strncmp(data, "CNT=", strlen("CNT=")) == 0)
    {
      uint32_t cnt = strtoul(data + strlen("CNT="), NULL, 10);
      mcual_timer_set_value(atoi(module_id), cnt);
    }
  }
  else if(strcmp(module, "CAN") == 0)
  {
    CanardCANFrame frame;
    char * ptr = data;
    frame.id = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data_len = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[0] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[1] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[2] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[3] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[4] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[5] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[6] = strtoul(ptr, &ptr, 16); ptr += 1;
    frame.data[7] = strtoul(ptr, &ptr, 16); ptr += 1;
    mcual_can_recv_new_frame(&frame);
  }
}

void mcual_arch_parse_buffer(void)
{
  unsigned int i;
  unsigned int start = 0;
  for(i = 0; i < _buffer_offset; i += 1)
  {
    if(_buffer[i] == '\n')
    {
      _buffer[i] = 0;
      mcual_arch_parse_cmd(&_buffer[start]);
      start = i + 1;
    }
  }
  if(start != 0)
  {
    for(i = start; i < _buffer_offset; i += 1)
    {
      _buffer[i - start] = _buffer[i]; 
    }
    _buffer_offset -= start;
  }
}


void vApplicationTickHook( void )
{
  int num = read(0, _buffer + _buffer_offset, sizeof(_buffer) - _buffer_offset);
  if(num > 0)
  {
    _buffer_offset += num;
    mcual_arch_parse_buffer();
  }
}


void mcual_arch_init(void)
{
  _buffer_offset = 0;
  setbuf(stdout, NULL);

  fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
  mcual_arch_request("ARCH", 0, "RUN");
}


void mcual_get_unique_id(uint8_t buffer[12])
{
  (void)buffer;
}


void mcual_bootloader(void)
{
}
