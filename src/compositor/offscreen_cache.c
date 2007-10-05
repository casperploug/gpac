/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Yi-Zhen Zhang, Jean Le Feuvre
 *
 *			Copyright (c) ENST 2005-200X
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

#include "offscreen_cache.h"

#ifdef GPAC_USE_GROUP_CACHE

#include "visual_manager.h"
#include "mpeg4_grouping.h"
#include "texturing.h"


#define GPAC_CACHE_PF	GF_PIXEL_ARGB
#define GPAC_CACHE_BPP	4

void drawable_cache_del(DrawableCache *cache)
{
	if (cache->txh.data) free(cache->txh.data);
	if (cache->path) gf_path_del(cache->path);
	free(cache);
}

DrawableCache *drawable_cache_initialize(DrawableContext *ctx, GF_Compositor *compositor, GF_Rect *local_bounds)
{
	DrawableCache *cache;
	GF_IRect pix_bounds = gf_rect_pixelize(local_bounds);
	GF_SAFEALLOC(cache, DrawableCache);

	/*setup texture */
	cache->txh.compositor = compositor;
	cache->txh.height = pix_bounds.height;
	cache->txh.width = pix_bounds.width;
	cache->txh.stride = pix_bounds.width * GPAC_CACHE_BPP;
	cache->txh.pixelformat = GPAC_CACHE_PF;

	if (cache->txh.data) free(cache->txh.data);

	cache->txh.data = (u8 *) malloc (sizeof(u8) * cache->txh.stride * cache->txh.height);
	memset(cache->txh.data, 0, sizeof(char) * cache->txh.stride * cache->txh.height);
	/*the path of drawable_cache is a rectangle one that is the the bound of the object*/
	cache->path = gf_path_new();
	/*set a rectangle to the path
	  Attention, we want to center the cached bitmap at the center of the screen (main visual), so we use
	  the global coordinate to parameterize the path*/
	gf_path_add_rect_center(cache->path, 
		local_bounds->x + local_bounds->width/2,
		local_bounds->y - local_bounds->height/2,
		local_bounds->width, local_bounds->height);
 
	gf_sc_texture_push_image(&cache->txh, 0, 1);
	return cache;
}


void drawable_cache_draw(GF_TraverseState *tr_state, GF_Path *cache_path, GF_TextureHandler *cache_txh) 
{
	GF_TextureHandler *old_txh = tr_state->ctx->aspect.fill_texture;
	/*switch the texture to our offscreen cache*/
	tr_state->ctx->aspect.fill_texture = cache_txh;

#if 0
	visual_2d_texture_path(tr_state->visual, tr_state->ctx->drawable->cached->path, tr_state->ctx);
#else
	/*if skew/rotate, don't try the bitmap Blit (HW/SW)*/
	if (tr_state->ctx->transform.m[1] || tr_state->ctx->transform.m[3]) { 
		visual_2d_texture_path(tr_state->visual, cache_path, tr_state->ctx);
	} else {
		DrawableContext *ctx = tr_state->ctx;
		GF_Rect unclip;
		GF_IRect clip;
		u8 alpha = GF_COL_A(ctx->aspect.fill_color);
		/*THIS IS A HACK, will not work when setting filled=0, transparency and XLineProps*/
		if (!alpha) alpha = GF_COL_A(ctx->aspect.line_color);

		unclip = ctx->bi->unclip;
		clip = ctx->bi->clip;

		/*direct drawing, draw without clippers */
		if (tr_state->direct_draw) {
			tr_state->visual->DrawBitmap(tr_state->visual, ctx->aspect.fill_texture, &clip, &unclip, alpha, NULL, ctx->col_mat);
		}
		/*draw bitmap for all dirty rects*/
		else {
			u32 i;
			GF_IRect a_clip;
			for (i=0; i<tr_state->visual->to_redraw.count; i++) {
				/*there's an opaque region above, don't draw*/
#ifdef TRACK_OPAQUE_REGIONS
				if (tr_state->visual->draw_node_index < tr_state->visual->to_redraw.opaque_node_index[i]) continue;
#endif
				a_clip = clip;
				gf_irect_intersect(&a_clip, &tr_state->visual->to_redraw.list[i]);
				if (a_clip.width && a_clip.height) {
					tr_state->visual->DrawBitmap(tr_state->visual, ctx->aspect.fill_texture, &a_clip, &unclip, alpha, NULL, ctx->col_mat);
				}
			}
		}
	}
#endif
	tr_state->ctx->aspect.fill_texture = old_txh;
}

GroupCache *group_cache_new(GF_Node *node)
{
	GroupCache *cache;
	GF_SAFEALLOC(cache, GroupCache);
	cache->drawable = drawable_new();
	/*draw the cache through traverse callback*/
	cache->drawable->flags |= DRAWABLE_USE_TRAVERSE_DRAW;
	cache->drawable->node = node;
	cache->opacity = FIX_ONE;
	gf_sc_texture_allocate(&cache->txh);
	return cache;
}

void group_cache_del(GroupCache *cache)
{
	drawable_del_ex(cache->drawable, cache->txh.compositor);
	if (cache->txh.data) free(cache->txh.data);
	gf_sc_texture_release(&cache->txh);
	free(cache);
}

void group_cache_setup(GroupCache *cache, GF_Rect *local_bounds, GF_Compositor *compositor)
{
	/*pixelize bounds*/
	GF_IRect pix_bounds = gf_rect_pixelize(local_bounds);

	/*setup texture */
	cache->txh.compositor = compositor;
	cache->txh.height = pix_bounds.height;
	cache->txh.width = pix_bounds.width;
	cache->txh.stride = pix_bounds.width * GPAC_CACHE_BPP;
	cache->txh.pixelformat = GPAC_CACHE_PF;

	if (cache->txh.data) free(cache->txh.data);

	cache->txh.data = (u8 *) malloc (sizeof(char) * cache->txh.stride * cache->txh.height);
	memset(cache->txh.data, 0, sizeof(char) * cache->txh.stride * cache->txh.height);
	/*the path of drawable_cache is a rectangle one that is the the bound of the object*/
	gf_path_reset(cache->drawable->path);

	/*set a rectangle to the path
	  Attention, we want to center the cached bitmap at the center of the screen (main visual), so we use
	  the local coordinate to parameterize the path*/
	gf_path_add_rect_center(cache->drawable->path, 
		local_bounds->x + local_bounds->width/2,
		local_bounds->y - local_bounds->height/2,
		local_bounds->width, local_bounds->height);
 }

#define OFFSCREEN_SCALE_UP		FIX_ONE
#define OFFSCREEN_SCALE_DOWN	FIX_ONE

/**/
Bool group_cache_traverse(GF_Node *node, GroupingNode2D *group, GF_TraverseState *tr_state)
{
	Bool needs_recompute = 0;
	DrawableContext *group_ctx = NULL;	
	GF_ChildNodeItem *l;	
	//u32 time;

	/*this is not an offscreen group*/
	if (!(group->flags & GROUP_IS_CACHED) ) return 0;

	/*we are currently in a group cache, regular traversing*/
	if (tr_state->in_group_cache) return 0;

	/*draw mode*/
	if (tr_state->traversing_mode == TRAVERSE_DRAW_2D) {
		/*cache SHALL BE SETUP*/
		assert(group->cache);
		/*draw it*/
		drawable_cache_draw(tr_state, 
							group->cache->drawable->path, 
							&group->cache->txh); 
		return 1;
	}
	/*other modes than sorting, use regular traversing*/
	if (tr_state->traversing_mode != TRAVERSE_SORT) return 0;

	/*do we need to recompute the cache*/
	if (gf_node_dirty_get(node) & GF_SG_CHILD_DIRTY) {
		needs_recompute = 1;
	}

	if (!group->cache) {
		/*ALLOCATE THE CACHE*/
		group->cache = group_cache_new(node);
		needs_recompute = 1;
	}

	/*we need to redraw the group in an offscreen visual*/
	if (needs_recompute) {
		GF_Matrix2D backup;
		u32 prev_flags;
		GF_Rect cache_bounds;
		GF_SURFACE offscreen_surface, old_surf;
		GF_Raster2D *r2d = tr_state->visual->compositor->rasterizer;
		DrawableContext *child_ctx;
		Fixed temp_x, temp_y;


		/*step 1 : store current state and indicate children should not be cached*/
		tr_state->in_group_cache = 1;
		prev_flags = tr_state->direct_draw;
		/*store the current transform matrix, create a new one for group_cache*/
		gf_mx2d_copy(backup, tr_state->transform);
		gf_mx2d_init(tr_state->transform);
		/*disable direct drawing to indicate that children in the group should not be drawn*/
		tr_state->direct_draw = 0;

		/*step 2: insert a DrawableContext for this group in the display list*/
		group_ctx = drawable_init_context_mpeg4(group->cache->drawable, tr_state);

		/*step 3: traverse the group to collect all children in the display list, but using the local coordinate system*/		
		l = ((GF_ParentNode*)node)->children;
		gf_mx2d_init(tr_state->transform);		
		gf_mx2d_add_scale(&tr_state->transform, OFFSCREEN_SCALE_UP, OFFSCREEN_SCALE_UP);		
		while (l) {				
			gf_node_traverse(l->node, tr_state);
			l = l->next;
		}

		/*step 4: now we have all DrawableContexts of the children of this group, get their bounds*/		
		cache_bounds.width = cache_bounds.height = 0;
		child_ctx = group_ctx->next;
		while (child_ctx && child_ctx->drawable) {
			gf_rect_union(&cache_bounds, &child_ctx->bi->unclip);

			/*TODO for dynamic group caching: add some tests*/
			/*1. do we have a texture on one drawable - if so maybe no cache*/
			/*2- how many drawable*/
			/*3- what is the complexity of each drawable 
				a. nb points on path
				b. nb points in outlines
				c. alpha used
				...
			*/
			child_ctx = child_ctx->next;
		}

		/*step 5: now we have the bounds:
			allocate the offscreen memory
			create temp raster visual & attach to buffer
			override the tr_state->visual->the_surface with the temp raster
			!! SAME AS DRAWABLE CACHE: WATCHOUT FOR NON-CENTERED RECT (add translation to visual and 
			build proper path)
			move to DRAW mode
		*/
		old_surf = tr_state->visual->raster_surface;
		offscreen_surface = r2d->surface_new(r2d, 1);	/*a new temp raster visual*/
		tr_state->visual->raster_surface = offscreen_surface;
		tr_state->traversing_mode = TRAVERSE_DRAW_2D;
		/* Initialize the group cache, and we build the path for the group */
		group_cache_setup(group->cache, &cache_bounds, tr_state->visual->compositor);
			
		/*attach the buffer to visual*/
		r2d->surface_attach_to_buffer(offscreen_surface, group->cache->txh.data,
										group->cache->txh.width, 
										group->cache->txh.height,
										group->cache->txh.stride, 
										group->cache->txh.pixelformat);
		
		/*centered the bitmap on the visual*/
		gf_mx2d_init(tr_state->transform);
		gf_mx2d_add_scale(&tr_state->transform, OFFSCREEN_SCALE_DOWN, OFFSCREEN_SCALE_DOWN); 
		temp_x = -cache_bounds.x-cache_bounds.width/2;
		temp_y = -cache_bounds.y +cache_bounds.height/2;
		gf_mx2d_add_translation(&tr_state->transform, temp_x, temp_y);		
		
		/*step 6: draw all child contexts - use direct drawing in order to bypass dirty rect*/
		tr_state->direct_draw = 1;
		group_ctx->flags &= ~CTX_NO_ANTIALIAS;
		child_ctx = group_ctx->next;		
		while (child_ctx && child_ctx->drawable) {
			gf_mx2d_pre_multiply(&child_ctx->transform, &tr_state->transform);
			child_ctx->bi->clip = tr_state->visual->top_clipper;
			tr_state->ctx = child_ctx;			
			if (child_ctx->drawable->flags & DRAWABLE_USE_TRAVERSE_DRAW) {
				gf_node_traverse(child_ctx->drawable->node, tr_state);
			} else {
				drawable_draw(child_ctx->drawable, tr_state);
			}
			/*and discard this context from the main visual display list*/
			child_ctx->drawable = NULL;	
			child_ctx = child_ctx->next;
		}	
		tr_state->ctx = NULL;
		/*and set ourselves as the last context on the main visual*/
		tr_state->visual->cur_context = group_ctx;

		/*restore state and destroy whatever needs to be cleaned*/
		gf_mx2d_copy(tr_state->transform, backup);
		tr_state->in_group_cache = 0;
		tr_state->direct_draw = prev_flags;
		r2d->surface_delete(offscreen_surface);
		tr_state->visual->raster_surface = old_surf;
		tr_state->traversing_mode = TRAVERSE_SORT;
		
		/*update texture*/
		group->cache->txh.transparent = 1;
		gf_sc_texture_push_image(&group->cache->txh, 0, 1);
	}
	/*just setup the context*/
	else if(group->cache) {
		group_ctx = drawable_init_context_mpeg4(group->cache->drawable, tr_state);
	}
	assert(group_ctx);
	group_ctx->flags |= CTX_NO_ANTIALIAS;
	group_ctx->aspect.fill_color = GF_COL_ARGB_FIXED(group->cache->opacity, 0, 0, 0);
	drawable_finalize_sort(group_ctx, tr_state, NULL);

	return 1;
}

#endif /*GPAC_USE_GROUP_CACHE*/