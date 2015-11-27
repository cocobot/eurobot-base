#include <mcual.h>
#include "extApi.h"



void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  (void)source;
  (void)target_freq_kHz;

  //create thread for simulated peripherals
  mcual_arch_sim_init_peripherals();


  int clientID = simxStart((simxChar*)"127.0.0.1", 1234, 1, 1, 2000, 5);
	if (clientID!=-1)
  {
    printf("Oh yeah !\n");
    while (simxGetConnectionId(clientID)!=-1)
    {

      extApi_sleepMs(5);
    }
  }
  else
  {
    printf("Echec de la connexion a VREP\n");
  }
}
