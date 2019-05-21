#ifndef COCOBOT_ARM_ACTION_H
#define COCOBOT_ARM_ACTION_H

// Function to call before any other function in this module
void cocobot_arm_action_init(void);
// arm_id value should be between 0 and 3
void cocobot_arm_action_prendre_palais_sol(int arm_id, float x, float y);
void cocobot_arm_action_pos_stockage_palais(int arm_id);
void cocobot_arm_action_prise_distributeur(int arm_id, float angle);
void cocobot_arm_action_orientation(int arm_id, int side);
void cocobot_arm_action_prise_goldenium(int arm_id, float angle);
void cocobot_arm_action_repos_goldenium(int arm_id);
void cocobot_arm_action_prise_bluenium(int arm_id, float angle);
void cocobot_arm_action_depose_balance(int arm_id, float angle);
void cocobot_arm_action_depose_accelerateur_particules(int arm_id, float angle, float distance);
void cocobot_arm_action_depose_case(int arm_id, float angle);

#endif /* COCOBOT_ARM_ACTION_H */
