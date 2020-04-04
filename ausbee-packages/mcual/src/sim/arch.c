#include <mcual.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define PERIPHERAL_TCP_PORT 10000
#define CLIENT_MAX          10

#define USART_PID(id) (id | 0x8000)
#define USART_PID_TO_ID(id) (id & 0xFF)

#define CAN_PID(id) (id | 0x9000)

typedef struct
{
  int socket;
  int init;
  int peripheral_id;

  union
  {
    struct
    {
      uint16_t id;
      uint8_t data[8];
      uint8_t data_len;

      uint32_t temp;
    }can;
  } decoder;
} mcual_arch_sim_peripheral_socket_t;

static pthread_t _peripheral_thread;
static mcual_arch_sim_peripheral_socket_t _peripherals_socket[CLIENT_MAX];
static pthread_mutex_t _mutex_network = PTHREAD_MUTEX_INITIALIZER;
static fd_set _valid_fds;

static char * _ip = "127.0.0.1";
static int _argc = 0;
static char ** _argv = NULL;
static int _port = 10000;
static int _id = 0;

static void mcual_arch_help(){
  printf("%s\n\n", _argv[0]);
  printf("Usage example:\n");
  printf("%s [(-h|--help)] [(-i|--ip)] [(-p|--port)]\n\n", _argv[0]);
  printf("Options:\n");
  printf("-h or --help: Displays this information.\n");
  printf("-i or --ip: Cocoui server IP.\n");
  printf("-p or --port: Cocoui server TCP port.\n");
  exit(1);
}

void mcual_arch_main(int argc, char *argv[])
{
  _argc = argc;
  _argv = argv;

  srand(time(NULL));
  _id = rand();

  struct option long_options[] =
  {
    {"ip", required_argument, NULL, 'i'},
    {"port", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "hi:p:", long_options, NULL)) != -1)
  {
    switch (opt)
    {
      case 'h':
        mcual_arch_help();
        break;

      case 'i':
        _ip = optarg;
        break;

      case 'p':
        _port = atoi(optarg);
        break;
    }
  }
}

void mcual_arch_sim_connect(const int pid, const char * peripheral_name)
{
  int i;

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    perror("Unable to create client socket for peripheral");
    exit(1);
  }  

  int flag = 1;
  if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0)
  {
    fprintf(stderr, "Unable to set TCP_NODELAY");
    exit(1);
  }

  struct hostent * hostinfo = gethostbyname(_ip);
  if (hostinfo == NULL)
  {
    fprintf(stderr, "Unknown host %s.\n", _ip);
    exit(1);
  }

  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(_port);
  saddr.sin_addr = *(struct in_addr *) hostinfo->h_addr;

  printf("Trying to connect: %s:%d (%s)\n", inet_ntoa(saddr.sin_addr), _port, peripheral_name);
  if(connect(sock,(struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) < 0)
  {
    perror("connect");
    close(sock);
  }
  else
  {
    printf(" [*] Success\n");
    for(i = 0; i < CLIENT_MAX; i += 1)
    {
      if(_peripherals_socket[i].socket == -1)
      {
        _peripherals_socket[i].socket = sock;
        _peripherals_socket[i].init = 1;
        _peripherals_socket[i].peripheral_id = pid;

        //send data to peripheral
        FD_SET(sock, &_valid_fds);

        write(_peripherals_socket[i].socket, peripheral_name, strlen(peripheral_name));

        pthread_mutex_unlock(&_mutex_network);
        pthread_kill(_peripheral_thread, SIGUSR2);
        return;
      }
    }
    fprintf(stderr, "No free peripheral available in list\n");
  }
}

void mcual_arch_sim_handle_uart_peripheral_write(mcual_usart_id_t usart_id, uint8_t byte)
{
  pthread_mutex_lock(&_mutex_network);

  //try to find usart slot (if client is connected)
  int i;
  for(i = 0; i < CLIENT_MAX; i += 1)
  {
    if((_peripherals_socket[i].socket != -1) && (_peripherals_socket[i].init) && (_peripherals_socket[i].peripheral_id == USART_PID(usart_id)))
    {
      //send data to peripheral
      pthread_mutex_unlock(&_mutex_network);
      char buffer[64];
      snprintf(buffer, sizeof(buffer), "%02x\r\n",
              byte
             );
      write(_peripherals_socket[i].socket, buffer, strlen(buffer));
      return;

    }
  }
  
  //no opened connection
  char buf[32];
  snprintf(buf, sizeof(buf), "U%d<<", usart_id);
  mcual_arch_sim_connect(USART_PID(usart_id), buf);
  pthread_mutex_unlock(&_mutex_network);
}

void mcual_arch_sim_handle_can_peripheral_write(const mcual_can_frame_t * const frame)
{
  pthread_mutex_lock(&_mutex_network);

  //try to find usart slot (if client is connected)
  int i;
  for(i = 0; i < CLIENT_MAX; i += 1)
  {
    if((_peripherals_socket[i].socket != -1) && (_peripherals_socket[i].init) && (_peripherals_socket[i].peripheral_id == CAN_PID(0)))
    {
      //send data to peripheral
      pthread_mutex_unlock(&_mutex_network);
      char buffer[64];
      snprintf(buffer, sizeof(buffer), "%08x:%01x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
              frame->id,
              frame->data_len,
              frame->data[0],
              frame->data[1],
              frame->data[2],
              frame->data[3],
              frame->data[4],
              frame->data[5],
              frame->data[6],
              frame->data[7]
             );

      write(_peripherals_socket[i].socket, buffer, strlen(buffer));
      return;
    }
  }
  
  //no opened connection
  char buf[32];
  snprintf(buf, sizeof(buf), "C0<<");
  mcual_arch_sim_connect(CAN_PID(0), buf);
  pthread_mutex_unlock(&_mutex_network);
}

static void * mcual_arch_sim_handle_peripherals(void * args)
{
  (void)args;
 
  //set signal mask for pselect
  sigset_t sig_mask;
  sigfillset(&sig_mask);
  sigdelset(&sig_mask, SIGUSR2);

  while(1)
  {
    //wait for an event
    pthread_mutex_lock(&_mutex_network);
    fd_set readable_fds = _valid_fds;
    pthread_mutex_unlock(&_mutex_network);
    if(pselect(FD_SETSIZE, &readable_fds, NULL, NULL, NULL, &sig_mask) < 0)
    {
      if(errno != EINTR)
      {
        perror("Unable to select readable fs for peripheral");
        exit (EXIT_FAILURE);
      }
    }

    //check if event is from an existing connection
    pthread_mutex_lock(&_mutex_network);
    int i;
    for(i = 0; i < CLIENT_MAX; i += 1)
    {
      if(_peripherals_socket[i].socket != -1)
      {
        if(FD_ISSET(_peripherals_socket[i].socket, &readable_fds))
        {
          //we have a winner !
          char buf[32];
          int r = read(_peripherals_socket[i].socket, buf, sizeof(buf) - 1);
          if(r <= 0)
          {
            //he is gone :'(
            close(_peripherals_socket[i].socket);
            FD_CLR(_peripherals_socket[i].socket, &_valid_fds);
            _peripherals_socket[i].socket = -1;
            _peripherals_socket[i].init = 0;
          }
          else
          {
            switch(_peripherals_socket[i].peripheral_id & 0xF000)
            {
              case 0x8000: //UART
                {
                  int j;
                  for(j = 0; j < r; j += 1)
                  {
                    //send data to freeRTOS
                    mcual_usart_recv_from_network(USART_PID_TO_ID(_peripherals_socket[i].peripheral_id), buf[j]);
                  }
                }
                break;

              case 0x9000: //can
                {
                  int j;
                  for(j = 0; j < r; j += 1)
                  {
                    //decode incoming data
                    if((buf[j] >= '0') && (buf[j] <= '9'))
                    {
                      _peripherals_socket[i].decoder.can.temp *= 16;
                      _peripherals_socket[i].decoder.can.temp += buf[j] - '0';
                    }
                    else if((buf[j] >= 'A') && (buf[j] <= 'F'))
                    {
                      _peripherals_socket[i].decoder.can.temp *= 16;
                      _peripherals_socket[i].decoder.can.temp += buf[j] - 'A' + 10;
                    }
                    else if((buf[j] >= 'a') && (buf[j] <= 'f'))
                    {
                      _peripherals_socket[i].decoder.can.temp *= 16;
                      _peripherals_socket[i].decoder.can.temp += buf[j] - 'a' + 10;
                    }
                    else if(buf[j] == '/')
                    {
                      _peripherals_socket[i].decoder.can.id = _peripherals_socket[i].decoder.can.temp;
                      _peripherals_socket[i].decoder.can.data_len = 0;
                      _peripherals_socket[i].decoder.can.temp = 0;
                    }
                    else if(buf[j] == ':')
                    {
                      _peripherals_socket[i].decoder.can.data[_peripherals_socket[i].decoder.can.data_len] = _peripherals_socket[i].decoder.can.temp;
                      _peripherals_socket[i].decoder.can.data_len += 1;
                      _peripherals_socket[i].decoder.can.temp = 0;
                    }
                    else if(buf[j] == '\n')
                    {
                      _peripherals_socket[i].decoder.can.data[_peripherals_socket[i].decoder.can.data_len] = _peripherals_socket[i].decoder.can.temp;
                      _peripherals_socket[i].decoder.can.data_len += 1;
                      _peripherals_socket[i].decoder.can.temp = 0;

                      mcual_can_frame_t frame;
                      frame.id = _peripherals_socket[i].decoder.can.id;
                      frame.data_len = _peripherals_socket[i].decoder.can.data_len;
                      memcpy(frame.data, _peripherals_socket[i].decoder.can.data, sizeof(frame.data));

                      mcual_can_recv_from_network(&frame);
                    }
                  }
                }
                break;

              default:
                fprintf(stderr, "PID ??? %x\r\n", _peripherals_socket[i].peripheral_id);
                break;
            }
          }
        }
      }
    }
    pthread_mutex_unlock(&_mutex_network);
  }


  return NULL;
}

void mcual_arch_sim_init_peripherals(void)
{
  int i;

  for(i = 0; i < CLIENT_MAX; i += 1)
  {
    _peripherals_socket[i].socket = -1;
  }

  //init set of valid sockets
  FD_ZERO(&_valid_fds);

  if(pthread_create(&_peripheral_thread, NULL, mcual_arch_sim_handle_peripherals, NULL) != 0)
  {
    perror("Unable to create thread for peripheral");
    exit(EXIT_FAILURE);
  }
}

void mcual_bootloader(void)
{
  //stop scheduler timer
  struct itimerval itimer, oitimer;
  itimer.it_interval.tv_sec = 0;
  itimer.it_interval.tv_usec = 0;
  itimer.it_value.tv_sec = 0;
  itimer.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &itimer, &oitimer );

  //kill clients
  int i;
  pthread_mutex_lock(&_mutex_network);
  for(i = 0; i < CLIENT_MAX; i += 1)
  {
    if(_peripherals_socket[i].socket != -1)
    {
      close(_peripherals_socket[i].socket);
      _peripherals_socket[i].socket = -1;
    }
  }
  pthread_mutex_unlock(&_mutex_network);

  //restart
  execv(_argv[0], _argv);
}

