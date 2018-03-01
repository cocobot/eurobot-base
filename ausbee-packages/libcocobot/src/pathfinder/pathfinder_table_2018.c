#include "cocobot/pathfinder_table_utils.h"
#include "cocobot.h"
#include <string.h>
#include <math.h>

void cocobot_pathfinder_initialize_table(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], uint16_t robot_length, uint16_t robot_width)
{
    memset(table, 0, (TABLE_LENGTH / GRID_SIZE) * sizeof(cocobot_node_s));
    for(int i = 0; i < TABLE_LENGTH / GRID_SIZE; i++)
    {
        memset(table[i], 0, (TABLE_WIDTH / GRID_SIZE) * sizeof(cocobot_node_s));
        for(int j = 0; j < TABLE_WIDTH / GRID_SIZE; j++)
        {
            table[i][j].x = i;
            table[i][j].y = j;
            table[i][j].nodeType = NEW_NODE;
        }
    }
    
    uint16_t half_width = robot_width/2 + (GRID_SIZE-1);
    uint8_t grid_half_width = (half_width / GRID_SIZE) + 1;
    uint16_t half_diag = (uint16_t) (sqrtf((robot_length * robot_length) + (robot_width * robot_width)) / 2) + (GRID_SIZE-1);
    uint8_t grid_half_diag = half_diag / GRID_SIZE;

    //walls half diag = soft_obstacle
    cocobot_pathfinder_set_rectangle(table, TABLE_LENGTH / GRID_SIZE, grid_half_diag, 0, 0, SOFT_OBSTACLE);  
    cocobot_pathfinder_set_rectangle(table, TABLE_LENGTH / GRID_SIZE, grid_half_diag, 0, TABLE_WIDTH/GRID_SIZE - grid_half_diag, SOFT_OBSTACLE);  
    cocobot_pathfinder_set_rectangle(table, grid_half_diag, TABLE_WIDTH / GRID_SIZE, 0, 0, SOFT_OBSTACLE);
    cocobot_pathfinder_set_rectangle(table, grid_half_diag, TABLE_WIDTH / GRID_SIZE, TABLE_LENGTH / GRID_SIZE - grid_half_diag, 0, SOFT_OBSTACLE);  

    //Epuration station = soft_obstacle
    cocobot_pathfinder_set_rectangle(table, 1212/GRID_SIZE + (grid_half_diag * 2), 250/GRID_SIZE + grid_half_diag, 894/GRID_SIZE - grid_half_diag, 1750/GRID_SIZE - grid_half_diag, SOFT_OBSTACLE);  

    //Distributors
    cocobot_pathfinder_set_rectangle(table, 100/GRID_SIZE + grid_half_diag, 54/GRID_SIZE + (grid_half_diag * 2), 0, 867/GRID_SIZE - grid_half_diag, SOFT_OBSTACLE);
    cocobot_pathfinder_set_rectangle(table, 100/GRID_SIZE + grid_half_diag, 54/GRID_SIZE + (grid_half_diag * 2), 3000/GRID_SIZE - 100/GRID_SIZE - grid_half_diag , 867/GRID_SIZE - grid_half_diag, SOFT_OBSTACLE);
    cocobot_pathfinder_set_rectangle(table, 54/GRID_SIZE + (grid_half_diag * 2), 100/GRID_SIZE + grid_half_diag, 583/GRID_SIZE - grid_half_diag, 1900/GRID_SIZE - grid_half_diag, SOFT_OBSTACLE);
    cocobot_pathfinder_set_rectangle(table, 54/GRID_SIZE + (grid_half_diag * 2), 100/GRID_SIZE + grid_half_diag, 2363/GRID_SIZE - grid_half_diag, 1900/GRID_SIZE - grid_half_diag, SOFT_OBSTACLE);

    //Distributors
    cocobot_pathfinder_set_rectangle(table, 100/GRID_SIZE + grid_half_width, 54/GRID_SIZE + (grid_half_width * 2), 0, 867/GRID_SIZE - grid_half_width, FORBIDDEN);
    cocobot_pathfinder_set_rectangle(table, 100/GRID_SIZE + grid_half_width, 54/GRID_SIZE + (grid_half_width * 2), 3000/GRID_SIZE - 100/GRID_SIZE - grid_half_width , 867/GRID_SIZE - grid_half_width, FORBIDDEN);
    cocobot_pathfinder_set_rectangle(table, 54/GRID_SIZE + (grid_half_width * 2), 100/GRID_SIZE + grid_half_width, 583/GRID_SIZE - grid_half_width, 1900/GRID_SIZE - grid_half_width, FORBIDDEN);
    cocobot_pathfinder_set_rectangle(table, 54/GRID_SIZE + (grid_half_width * 2), 100/GRID_SIZE + grid_half_width, 2363/GRID_SIZE - grid_half_diag, 1900/GRID_SIZE - grid_half_diag, FORBIDDEN);

    //Epuration station = forbidden
    cocobot_pathfinder_set_rectangle(table, 1212/GRID_SIZE + (grid_half_width * 2), 250/GRID_SIZE + grid_half_width, 894/GRID_SIZE - grid_half_diag, 1750/GRID_SIZE - grid_half_width, FORBIDDEN);  

    //Expand walls half width = forbidden 
    cocobot_pathfinder_set_rectangle(table, TABLE_LENGTH / GRID_SIZE, grid_half_width, 0, 0, FORBIDDEN);  
    cocobot_pathfinder_set_rectangle(table, TABLE_LENGTH / GRID_SIZE, grid_half_width, 0, TABLE_WIDTH/GRID_SIZE - grid_half_width, FORBIDDEN);  
    cocobot_pathfinder_set_rectangle(table, grid_half_width, TABLE_WIDTH / GRID_SIZE, 0, 0, FORBIDDEN);
    cocobot_pathfinder_set_rectangle(table, grid_half_width, TABLE_WIDTH / GRID_SIZE, TABLE_LENGTH / GRID_SIZE - grid_half_width, 0, FORBIDDEN);  
}

