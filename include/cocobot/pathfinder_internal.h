#ifndef COCOBOT_PATHFINDER_INTERNAL_H
#define COCOBOT_PATHFINDER_INTERNAL_H

#include "cocobot/pathfinder_table.h"
/**
 * Compute a node of the table
 * Arguments:
 *  - open_list : pointer on a* openlist
 *  - node : pointer on the node to be computed
 *  - parent_node : pointer on the parent of node. 
 *  
 * Return Value: void
 */
void cocobot_pathfinder_compute_node(list_s** open_list, cocobot_node_s* node, cocobot_node_s* parent_node);

float cocobot_get_distance(cocobot_node_s *_p_source, cocobot_node_s *_p_dest);
int removeFromList(list_s* _p_list, cocobot_node_s* _p_node);
void addToList(list_s** _p_list, cocobot_node_s* _p_node);
void cocobot_pathfinder_set_target_node(cocobot_node_s *target_node);
void cocobot_pathfinder_set_start_node(cocobot_node_s *start_node);
list_s* getPath(cocobot_node_s*_p_finalNode, cocobot_node_s p_table[][TABLE_WIDTH/GRID_SIZE]);

#endif  //COCOBOT_PATHFINDER_INTERNAL_H
