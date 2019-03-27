#include <pthread.h>
#include <stdio.h>
#include <mcual.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>


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
  mcual_arch_request("ARCH", 0, "bad request=%s", cmd);
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
