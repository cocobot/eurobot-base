#ifndef COCOBOT_ARM_ACTION_H
#define COCOBOT_ARM_ACTION_H

typedef enum {
  PLIERS_FRONT_RIGHT = 0,
  PLIERS_FRONT_CENTER,
  PLIERS_FRONT_LEFT,
  PLIERS_BACK_RIGHT,
  PLIERS_BACK_CENTER,
  PLIERS_BACK_LEFT,
}pliers_e;

typedef enum {
  PLIERS_CARRIER_FRONT = 0x10,
  PLIERS_CARRIER_BACK,
}pliersCarrier_e;

typedef enum {
  WINDSOCK_LEFT = 0x100,
  WINDSOCK_RIGHT,
}windsock_e;

// Function to call before any other function in this module
void cocobot_arm_action_init(void);
// arm_id value should be between 0 and 3
void cocobot_arm_action_flag_init();
void cocobot_arm_action_flag_down();
void cocobot_arm_action_flag_up();

void cocobot_arm_action_windsock_down(windsock_e windsock);
void cocobot_arm_action_windsock_up(windsock_e windsock);

void cocobot_arm_action_pliers_open(pliers_e pliers);
void cocobot_arm_action_pliers_close(pliers_e pliers);

void cocobot_arm_action_pliers_up(pliersCarrier_e pliersCarrier);
void cocobot_arm_action_pliers_down(pliersCarrier_e pliersCarrier);


void cocobot_arm_action_prendre_palais_sol(int arm_id, float x, float y);
void cocobot_arm_action_pos_stockage_palais(int arm_id);
void cocobot_arm_action_prise_distributeur(int arm_id, float angle);
void cocobot_arm_action_repos_vide(int arm_id);
void cocobot_arm_action_repos_normal(int arm_id);
void cocobot_arm_action_depose_balance(int arm_id, float angle);

#endif /* COCOBOT_ARM_ACTION_H */
