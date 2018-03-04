#ifndef COCOBOT_PATHFINDER_H
#define COCOBOT_PATHFINDER_H

#include "cocobot/pathfinder_table_utils.h"

#define TRAJECTORY_NBR_POINTS_MAX   160

#define DESTINATION_NOT_AVAILABLE   0
#define NO_ROUTE_TO_TARGET          1
#define TRAJECTORY_READY            2

/**
 * Get execution time to go from starting point to target point
 * Arguments:
 *  - starting_point_x: trajectory starting point x
 *  - starting_point_y: trajectory starting point y
 *  - target_point_x: trajectory target point x
 *  - target_point_y: trajectory target point y
 *  
 * Return Value: Time (ms) to execute the trajectory, 0 if the trajectory is not available
 **/
uint16_t cocobot_pathfinder_get_trajectory_time(int16_t starting_point_x, int16_t starting_point_y, int16_t target_point_x, int16_t target_point_y);

/**
 * Execute trajectory from starting point to target point
 * Arguments:
 *  - starting_point_x: trajectory starting point x
 *  - starting_point_y: trajectory starting point y
 *  - target_point_x: trajectory target point x
 *  - target_point_y: trajectory target point y
 *  
 * Return Value: NO_TRAJECTORY_AVAILABLE if the target_point is not reachable 0 eitherway 
 */
char cocobot_pathfinder_execute_trajectory(int16_t starting_point_x, int16_t starting_point_y, int16_t target_point_x, int16_t target_point_y);

/**
 * Allow the entire start zone for pathfinder
 */
void cocobot_pathfinder_allow_start_zone();

/**
 * Set a position for opponent robot
 * Arguments:
 *  - adv_x: x position of the opponent (considered as the x center)
 *  - adv_y: y position of the opponent (considered as the y center)
 */
void cocobot_pathfinder_set_robot(int adv_x, int adv_y);

/**
 * Remove opponent robot (as it is no longer in this position)
 * Arguments:
 *  - adv_x: x position of the opponent (considered as the x center)
 *  - adv_y: y position of the opponent (considered as the y center)
 */
void cocobot_pathfinder_remove_robot(int adv_x, int adv_y);

/**
 * Remove a game element from the pathfinder table
 * When it is removed, it not considered as an obstacle by the pathfinder.
 * WARNING : Units used here are grid units
 * This function cannot be internal, because it is called by pathfinder conf. Nevertheless it's easier to use grid coordinates.
 * Not to be called directly, only by conf
 * Arguments:
 *  - x : xposition of the game element to remove.
 *  - y : yposition of the game element to remove.
 *  - radius: Size of the game element to remove (it is considered as a circle)
 */
void cocobot_pathfinder_remove_game_element(uint8_t x, uint8_t y, uint8_t radius);

/**
 * Initialize the pathFinder
 * Arguments : 
 *  - initTable : Table containing all the elements to be used for initialisation
 */
void cocobot_pathfinder_init(cocobot_pathfinder_table_init_s * initTable);

void cocobot_pathfinder_handle_async_com(void);

#endif //COCOBOT_PATHFINDER_H
