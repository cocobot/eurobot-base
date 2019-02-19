#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_PATHFINDER

#include <cocobot.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "cocobot/pathfinder_internal.h"
#include "cocobot/pathfinder_table_utils.h"
#include "cocobot/pathfinder_douglas_peucker.h"

static int g_table_updated;
static cocobot_node_s g_table[TABLE_LENGTH/GRID_SIZE][TABLE_WIDTH/GRID_SIZE];
static cocobot_list_s open_list;
static cocobot_trajectory_s final_traj;
static opponent_table_s g_opponent_robot;
static cocobot_trajectory_final_s g_resultTraj;

static void cocobot_pathfinder_set_other_robot()
{
    for(int i = 0; i < g_opponent_robot.nbr_slot_used; i++)
    {
        cocobot_pathfinder_set_robot_zone(g_table, (g_opponent_robot.other_robot[i].x + (TABLE_LENGTH / 2))/GRID_SIZE, ((TABLE_WIDTH / 2) - g_opponent_robot.other_robot[i].y)/GRID_SIZE);
    }
}

uint16_t cocobot_pathfinder_execute(int16_t starting_point_x, int16_t starting_point_y, int16_t target_point_x, int16_t target_point_y, cocobot_pathfinder_mode_e mode)
{
    uint16_t time = 0xffff;
    //cocobot_com_printf("PATHFINDER: Start point:(%d, %d) Target Point:(%d, %d)", starting_point_x, starting_point_y, target_point_x, target_point_y);

    //Reset table and init the open list
    cocobot_pathfinder_reset_table(g_table);
    cocobot_pathfinder_initialize_list(&open_list);

    //Reset the opponent table list
    memset(&g_opponent_robot, 0, sizeof(opponent_table_s));
    //Ask to the opponent_detection module if it sees something
    cocobot_opponent_detection_set_on_map();
    //Set on the map other robots
    cocobot_pathfinder_set_other_robot();

    //target_node
    cocobot_pathfinder_save_real_target_node(target_point_x, target_point_y);
    cocobot_node_s* target_node = &g_table[(target_point_x + (TABLE_LENGTH / 2))/GRID_SIZE][((TABLE_WIDTH / 2) - target_point_y)/GRID_SIZE];
    if((target_node->nodeType & OBSTACLE) || (target_node->nodeType & SOFT_OBSTACLE) || (target_node->nodeType & FORBIDDEN) || (target_node->nodeType & ROBOT) || (target_node->nodeType & GAME_ELEMENT))
    {
        //Set start and target point even if unreachable
        g_table[(starting_point_x + (TABLE_LENGTH / 2)) / GRID_SIZE][((TABLE_WIDTH / 2) - starting_point_y)/GRID_SIZE].nodeType |= START_POINT; 
        target_node->nodeType |= TARGET_POINT;
        //cocobot_com_printf("PATHFINDER: Target not reachable");
        g_table_updated = 1;
        return DESTINATION_NOT_AVAILABLE;
    }
    cocobot_pathfinder_set_target_node(target_node);

    //start_node
    cocobot_node_s* start_node = &g_table[(starting_point_x + (TABLE_LENGTH / 2)) / GRID_SIZE][((TABLE_WIDTH / 2) - starting_point_y)/GRID_SIZE];
    cocobot_pathfinder_set_real_start_node(start_node);

    //Check that start node is not a forbidden place
    if(!(start_node->nodeType == NEW_NODE) && !(start_node->nodeType & TEMPORARY_ALLOWED))
    {
        start_node = cocobot_pathfinder_find_closest_new_node(g_table, start_node);
    }

    cocobot_pathfinder_set_start_node(start_node);

    cocobot_node_s current_node = *start_node;
    current_node.cost = cocobot_pathfinder_get_distance(&current_node, target_node);

    cocobot_pathfinder_init_trajectory(&final_traj);

    while((current_node.x != target_node->x) || (current_node.y != target_node->y))
    {
        //Treat adjacent node
        for(int i=current_node.x-1; i<=current_node.x+1; i++)
        {
            for(int j=current_node.y-1; j<=current_node.y+1; j++)
            {
                if((i>=0) && (j>=0) && (i<(TABLE_LENGTH/GRID_SIZE)) && (j<(TABLE_WIDTH/GRID_SIZE)) && ((i != current_node.x) || (j!=current_node.y)))
                {
                    cocobot_pathfinder_compute_node(&open_list, &g_table[i][j], &current_node);
                }
            }
        }
        //open_list is not null
        if(open_list.nb_elements != 0)
        {
            //get first of the list
            open_list.table[0]->nodeType &= MASK_NEW_NODE;
            open_list.table[0]->nodeType |= CLOSED_LIST;
            current_node = *open_list.table[0];
            cocobot_pathfinder_remove_from_list(&open_list, open_list.table[0]);
        }
        else
        {
            //cocobot_com_printf("PATHFINDER: No solution");
            g_table_updated = 1;
            return NO_ROUTE_TO_TARGET;
        }
    }

    cocobot_pathfinder_get_path(&current_node, g_table, &final_traj);
    for(int i = 0; i < final_traj.nbr_points; i++)
    {
        //cocobot_com_printf("REAL_PATH x:%d, y:%d", final_traj.trajectory[i].x, final_traj.trajectory[i].y);
    }

    //Linearization of the trajectory
    cocobot_pathfinder_init_final_traj(&final_traj, &g_resultTraj);
    cocobot_pathfinder_douglas_peucker(&g_resultTraj, DP_MINIMUM_THRESHOLD);

    if(mode != COCOBOT_PATHFINDER_MODE_GET_DURATION)
    {
        cocobot_pathfinder_set_trajectory(&g_resultTraj);
    }

    time = cocobot_pathfinder_get_time(&g_resultTraj);
    //cocobot_com_printf("Trajectory duration: %dms", time);
    g_table_updated = 1;
    return time;
}

void cocobot_pathfinder_set_robot(int adv_x, int adv_y)
{
    if(g_opponent_robot.nbr_slot_used < NBR_OTHER_ROBOT_MAX)
    {
        g_opponent_robot.other_robot[g_opponent_robot.nbr_slot_used].x = adv_x;
        g_opponent_robot.other_robot[g_opponent_robot.nbr_slot_used].y = adv_y;
        g_opponent_robot.nbr_slot_used++;
    }
    else
    {
        memmove(&g_opponent_robot.other_robot[0], &g_opponent_robot.other_robot[1], (NBR_OTHER_ROBOT_MAX - 1) * sizeof(cocobot_point_s));
        g_opponent_robot.other_robot[NBR_OTHER_ROBOT_MAX - 1].x = adv_x;
        g_opponent_robot.other_robot[NBR_OTHER_ROBOT_MAX - 1].y = adv_y;
    }
}

void cocobot_pathfinder_remove_game_element(uint8_t x, uint8_t y, uint8_t radius)
{
    cocobot_pathfinder_set_circle_unmask(g_table, x, y, radius + ROBOT_HALF_WIDTH_GRID, GAME_ELEMENT);
}

void cocobot_pathfinder_allow_start_zone()
{
    cocobot_pathfinder_set_start_zone_allowed();
}

void cocobot_pathfinder_init(cocobot_pathfinder_table_init_s * initTable)
{
    memset(&g_opponent_robot, 0, sizeof(opponent_table_s));
    g_table_updated = 0;

    cocobot_pathfinder_initialize_table(g_table, initTable);
    g_table_updated = 1;
}

#if 0
void cocobot_pathfinder_handle_async_com(void)
{
    if(g_table_updated)
    {
        g_table_updated = 0;
        cocobot_com_send(COCOBOT_COM_PATHFINDER_DEBUG_PID,
                "HH[H]",
                TABLE_LENGTH / GRID_SIZE,
                TABLE_WIDTH / GRID_SIZE,
                (uint8_t *)g_table,                        //array ptr
                sizeof(g_table[0][0]),                        //array elm size 
                sizeof(g_table)/sizeof(g_table[0][0]),        //array size 
                0,                                         //array start
                sizeof(g_table)/sizeof(g_table[0][0]),        //array end 
                offsetof(cocobot_node_s, nodeType)
                );
    }
}
#endif
#endif
