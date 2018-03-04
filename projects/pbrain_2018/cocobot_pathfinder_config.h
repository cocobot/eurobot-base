#ifndef PATHFINDER_CONFIG_H
#define PATHFINDER_CONFIG_H

//Table dimension (mm)
#define TABLE_LENGTH 3000
#define TABLE_WIDTH 2000

//Robot dimensions (mm)
#define ROBOT_LENGTH    300
#define ROBOT_WIDTH     300

//Opponent half diag dimension
#define OPPONENT_HALF_DIAG 225

//Grid size
#define GRID_SIZE 50

//Table dimension (grid type)
#define TABLE_LENGTH_GRID   60
#define TABLE_WIDTH_GRID    40

// (ROBOT_WIDTH / GRID_SIZE) / 2
// rounded to superior
#define ROBOT_HALF_WIDTH_GRID   3

// (sqrt((ROBOT_WIDTH * ROBOT_WIDTH) + (ROBOT_LENGTH * ROBOT_LENGTH))/ GRID_SIZE) / 2
// rounded to superior
#define ROBOT_HALF_DIAG_GRID    5

#endif
