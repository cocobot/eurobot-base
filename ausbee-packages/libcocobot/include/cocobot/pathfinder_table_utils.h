#ifndef COCOBOT_PATHFINDER_TABLE_H
#define COCOBOT_PATHFINDER_TABLE_H

#include <stdint.h>
#include "cocobot_pathfinder_config.h"

#define MAXIMUM_NODE_IN_LIST        200
#define MASK_TEMPORARY_OBSTACLE     0x0F00
#define MASK_NEW_NODE               0xFF00
#define MASK_REMOVE_ROBOT           0xF0FF

typedef enum
{
    NEW_NODE = 0x1000,
    OBSTACLE = 0x2000,
    FORBIDDEN = 0x4000,     // Zone where the robot can't go
    SOFT_OBSTACLE = 0x8000, // Zone where the robot can eventually go but must be cautious when turning
    CLOSED_LIST = 0x0001,
    FINAL_TRAJ = 0x0002,
    OPEN_LIST = 0x0004,
    TEMPORARY_ALLOWED = 0x0008,
    ROBOT = 0x0100,         // Adversary
    GAME_ELEMENT = 0x0200   // Game element, that can be removed manually.
}cocobot_nodeType_e;

typedef struct
{
    char x;
    char y;
    uint16_t nodeType; //is a cocobot_nodeType_e cast in uint16_t to be sure of the size
    float cost;
    char pX;
    char pY;
}cocobot_node_s;


/**
 * cocobot_list_t is a sorted list (using the cost of each node)
 * table[0] is the node of the list with the smallest cost
 */
typedef struct
{
    cocobot_node_s* table[MAXIMUM_NODE_IN_LIST];//Arbitrary chosen value
    uint8_t nb_elements;
} cocobot_list_s;

typedef enum
{
    RECTANGLE = 1,
    CIRCLE,
}cocobot_pathfinder_obsType_e;

/*
 * Structure used to initialize the table
 * Note : x and y position are pathfinder coordinates.
 */
typedef struct 
{
    uint16_t x_position;
    uint16_t y_position;
    uint16_t x_dimension;
    uint16_t y_dimension;
    uint16_t nodeType;
    uint8_t obsType;
    uint8_t isMask;     //Set if the obstacle can be removed during game (case of game element)
}cocobot_pathfinder_table_init_s;


/**
 * Initialize the table used for a*
 * Table width used is TABLE_WIDTH as for table length
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  
 */
//void cocobot_pathfinder_initialize_table(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], uint16_t robot_length, uint16_t robot_width);
void cocobot_pathfinder_initialize_table(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], cocobot_pathfinder_table_init_s * initStruct);

/**
 * Set start zone allowed token to true to be allowed on next reset
 */
void cocobot_pathfinder_set_start_zone_allowed();

/**
 * Set a zone where an opponent robot is located
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x : x center of the zone
 *  - y : y center of the zone 
 */
void cocobot_pathfinder_set_robot_zone(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], uint8_t x, uint8_t y);

/**
 *Unset a zone where an opponent robot was previously located
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x : x center of the zone
 *  - y : y center of the zone 
 */
void cocobot_pathfinder_unset_robot_zone(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], uint8_t x, uint8_t y);

/**
 * Set a mask on a point on the table
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x : x coordinate of the point
 *  - y : y coordinate of the point
 *  - node_type : node_type mask wanted to be applied on the point.
 *  
 */
void cocobot_pathfinder_set_point_mask(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x, int y, cocobot_nodeType_e node_type);

/**
 * Set a mask on a rectangle on the table of the same nodetype
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x_dimension : x dimension of the rectangle
 *  - y_dimension : y dimension of the rectangle
 *  - x_position : x position of the rectangle (x correspond to the smaller x coordinate of the rectangle)
 *  - y_position : y position of the rectangle (y correspond to the smaller y coordinate of the rectangle)
 *  - node_type : node_type mask wanted to be applied on the rectangle.
 *  
 */
void cocobot_pathfinder_set_rectangle_mask(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x_dimension, int y_dimension, int x_position, int y_position, cocobot_nodeType_e node_type);


/**
 * Set a mask on a circle on the table of the same nodetype
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x_center : x coordinate of the center of the circle
 *  - y_center : y coordinate of the center of the circle
 *  - radius : radius of the circle
 *  - node_type : node_type mask wanted to be applied on the circle.
 *  
 */
void cocobot_pathfinder_set_circle_mask(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x_center, int y_center, int radius, cocobot_nodeType_e node_type);


/**
 * Remove a mask from a point on the table
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x : x coordinate of the point
 *  - y : y coordinate of the point
 *  - node_type : node_type mask wanted to be removed from the point.
 *  
 */
void cocobot_pathfinder_set_point_unmask(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x, int y, cocobot_nodeType_e node_type);


/**
 * Remove a mask from a rectangle on the table of the same nodetype
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x_dimension : x dimension of the rectangle
 *  - y_dimension : y dimension of the rectangle
 *  - x_position : x position of the rectangle (x correspond to the smaller x coordinate of the rectangle)
 *  - y_position : y position of the rectangle (y correspond to the smaller y coordinate of the rectangle)
 *  - node_type : node_type mask wanted to be removed from the rectangle.
 *  
 */
void cocobot_pathfinder_set_rectangle_unmask(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x_dimension, int y_dimension, int x_position, int y_position, cocobot_nodeType_e node_type);


/**
 * Remove a mask from a circle on the table of the same nodetype
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x_center : x coordinate of the center of the circle
 *  - y_center : y coordinate of the center of the circle
 *  - radius : radius of the circle
 *  - node_type : node_type mask wanted to be removed from the circle.
 *  
 */
void cocobot_pathfinder_set_circle_unmask(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x_center, int y_center, int radius, cocobot_nodeType_e node_type);

/**
 * Reset the table to initial state
 * To be called before each new path finding
 */
void cocobot_pathfinder_reset_table(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE]);

/**
 * Set a point on the table
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x : x coordinate of the point
 *  - y : y coordinate of the point
 *  - node_type : node_type wanted for the point
 *  
 */
void cocobot_pathfinder_set_point(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x, int y, cocobot_nodeType_e node_type);

/**
 * Set a rectangle on the table of the same nodetype
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x_dimension : x dimension of the rectangle
 *  - y_dimension : y dimension of the rectangle
 *  - x_position : x position of the rectangle (x correspond to the smaller x coordinate of the rectangle)
 *  - y_position : y position of the rectangle (y correspond to the smaller y coordinate of the rectangle)
 *  - node_type : node_type wanted for the rectangle
 *  
 */
void cocobot_pathfinder_set_rectangle(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x_dimension, int y_dimension, int x_position, int y_position, cocobot_nodeType_e node_type);

/**
 * Set a circle on the table of the same nodetype
 * Arguments:
 *  - table : static 2nd array representing the playground area for a*
 *  - x_center : x coordinate of the center of the circle
 *  - y_center : y coordinate of the center of the circle
 *  - radius : radius of the circle
 *  - node_type : node_type wanted for the point
 *  
 */
void cocobot_pathfinder_set_circle(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], int x_center, int y_center, int radius, cocobot_nodeType_e node_type);

#endif //COCOBOT_PATHFINDER_TABLE_H
