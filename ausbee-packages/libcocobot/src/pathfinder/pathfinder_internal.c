#include <include/generated/autoconf.h>
#ifdef CONFIG_LIBCOCOBOT_PATHFINDER

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cocobot.h>
#include <float.h>
#include "cocobot/pathfinder_internal.h"
#include "cocobot/pathfinder_douglas_peucker.h"
#include "cocobot_pathfinder_config.h"

static cocobot_node_s g_target_node;
static cocobot_node_s g_start_node;
static cocobot_node_s g_real_start_node;
static cocobot_point_s g_real_target_point;

void cocobot_pathfinder_compute_node(cocobot_list_s *open_list, cocobot_node_s* node, cocobot_node_s* parent_node)
{
    if((node->nodeType & MASK_TEMPORARY_OBSTACLE) != 0)
    {
        //Do nothing, a robot is an obstacle
    }
    else if((node->nodeType & OPEN_LIST) == OPEN_LIST)
    {
        if((parent_node->cost - cocobot_pathfinder_get_distance(parent_node, &g_target_node)) + cocobot_pathfinder_get_distance(parent_node, node) + cocobot_pathfinder_get_distance(node, &g_target_node) < node->cost)
        {
            cocobot_pathfinder_remove_from_list(open_list, node);
            node->cost = (parent_node->cost - cocobot_pathfinder_get_distance(parent_node, &g_target_node)) + cocobot_pathfinder_get_distance(parent_node, node) + cocobot_pathfinder_get_distance(node, &g_target_node);
            node->pX = parent_node->x;
            node->pY = parent_node->y;
            cocobot_pathfinder_add_to_list(open_list, node);
        }
        else
        {
            ;
        }
    }
    else if((node->nodeType & CLOSED_LIST) == CLOSED_LIST)
    {
        ;//Do nothing
    }
    else if((node->nodeType & NEW_NODE) == NEW_NODE)
    {
        node->pX = parent_node->x;
        node->pY = parent_node->y;
        node->nodeType |= OPEN_LIST;
        node->cost = (parent_node->cost - cocobot_pathfinder_get_distance(parent_node, &g_target_node)) + cocobot_pathfinder_get_distance(parent_node, node) + cocobot_pathfinder_get_distance(node, &g_target_node);
        cocobot_pathfinder_add_to_list(open_list, node);
    }
    else if((node->nodeType & TEMPORARY_ALLOWED) == TEMPORARY_ALLOWED)
    {
        node->pX = parent_node->x;
        node->pY = parent_node->y;
        node->nodeType |= OPEN_LIST;
        node->cost = (parent_node->cost - cocobot_pathfinder_get_distance(parent_node, &g_target_node)) + cocobot_pathfinder_get_distance(parent_node, node) + cocobot_pathfinder_get_distance(node, &g_target_node);
        cocobot_pathfinder_add_to_list(open_list, node);
    }
    else
    {
        ;//Do nothing for all other cases
    }
}

float cocobot_pathfinder_get_distance(cocobot_node_s *source, cocobot_node_s *dest)
{
    // Distance between a node and an other one located just next to it in the diagonal
    static float distance_diag = M_SQRT2;

    float _return_value = 0.0;

    //Same node, in case of a dummy wants a distance between the same node
    if((source->x == dest->x) && (source->y == dest->y))
        _return_value = 0.0;
    else if(source->x == dest->x)
        _return_value = (float)abs(source->y - dest->y);
    else if(source->y == dest->y)
        _return_value = (float)abs(source->x - dest->x);
    else if(abs(source->y - dest->y) == abs(source->x - dest->x))
        _return_value = (float)abs(source->x - dest->x) * distance_diag;
    else
        _return_value = sqrtf((dest->y - source->y)*(dest->y - source->y) + (dest->x - source->x)*(dest->x - source->x));

    return _return_value;
}

void cocobot_pathfinder_initialize_list(cocobot_list_s *list)
{
    list->nb_elements = 0;
    //in theory, not usefull 
    memset(list, 0, sizeof(cocobot_list_s));
}

void cocobot_pathfinder_add_to_list(cocobot_list_s *list, cocobot_node_s *node)
{
    //If the list is empty
    if(list->nb_elements == 0)
    {
        list->table[0] = node;
    }
    else
    {
        int index = 0;
        //find its position in the list
        while(node->cost > list->table[index]->cost)
        {
            index++;
            if(index == list->nb_elements)
                break;
        }
        //if the list is not full
        if(index < MAXIMUM_NODE_IN_LIST)
        {
            memmove(&list->table[index+1], &list->table[index], (list->nb_elements - index + 1) * sizeof(cocobot_node_s*));
            list->table[index] = node;
        }
        else
        {
            //cocobot_com_printf("ADD: Cost is too big : cost :%f", (double)node->cost);
            //TBD
            //Cost is too big, not added in list --> not supposed to happen, take a list size big enough
        }
    }
    list->nb_elements++;
}

int cocobot_pathfinder_remove_from_list(cocobot_list_s *list, cocobot_node_s *node)
{
    if(list->nb_elements == 0)
    {
        //printf("List is empty");
        return -1;
    }
    else
    {
        int index = 0;
        while(list->table[index] != node)
        {
            index++;
            if(index == list->nb_elements)
                return -2;
        }
        if(index != (MAXIMUM_NODE_IN_LIST - 1))
        {
            memmove(&list->table[index], &list->table[index+1], (list->nb_elements - index - 1) * sizeof(cocobot_node_s*));
            list->nb_elements--;
        }
        else
        {
            ; //Nothing to do
        }
    }
    return 0;
}

void cocobot_pathfinder_set_target_node(cocobot_node_s *target_node)
{
    memcpy(&g_target_node, target_node, sizeof(cocobot_node_s));
    //cocobot_com_printf("TARGET_NODE: x=%d, y=%d, nodeType = %x", g_target_node.x, g_target_node.y, g_target_node.nodeType);
}

void cocobot_pathfinder_save_real_target_node(int16_t x, int16_t y)
{
    g_real_target_point.x = x;
    g_real_target_point.y = y;
}

void cocobot_pathfinder_set_start_node(cocobot_node_s *start_node)
{
    memcpy(&g_start_node, start_node, sizeof(cocobot_node_s));
    //cocobot_com_printf("PATHFINDER_STARTING_NODE x=%d, y=%d", g_start_node.x, g_start_node.y);
}

void cocobot_pathfinder_set_real_start_node(cocobot_node_s *start_node)
{
    memcpy(&g_real_start_node, start_node, sizeof(cocobot_node_s));
    //cocobot_com_printf("REAL_STARTING_NODE x=%d, y=%d", g_real_start_node.x, g_real_start_node.y);
}

void cocobot_pathfinder_get_path(cocobot_node_s *final_node, cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], cocobot_trajectory_s* trajectory)
{
    //Set target point
    trajectory->trajectory[TRAJECTORY_NBR_POINTS_MAX - 1 - trajectory->nbr_points] = cocobot_pathfinder_get_point_from_node(final_node);
    trajectory->nbr_points++;
    table[(int)final_node->x][(int)final_node->y].nodeType &= MASK_NEW_NODE;
    table[(int)final_node->x][(int)final_node->y].nodeType |= TARGET_POINT;
    //cocobot_com_printf("PATH x=%d, y=%d px=%d, py=%d NodeType=%X", final_node->x, final_node->y, final_node->pX, final_node->pY, final_node->nodeType);

    ////Next points
    final_node = &table[(int)final_node->pX][(int)final_node->pY];
    while((final_node->x != g_start_node.x) || (final_node->y != g_start_node.y))
    {
        //cocobot_com_printf("PATH x=%d, y=%d px=%d, py=%d NodeType=%X", final_node->x, final_node->y, final_node->pX, final_node->pY, final_node->nodeType);
        //fill trajectory beginning by the end
        trajectory->trajectory[TRAJECTORY_NBR_POINTS_MAX - 1 - trajectory->nbr_points] = cocobot_pathfinder_get_point_from_node(final_node);
        trajectory->nbr_points++;
        final_node->nodeType &= MASK_NEW_NODE;
        final_node->nodeType |= FINAL_TRAJ;
        final_node = &table[(int)final_node->pX][(int)final_node->pY];
    }
    //start point
    trajectory->trajectory[TRAJECTORY_NBR_POINTS_MAX - 1 - trajectory->nbr_points] = cocobot_pathfinder_get_point_from_node(final_node);
    trajectory->nbr_points++;
    final_node->nodeType &= MASK_NEW_NODE;
    final_node->nodeType |= START_POINT;
    //cocobot_com_printf("PATH x=%d, y=%d NodeType=%X", final_node->x, final_node->y, final_node->nodeType);
    
    //put the trajectory at the begining of the array using memcpy if possible, memmove otherwise
    if(trajectory->nbr_points <= (TRAJECTORY_NBR_POINTS_MAX / 2))
        memcpy(trajectory->trajectory, &trajectory->trajectory[TRAJECTORY_NBR_POINTS_MAX - trajectory->nbr_points], trajectory->nbr_points * sizeof(cocobot_point_s));
    else
        memmove(trajectory->trajectory, &trajectory->trajectory[TRAJECTORY_NBR_POINTS_MAX - trajectory->nbr_points], trajectory->nbr_points * sizeof(cocobot_point_s));
}

void cocobot_pathfinder_set_trajectory(cocobot_trajectory_final_s * resultTraj)
{
    cocobot_point_s point;
    cocobot_point_final_s final_point;

    //First point
    if((g_real_start_node.x != g_start_node.x) || (g_real_start_node.y != g_start_node.y))
    {
        //If the start point of the pathfinder traj is different from the real start point, it can be added here.
        final_point.x = g_start_node.x;
        final_point.y = g_start_node.y;
        point = cocobot_pathfinder_get_real_coordinate(final_point);
        cocobot_trajectory_goto_xy(point.x, point.y, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
        //cocobot_com_printf("LINEAR_PATH first point: x=%d (x=%d), y=%d (y=%d)", point.x, final_point.x, point.y, final_point.y);
    }
            

    //Next points
    for(int i = 1; i < (resultTraj->nbr_points - 1); i++)
    {
        if(resultTraj->trajectory[i].status == POINT_TO_KEEP)
        {
            point = cocobot_pathfinder_get_real_coordinate(resultTraj->trajectory[i]);
            cocobot_trajectory_goto_xy(point.x, point.y, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
            //cocobot_com_printf("LINEAR_PATH: x=%d (x=%d), y=%d (y=%d)", point.x, resultTraj->trajectory[i].x, point.y, resultTraj->trajectory[i].y);
        }
    }
    //Last point
    cocobot_trajectory_goto_xy(g_real_target_point.x, g_real_target_point.y, COCOBOT_TRAJECTORY_UNLIMITED_TIME);
    //cocobot_com_printf("LINEAR_PATH: x=%d, y=%d", g_real_target_point.x, g_real_target_point.y);

}

uint16_t cocobot_pathfinder_get_time(cocobot_trajectory_final_s * resultTraj)
{
    uint16_t time = 0;
    cocobot_node_s node;
    cocobot_node_s lastNode;

    //First point
    if((g_real_start_node.x != g_start_node.x) || (g_real_start_node.y != g_start_node.y))
    {
        time += (uint16_t)(cocobot_pathfinder_get_distance(&g_real_start_node, &g_start_node) * (float)GRID_SIZE) / cocobot_asserv_get_linear_speed();
    }
    
    lastNode = g_start_node;

    //Next points
    for(int i = 1; i < (resultTraj->nbr_points - 1); i++)
    {
        if(resultTraj->trajectory[i].status == POINT_TO_KEEP)
        {
            node.x = resultTraj->trajectory[i].x;
            node.y = resultTraj->trajectory[i].y;
            time += (uint16_t)(cocobot_pathfinder_get_distance(&node, &lastNode) * (float)GRID_SIZE) / cocobot_asserv_get_linear_speed();
            lastNode.x = node.x;
            lastNode.y = node.y;
        }
    }
    //Last point
    time += (uint16_t)(cocobot_pathfinder_get_distance(&lastNode, &g_target_node) * (float)GRID_SIZE) / cocobot_asserv_get_linear_speed();
    return time;
}

cocobot_point_s cocobot_pathfinder_get_point_from_node(cocobot_node_s *node)
{
    cocobot_point_s _point;
    _point.x = node->x;
    _point.y = node->y;
    return _point;
}

cocobot_point_s cocobot_pathfinder_get_real_coordinate(cocobot_point_final_s point)
{
    cocobot_point_s _point;
    _point.x = point.x * GRID_SIZE + GRID_SIZE/2 - TABLE_LENGTH/2;
    _point.y = TABLE_WIDTH/2 - (point.y * GRID_SIZE + GRID_SIZE/2);
    return _point;
}

void cocobot_pathfinder_init_trajectory(cocobot_trajectory_s *trajectory)
{
    trajectory->nbr_points = 0;
    memset(trajectory->trajectory, 0, TRAJECTORY_NBR_POINTS_MAX*sizeof(cocobot_point_s));
}

cocobot_node_s * cocobot_pathfinder_find_closest_new_node(cocobot_node_s table[][TABLE_WIDTH/GRID_SIZE], cocobot_node_s *startPoint)
{
    int16_t i = 1;
    float tmpDistance = 0;
    uint8_t pointFound = 0;
    float finalDistance = FLT_MAX;
    cocobot_node_s * realStart = startPoint;

    while((!pointFound) && (i <= 10))
    {
        for(int j = (int16_t)startPoint->y - i; j <= (int16_t)startPoint->y + i; j++)
        {
            if((j >= 0) || (j < TABLE_WIDTH))
            {
                if(((int16_t)startPoint->x - i) >= 0)
                {
                    if(table[startPoint->x - i][j].nodeType == NEW_NODE) 
                    {
                        tmpDistance = cocobot_pathfinder_get_distance(&table[startPoint->x - i][j], &g_target_node);
                        if(tmpDistance < finalDistance)
                        {
                            realStart = &table[startPoint->x - i][j];
                            finalDistance = tmpDistance;
                            pointFound = 1;
                        }
                    }
                }
                if(((int16_t)startPoint->x + i) < TABLE_LENGTH)
                {
                    if(table[startPoint->x + i][j].nodeType == NEW_NODE) 
                    {
                        tmpDistance = cocobot_pathfinder_get_distance(&table[startPoint->x + i][j], &g_target_node);
                        if(tmpDistance < finalDistance)
                        {
                            realStart = &table[startPoint->x + i][j];
                            finalDistance = tmpDistance;
                            pointFound = 1;
                        }
                    }

                }
            }
        }
        for(int j = (int16_t)startPoint->x - i - 1; j < (int16_t)startPoint->x + i - 1; j++)
        {
            if((j >= 0) || (j < TABLE_LENGTH))
            {
                if(((int16_t)startPoint->y - i) >= 0)
                {
                    if(table[j][startPoint->y - i].nodeType == NEW_NODE) 
                    {
                        tmpDistance = cocobot_pathfinder_get_distance(&table[j][startPoint->y - i], &g_target_node);
                        if(tmpDistance < finalDistance)
                        {
                            realStart = &table[j][startPoint->y - i];
                            finalDistance = tmpDistance;
                            pointFound = 1;
                        }
                    }

                }
                if(((int16_t)startPoint->y + i) < TABLE_WIDTH)
                {
                    if(table[j][startPoint->y + i].nodeType == NEW_NODE) 
                    {
                        tmpDistance = cocobot_pathfinder_get_distance(&table[j][startPoint->y + i], &g_target_node);
                        if(tmpDistance < finalDistance)
                        {
                            realStart = &table[j][startPoint->y + i];
                            finalDistance = tmpDistance;
                            pointFound = 1;
                        }
                    }
                }
            }
        }
        i++;
    }
    return realStart;
}
#endif
