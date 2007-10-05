/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Copyright (c) Jean Le Feuvre 2000-2005
 *					All rights reserved
 *
 *  This file is part of GPAC / Scene Compositor sub-project
 *
 *  GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *   
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#ifndef GROUPING_H
#define GROUPING_H

#include "drawable.h"

enum
{
	GROUP_HAS_SENSORS	=	1,
	GROUP_SKIP_CULLING	=	1<<1,
	GROUP_HAS_LIGHTS	=	1<<2,
	GROUP_IS_ANCHOR		=	1<<3,
	GROUP_IS_CACHED		=	1<<4,
};

#ifdef GPAC_USE_GROUP_CACHE

#define GROUPING_NODE_STACK_2D		\
	u32 flags;						\
	GF_Rect bounds;					\
	struct _group_cache *cache;	\

#else

#define GROUPING_NODE_STACK_2D		\
	u32 flags;						\
	GF_Rect bounds;					\

#endif

typedef struct
{
	GROUPING_NODE_STACK_2D
} GroupingNode2D;

/*traverse all children of the node */
void group_2d_traverse(GF_Node *node, GroupingNode2D *group, GF_TraverseState *tr_state);

/*traverse all children of the node with the given traversing order*/
void group_2d_traverse_with_order(GF_Node *node, GroupingNode2D *group, GF_TraverseState *tr_state, u32 *positions);


#ifndef GPAC_DISABLE_3D

/*
	@children: pointer to children field
	@sensors: cached list of sensors at this level, NULL if none
	@lights: cached list of lights (local and global) at this level, NULL if none
	@bbox: bounding box of sub-tree
	@dont_cull: if set, traversing always occur (needed for audio subgraph)
*/

#define GROUPING_NODE_STACK_3D	\
	u32 flags;						\
	GF_BBox bbox;				\

typedef struct _parent_node_3d
{
	GROUPING_NODE_STACK_3D
} GroupingNode;

GroupingNode *group_3d_new(GF_Node *node);
void group_3d_delete(GF_Node *node);


/*traverse all children of the node */
void group_3d_traverse(GF_Node *n, GroupingNode *group, GF_TraverseState *tr_state);

#endif



/*
 * ParentNode (Form, Layout, PathLayout, ...) tools
*/


typedef struct 
{
	/*the associated child (can be a group or a shape)*/
	GF_Node *child;

	/*child bounds before and after placement*/
	GF_Rect original, final;
	
	/*layout run-time scroll*/
	Fixed scroll_x, scroll_y;

	/*if text node, ascent and descent of the text is stored for baseline alignment*/
	Fixed ascent, descent;
	u32 text_split_idx;
} ChildGroup;



#define PARENT_NODE_STACK_2D	\
			GROUPING_NODE_STACK_2D	\
			/*list of ChildGroup drawn (can be fully transparents) - used for post placement*/	\
			GF_List *groups;	


typedef struct _parent_node_2d
{
	PARENT_NODE_STACK_2D
} ParentNode2D;

/*performs stack init (allocate all base stuff of stack)*/
void parent_node_setup(ParentNode2D *ptr);
/*performs stack init (frees all base stuff of stack, but not the stack)*/
void parent_node_predestroy(ParentNode2D *gr);
/*reset children list*/
void parent_node_reset(ParentNode2D *gr);
/*creates a new group for the given node. If the node is NULL, the previous group's node is used*/
void parent_node_start_group(ParentNode2D *group, GF_Node *n);
/*sets the current group bounds*/
void parent_node_end_group(ParentNode2D *group, GF_Rect *bounds);
/*sets the current group bounds + font properties*/
void parent_node_end_text_group(ParentNode2D *group, GF_Rect *bounds, Fixed ascent, Fixed descent, u32 split_text_idx);

/*traverse the parent group's children for bounds compute. Starts a new ChildGroup for each child*/
void parent_node_traverse(GF_Node *node, ParentNode2D *group, GF_TraverseState *tr_state);

/*updates the matrix (translations only, computed between final and original bounds) and traverse the given child*/
void parent_node_child_traverse(ChildGroup *cg, GF_TraverseState *tr_state);

/*updates the matrix (position assigned by caller) and traverse the given child. Do not traverse if the matrix is NULL*/
void parent_node_child_traverse_matrix(ChildGroup *cg, GF_TraverseState *tr_state, GF_Matrix2D *mat2D);


#endif
