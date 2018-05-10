#ifndef PATHFINDER_CONFIG_H
#define PATHFINDER_CONFIG_H

//Table dimension (mm)
#define TABLE_LENGTH            3000
#define TABLE_WIDTH             2000

//Robot dimensions (mm)
#define ROBOT_LENGTH            232
#define ROBOT_WIDTH             281

//Opponent half diag dimension
#define OPPONENT_HALF_DIAG      225

//Grid size
#define GRID_SIZE               50

//Table dimension (grid type)
#define TABLE_LENGTH_GRID       60
#define TABLE_WIDTH_GRID        40

// (ROBOT_WIDTH / GRID_SIZE) / 2
// rounded to superior
#define ROBOT_HALF_WIDTH_GRID   3

// (sqrt((ROBOT_WIDTH * ROBOT_WIDTH) + (ROBOT_LENGTH * ROBOT_LENGTH))/ GRID_SIZE) / 2
// rounded to superior
#define ROBOT_HALF_DIAG_GRID    4

//Depart zone position and dimension (grid type)
//WARNING, Calculated with a GRID_SIZE = 50
#define NEGATIVE_DEPART_ZONE_X_DIMENSION    8
#define NEGATIVE_DEPART_ZONE_Y_DIMENSION    13
#define NEGATIVE_DEPART_ZONE_X_POSITION     0
#define NEGATIVE_DEPART_ZONE_Y_POSITION     0

#define POSITIVE_DEPART_ZONE_X_DIMENSION    8
#define POSITIVE_DEPART_ZONE_Y_DIMENSION    13
#define POSITIVE_DEPART_ZONE_X_POSITION     52
#define POSITIVE_DEPART_ZONE_Y_POSITION     0

//Douglas-Peucker threshold
#define DP_MINIMUM_THRESHOLD                1.0

//Enumeration for removable game element
typedef enum
{
    CUBE_CROSS_0 = 0,
    CUBE_CROSS_1,
    CUBE_CROSS_2,
    CUBE_CROSS_3,
    CUBE_CROSS_4,
    CUBE_CROSS_5,
}gameElement_e;

void cocobot_pathfinder_conf_remove_game_element(gameElement_e element);


#endif
