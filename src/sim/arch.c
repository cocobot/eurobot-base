#include <mcual.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define PERIPHERAL_TCP_PORT 10000
#define CLIENT_MAX          10

typedef struct
{
  int socket;
  int init;
  mcual_usart_id_t usart_id;
} mcual_arch_sim_peripheral_socket_t;

mcual_arch_sim_peripheral_socket_t peripherals_socket[CLIENT_MAX];

void mcual_arch_sim_handle_uart_peripheral_write(mcual_usart_id_t usart_id, uint8_t byte)
{
  //try to find usart slot (if client is connected)
  int i;
  for(i = 0; i < CLIENT_MAX; i += 1)
  {
    if((peripherals_socket[i].socket != -1) && (peripherals_socket[i].init) && (peripherals_socket[i].usart_id == usart_id))
    {
      //send data to peripheral
      write(peripherals_socket[i].socket, &byte, 1);
    }
  }
}

static void * mcual_arch_sim_handle_peripherals(void * args)
{
  (void)args;

  //create socket server
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket < 0)
  {
    perror("Unable to create server socket for peripheral");
    exit(EXIT_FAILURE);
  }

  //create socket info storage
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(PERIPHERAL_TCP_PORT);
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //configure socket in order to prevent failed bind in quick(burger king?) restart
  int on = 1;
  if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
  {
    perror("Unable to set SO_REUSEADDR for peripheral server");
    exit(EXIT_FAILURE);
  }

  //bind socket & storage
  if(bind(server_socket, (struct sockaddr *) &saddr, sizeof (saddr)) < 0)
  {
    perror("Unable to bind socket to storage for peripheral");
    exit(EXIT_FAILURE);
  }

  //declare we are ready to accept incoming connection
  if(listen(server_socket, 1) < 0)
  {
    perror("Unable to listen from socket for peripheral");
    exit (EXIT_FAILURE);
  }

  //init set of valid sockets
  fd_set valid_fds;
  FD_ZERO(&valid_fds);
  FD_SET(server_socket, &valid_fds);

  //set signal mask for pselect
  sigset_t sig_mask;
  sigfillset(&sig_mask);
  sigdelset(&sig_mask, SIGUSR2);

  while(1)
  {
    //wait for an event
    fd_set readable_fds = valid_fds;
    if(pselect(FD_SETSIZE, &readable_fds, NULL, NULL, NULL, &sig_mask) < 0)
    {
      perror("Unable to select readable fs for peripheral");
      exit (EXIT_FAILURE);
    }

    //check if event is a new incoming connection
    if(FD_ISSET(server_socket, &readable_fds))
    {
      int new_socket;
      struct sockaddr_in new_saddr;
      int size = sizeof(new_saddr);

      //accept the incoming connection
      new_socket = accept(server_socket, (struct sockaddr *)&new_saddr,&size);
      if(new_socket < 0)
      {
        perror("Unable to accept the incoming connection for peripheral");
        exit(EXIT_FAILURE);
      }

      //find free slot
      int i;
      for(i = 0; i < CLIENT_MAX; i += 1)
      {
        if(peripherals_socket[i].socket == -1)
        {
          peripherals_socket[i].socket = new_socket;
          peripherals_socket[i].init = 0;
          FD_SET(new_socket, &valid_fds);
          break;
        }
      }
      
      //if no empty slot has been found, close the socket
      if(i >= CLIENT_MAX)
      {
        close(new_socket);
      }
    }

    //check if event is from an existing connection
    int i;
    for(i = 0; i < CLIENT_MAX; i += 1)
    {
      if(peripherals_socket[i].socket != -1)
      {
        if(FD_ISSET(peripherals_socket[i].socket, &readable_fds))
        {
          //we have a winner !
          char buf[32];
          int r = read(peripherals_socket[i].socket, buf, sizeof(buf));
          if(r <= 0)
          {
            //he is gone :'(
            close(peripherals_socket[i].socket);
            FD_CLR(peripherals_socket[i].socket, &valid_fds);
            peripherals_socket[i].socket = -1;
          }
          else
          {
            buf[r] = 0;
            if(!peripherals_socket[i].init)
            {
              //if never init find usart id
              if(strncmp(buf, "USART0", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART0;
                peripherals_socket[i].init = 1;
              }
              else if(strncmp(buf, "USART1", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART1;
                peripherals_socket[i].init = 1;
              }
              else if(strncmp(buf, "USART2", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART2;
                peripherals_socket[i].init = 2;
              }
              else if(strncmp(buf, "USART3", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART3;
                peripherals_socket[i].init = 3;
              }
              else if(strncmp(buf, "USART4", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART4;
                peripherals_socket[i].init = 1;
              }
              else if(strncmp(buf, "USART5", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART5;
                peripherals_socket[i].init = 1;
              }
              else if(strncmp(buf, "USART6", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART6;
                peripherals_socket[i].init = 1;
              }
              else if(strncmp(buf, "USART7", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART7;
                peripherals_socket[i].init = 1;
              }
              else if(strncmp(buf, "USART8", 6) == 0)
              {
                peripherals_socket[i].usart_id = MCUAL_USART8;
                peripherals_socket[i].init = 1;
              }
            }
            else
            {
              int j;
              for(j = 0; j < r; j += 1)
              {
                //send data to freeRTOS
                mcual_usart_recv_from_network(peripherals_socket[i].usart_id, buf[j]);
              }
            }
          }
        }
      }
    }
  }

  return NULL;
}

void mcual_arch_sim_init_peripherals(void)
{
  int i;

  for(i = 0; i < CLIENT_MAX; i += 1)
  {
    peripherals_socket[i].socket = -1;
  }

  pthread_t peripheral_thread;
  if(pthread_create(&peripheral_thread, NULL, mcual_arch_sim_handle_peripherals, NULL) != 0)
  {
    perror("Unable to create thread for peripheral");
    exit(EXIT_FAILURE);
  }
}
