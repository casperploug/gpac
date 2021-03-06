/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Copyright (c) Jean Le Feuvre 2000-2005
 *					All rights reserved
 *
 *  This file is part of GPAC / SDL audio and video module
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

#ifdef GPAC_IPHONE
#include "libgpac_symbols.h"
#else
#include "sdl_out.h"
#include <gpac/user.h>
#endif


#ifdef WIN32
#include <windows.h>
#endif


/*cursors data*/
static char hand_data[] = 
{
	0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,2,2,1,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,1,2,2,1,2,2,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,0,1,2,2,1,2,2,1,2,2,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,2,2,1,1,2,2,2,2,2,2,2,2,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,2,2,2,1,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,2,1,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


static char collide_data[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,0,1,1,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,0,1,1,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};




#define SDLVID()	SDLVidCtx *ctx = (SDLVidCtx *)dr->opaque

static u32 video_modes[] = 
{
	320, 200,
	320, 240,
	400, 300,
	600, 400,
	800, 600,
	1024, 768,
	1152, 864,
	1280, 1024
};
static u32 nb_video_modes = 8;

void SDLVid_SetCaption()
{
	char szName[100];
	if (SDL_VideoDriverName(szName, 100)) {
		char szCap[1024];
		sprintf(szCap, "SDL Video Output (%s)", szName);
		SDL_WM_SetCaption(szCap, NULL);
	} else {
		SDL_WM_SetCaption("SDL Video Output", NULL);
	}
}

SDL_Cursor *SDLVid_LoadCursor(char *maskdata)
{
	s32 ind, i, j;
	u8 data[4*32];
	u8 mask[4*32];

	ind = -1;
	for (i=0; i<32; i++) {
		for (j=0; j<32; j++) {
			if (j%8) {
				data[ind] <<= 1;
				mask[ind] <<= 1;
			} else {
				ind++;
				data[ind] = mask[ind] = 0;
			}
			switch (maskdata[j+32*i]) {
			/*black*/
			case 1:
				data[ind] |= 0x01;
			/*white*/
			case 2:
				mask[ind] |= 0x01;
				break;
			}
		}
	}
	return SDL_CreateCursor(data, mask, 32, 32, 0, 0);
}


static void sdl_translate_key(u32 SDLkey, GF_EventKey *evt) 
{
	evt->flags = 0;
	evt->hw_code = SDLkey;
	switch (SDLkey) {
	case SDLK_BACKSPACE: evt->key_code = GF_KEY_BACKSPACE; break;
	case SDLK_TAB: evt->key_code = GF_KEY_TAB; break;
	case SDLK_CLEAR: evt->key_code = GF_KEY_CLEAR; break;
	case SDLK_PAUSE: evt->key_code = GF_KEY_PAUSE; break;
	case SDLK_ESCAPE: evt->key_code = GF_KEY_ESCAPE; break;
	case SDLK_SPACE: evt->key_code = GF_KEY_SPACE; break;

	case SDLK_KP_ENTER: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_RETURN: 
		evt->key_code = GF_KEY_ENTER; 
		break;

	case SDLK_KP_MULTIPLY: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_ASTERISK: 
		evt->key_code = GF_KEY_STAR; 
		break;
	case SDLK_KP_PLUS: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_PLUS: 
		evt->key_code = GF_KEY_PLUS; 
		break;
	case SDLK_KP_MINUS: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_MINUS: 
		evt->key_code = GF_KEY_HYPHEN; 
		break;
	case SDLK_KP_DIVIDE: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_SLASH: 
		evt->key_code = GF_KEY_SLASH; 
		break;

	case SDLK_KP0: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_0: 
		evt->key_code = GF_KEY_0; 
		break;
	case SDLK_KP1: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_1: 
		evt->key_code = GF_KEY_1; 
		break;
	case SDLK_KP2: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_2: 
		evt->key_code = GF_KEY_2; 
		break;
	case SDLK_KP3: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_3: 
		evt->key_code = GF_KEY_3; 
		break;
	case SDLK_KP4: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_4: 
		evt->key_code = GF_KEY_4; 
		break;
	case SDLK_KP5: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_5: 
		evt->key_code = GF_KEY_5; 
		break;
	case SDLK_KP6: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_6: 
		evt->key_code = GF_KEY_6; 
		break;
	case SDLK_KP7: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_7: 
		evt->key_code = GF_KEY_7; 
		break;
	case SDLK_KP8: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_8: 
		evt->key_code = GF_KEY_8; 
		break;
	case SDLK_KP9: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_9: 
		evt->key_code = GF_KEY_9; 
		break;
	case SDLK_KP_PERIOD: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_PERIOD:
		evt->key_code = GF_KEY_FULLSTOP; 
		break;
	case SDLK_KP_EQUALS: evt->flags = GF_KEY_EXT_NUMPAD;
	case SDLK_EQUALS:
		evt->key_code = GF_KEY_EQUALS; 
		break;
	
	case SDLK_EXCLAIM: evt->key_code = GF_KEY_EXCLAMATION; break;
	case SDLK_QUOTEDBL: evt->key_code = GF_KEY_QUOTATION; break;
	case SDLK_HASH: evt->key_code = GF_KEY_NUMBER; break;
	case SDLK_DOLLAR: evt->key_code = GF_KEY_DOLLAR; break;
	case SDLK_AMPERSAND: evt->key_code = GF_KEY_AMPERSAND; break;
	case SDLK_QUOTE: evt->key_code = GF_KEY_APOSTROPHE; break;
	case SDLK_LEFTPAREN: evt->key_code = GF_KEY_LEFTPARENTHESIS; break;
	case SDLK_RIGHTPAREN: evt->key_code = GF_KEY_RIGHTPARENTHESIS; break;
	case SDLK_COMMA: evt->key_code = GF_KEY_COMMA; break;
	case SDLK_COLON: evt->key_code = GF_KEY_COLON; break;
	case SDLK_SEMICOLON: evt->key_code = GF_KEY_SEMICOLON; break;
	case SDLK_LESS: evt->key_code = GF_KEY_LESSTHAN; break;
	case SDLK_GREATER: evt->key_code = GF_KEY_GREATERTHAN; break;
	case SDLK_QUESTION: evt->key_code = GF_KEY_QUESTION; break;
	case SDLK_AT: evt->key_code = GF_KEY_AT; break;
	case SDLK_LEFTBRACKET: evt->key_code = GF_KEY_LEFTSQUAREBRACKET; break;
	case SDLK_RIGHTBRACKET: evt->key_code = GF_KEY_RIGHTSQUAREBRACKET; break;
	case SDLK_BACKSLASH: evt->key_code = GF_KEY_BACKSLASH; break;
	case SDLK_UNDERSCORE: evt->key_code = GF_KEY_UNDERSCORE; break;
	case SDLK_BACKQUOTE: evt->key_code = GF_KEY_GRAVEACCENT; break;
	case SDLK_DELETE: evt->key_code = GF_KEY_DEL; break;
	case SDLK_UNDO: evt->key_code = GF_KEY_UNDO; break;

	case SDLK_UP: evt->key_code = GF_KEY_UP; break;
	case SDLK_DOWN: evt->key_code = GF_KEY_DOWN; break;
	case SDLK_RIGHT: evt->key_code = GF_KEY_RIGHT; break;
	case SDLK_LEFT: evt->key_code = GF_KEY_LEFT; break;
	case SDLK_INSERT: evt->key_code = GF_KEY_INSERT; break;
	case SDLK_HOME: evt->key_code = GF_KEY_HOME; break;
	case SDLK_END: evt->key_code = GF_KEY_END; break;
	case SDLK_PAGEUP: evt->key_code = GF_KEY_PAGEUP; break;
	case SDLK_PAGEDOWN: evt->key_code = GF_KEY_PAGEDOWN; break;
	case SDLK_F1: evt->key_code = GF_KEY_F1; break;
	case SDLK_F2: evt->key_code = GF_KEY_F2; break;
	case SDLK_F3: evt->key_code = GF_KEY_F3; break;
	case SDLK_F4: evt->key_code = GF_KEY_F4; break;
	case SDLK_F5: evt->key_code = GF_KEY_F5; break;
	case SDLK_F6: evt->key_code = GF_KEY_F6; break;
	case SDLK_F7: evt->key_code = GF_KEY_F7; break;
	case SDLK_F8: evt->key_code = GF_KEY_F8; break;
	case SDLK_F9: evt->key_code = GF_KEY_F9; break;
	case SDLK_F10: evt->key_code = GF_KEY_F10; break;
	case SDLK_F11: evt->key_code = GF_KEY_F11; break;
	case SDLK_F12: evt->key_code = GF_KEY_F12; break;
	case SDLK_F13: evt->key_code = GF_KEY_F13; break;
	case SDLK_F14: evt->key_code = GF_KEY_F14; break;
	case SDLK_F15: evt->key_code = GF_KEY_F15; break;
	case SDLK_NUMLOCK: evt->key_code = GF_KEY_NUMLOCK; break;
	case SDLK_CAPSLOCK: evt->key_code = GF_KEY_CAPSLOCK; break;
	case SDLK_SCROLLOCK: evt->key_code = GF_KEY_SCROLL; break;

	case SDLK_RSHIFT: 
		evt->key_code = GF_KEY_SHIFT;
		evt->flags = GF_KEY_EXT_RIGHT;
		break;
	case SDLK_LSHIFT: 
		evt->key_code = GF_KEY_SHIFT;
		evt->flags = GF_KEY_EXT_LEFT;
		break;
	case SDLK_LCTRL: 
		evt->key_code = GF_KEY_CONTROL;
		evt->flags = GF_KEY_EXT_LEFT;
		break;
	case SDLK_RCTRL: 
		evt->key_code = GF_KEY_CONTROL;
		evt->flags = GF_KEY_EXT_RIGHT;
		break;
	case SDLK_LALT: 
		evt->key_code = GF_KEY_ALT;
		evt->flags = GF_KEY_EXT_LEFT;
		break;
	case SDLK_RALT: 
		evt->key_code = GF_KEY_ALT;
		evt->flags = GF_KEY_EXT_RIGHT;
		break;
/*	case SDLK_LSUPER:
		evt->key_code = GF_KEY_META;
		evt->flags = GF_KEY_EXT_LEFT;
		break;
	case SDLK_RSUPER:
		evt->key_code = GF_KEY_META;
		evt->flags = GF_KEY_EXT_RIGHT;
		break;*/
	case SDLK_MODE: evt->key_code = GF_KEY_MODECHANGE; break;
	case SDLK_COMPOSE: evt->key_code = GF_KEY_COMPOSE; break;
	case SDLK_HELP: evt->key_code = GF_KEY_HELP; break;
	case SDLK_PRINT: evt->key_code = GF_KEY_PRINTSCREEN; break;

#if (SDL_MAJOR_VERSION>=1) && (SDL_MINOR_VERSION>=3)
/*
	SDLK_CARET		= 94,
 */
	case SDLK_a:
	case SDLK_b:
	case SDLK_c:
	case SDLK_d:
	case SDLK_e:
	case SDLK_f:
	case SDLK_g:
	case SDLK_h:
	case SDLK_i:
	case SDLK_j:
	case SDLK_k:
	case SDLK_l:
	case SDLK_m:
	case SDLK_n:
	case SDLK_o:
	case SDLK_p:
	case SDLK_q:
	case SDLK_r:
	case SDLK_s:
	case SDLK_t:
	case SDLK_u:
	case SDLK_v:
	case SDLK_w:
	case SDLK_x:
	case SDLK_y:
	case SDLK_z:
		evt->key_code = GF_KEY_A + SDLkey - SDLK_a;
		break;
			/*
 SDLK_DELETE		= 127,

	SDLK_SYSREQ		= 317,
	SDLK_POWER		= 320,

*/
#endif
			
	default:
		if ((SDLkey>=0x30) && (SDLkey<=0x39))  evt->key_code = GF_KEY_0 + SDLkey-0x30;
		else if ((SDLkey>=0x41) && (SDLkey<=0x5A))  evt->key_code = GF_KEY_A + SDLkey-0x51;
		else
		{
			evt->key_code = GF_KEY_UNIDENTIFIED;
		}
		break;
	}
}

#if 0
void SDLVid_SetHack(void *os_handle, Bool set_on)
{
	char buf[50];
	if (set_on && os_handle) {
		sprintf(buf, "SDL_WINDOWID=%u", (u32) os_handle);
	} else {
		strcpy(buf, "SDL_WINDOWID=");
	}
#ifdef WIN32
	putenv(buf);
#else
	if (set_on) unsetenv("SDL_WINDOWID=");
	else putenv(buf);
#endif
}
#endif

static void SDLVid_DestroyObjects(SDLVidCtx *ctx)
{
	if (ctx->back_buffer) SDL_FreeSurface(ctx->back_buffer);
	ctx->back_buffer = NULL;
	if (ctx->pool_rgb) SDL_FreeSurface(ctx->pool_rgb);
	ctx->pool_rgb = NULL;
	if (ctx->pool_rgba) SDL_FreeSurface(ctx->pool_rgba);
	ctx->pool_rgba = NULL;
	SDL_FreeYUVOverlay(ctx->yuv_overlay);
	ctx->yuv_overlay=NULL;
}


#ifdef GPAC_IPHONE
#define SDL_WINDOW_FLAGS			SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_RESIZABLE | SDL_NOFRAME
#define SDL_FULLSCREEN_FLAGS		SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_FULLSCREEN | SDL_NOFRAME
#define SDL_GL_WINDOW_FLAGS			SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE | SDL_NOFRAME
#define SDL_GL_FULLSCREEN_FLAGS		SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN | SDL_NOFRAME
#else
#define SDL_WINDOW_FLAGS			SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_RESIZABLE
#define SDL_FULLSCREEN_FLAGS		SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_FULLSCREEN
#define SDL_GL_WINDOW_FLAGS			SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE
#define SDL_GL_FULLSCREEN_FLAGS		SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN
#endif


GF_Err SDLVid_ResizeWindow(GF_VideoOutput *dr, u32 width, u32 height) 
{
	SDLVID();
	GF_Event evt;

	/*lock X mutex to make sure the event queue is not being processed*/
	gf_mx_p(ctx->evt_mx);

	if (ctx->output_3d_type==1) {
		u32 flags, nb_bits;
		const char *opt;
		if ((ctx->width==width) && (ctx->height==height) ) {
			gf_mx_v(ctx->evt_mx);
			return GF_OK;
		}
		flags = SDL_GL_WINDOW_FLAGS;
		if (ctx->os_handle) flags &= ~SDL_RESIZABLE;
		if (ctx->fullscreen) flags |= SDL_FULLSCREEN_FLAGS;
		if (!ctx->screen) ctx->screen = SDL_SetVideoMode(width, height, 0, flags);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		opt = gf_modules_get_option((GF_BaseInterface *)dr, "Video", "GLNbBitsDepth");
		nb_bits = opt ? atoi(opt) : 16;
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, nb_bits);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
		opt = gf_modules_get_option((GF_BaseInterface *)dr, "Video", "GLNbBitsPerComponent");
		nb_bits = opt ? atoi(opt) : 5;
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, nb_bits);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, nb_bits);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, nb_bits);

		assert(width);
		assert(height);
		ctx->screen = SDL_SetVideoMode(width, height, 0, flags);
		assert(ctx->screen);
		ctx->width = width;
		ctx->height = height;
		evt.type = GF_EVENT_VIDEO_SETUP;
		dr->on_event(dr->evt_cbk_hdl, &evt);		
	} else {
		u32 flags;
#ifdef GPAC_IPHONE
		flags = SDL_FULLSCREEN_FLAGS;
		//SDL readme says it would make us faster
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
#else
		flags = SDL_WINDOW_FLAGS;
#endif
		if (ctx->os_handle) flags &= ~SDL_RESIZABLE;
		ctx->screen = SDL_SetVideoMode(width, height, 0, flags);
	}
	gf_mx_v(ctx->evt_mx);
	return ctx->screen ? GF_OK : GF_IO_ERR;
}


static Bool SDLVid_InitializeWindow(SDLVidCtx *ctx, GF_VideoOutput *dr)
{
	u32 flags;
	const SDL_VideoInfo *vinf;

	putenv("directx");
	flags = SDL_WasInit(SDL_INIT_VIDEO);
	if (!(flags & SDL_INIT_VIDEO)) {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO)<0) {
			return 0;
		}
	}

	ctx->curs_def = SDL_GetCursor();
	ctx->curs_hand = SDLVid_LoadCursor(hand_data);
	ctx->curs_collide = SDLVid_LoadCursor(collide_data);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	ctx->last_mouse_move = SDL_GetTicks();
	ctx->cursor_on = 1;

	/*save display resolution - SDL seems to get the screen resolution if asked for video info before
	changing the video mode - to check on other platforms*/
	vinf = SDL_GetVideoInfo();
#if SDL_VERSION_ATLEAST(1, 2, 10)
	dr->max_screen_width = vinf->current_w;
	dr->max_screen_height = vinf->current_h;
#else
	{
		SDL_Rect** modes;
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
		assert( (modes != (SDL_Rect**)0));
		if ( modes == (SDL_Rect**)-1 ){
			fprintf(stderr, "SDL : DONT KNOW WHICH MODE TO USE, using 640x480\n");
			dr->max_screen_width = 640;
			dr->max_screen_height = 480;
		} else {
			int i;
			dr->max_screen_width = 0;
			for (i=0; modes[i]; ++i){
				int w = modes[i]->w;
				if (w > dr->max_screen_width){
					dr->max_screen_width = w;
					dr->max_screen_height = modes[i]->h;
				}
			}
		}
	}
#endif /* versions prior to 1.2.10 do not have the size of screen */

	SDLVid_ResizeWindow(dr, 100, 100);
	if (!ctx->os_handle) SDLVid_SetCaption();
	GF_LOG(GF_LOG_INFO, GF_LOG_MMIO, ("[SDL] Video output initialized - screen resolution %d %d\n", dr->max_screen_width, dr->max_screen_height)); 
	return 1;
}

static void SDLVid_ShutdownWindow(SDLVidCtx *ctx)
{
	SDLVid_DestroyObjects(ctx);
	SDL_FreeCursor(ctx->curs_hand);
	SDL_FreeCursor(ctx->curs_collide);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

#if defined SDL_TEXTINPUTEVENT_TEXT_SIZE && !defined GPAC_IPHONE
#include <gpac/unicode.h>
#endif


Bool SDLVid_ProcessMessageQueue(SDLVidCtx *ctx, GF_VideoOutput *dr)
{
	SDL_Event sdl_evt;
	GF_Event gpac_evt;

	while (SDL_PollEvent(&sdl_evt)) {
		switch (sdl_evt.type) {
		case SDL_VIDEORESIZE:
		  	gpac_evt.type = GF_EVENT_SIZE;
			gpac_evt.size.width = sdl_evt.resize.w;
			gpac_evt.size.height = sdl_evt.resize.h;
			dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
			break;
		case SDL_QUIT:
			gpac_evt.type = GF_EVENT_QUIT;
			dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
			return 0;

		case SDL_VIDEOEXPOSE:
			gpac_evt.type = GF_EVENT_REFRESH;
			dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
			break;
#ifdef SDL_TEXTINPUTEVENT_TEXT_SIZE
		/*keyboard*/
		case SDL_TEXTINPUT: /* Since SDL 1.3, text-input is handled in a specific event */
		{
			u32 len = strlen( sdl_evt.text.text);
			u32 ucs4_len;
			assert( len < 5 );
			ucs4_len = utf8_to_ucs4 (&(gpac_evt.character.unicode_char), len, sdl_evt.text.text);
			gpac_evt.type = GF_EVENT_TEXTINPUT;
			dr->on_event(dr->evt_cbk_hdl, &gpac_evt);	
			break;
		}
#endif /* SDL_TEXTINPUTEVENT_TEXT_SIZE */
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			sdl_translate_key(sdl_evt.key.keysym.sym, &gpac_evt.key);
			gpac_evt.type = (sdl_evt.key.type==SDL_KEYDOWN) ? GF_EVENT_KEYDOWN : GF_EVENT_KEYUP;
			dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
			if ((sdl_evt.key.type==SDL_KEYDOWN)
				&& sdl_evt.key.keysym.unicode
#ifdef SDL_TEXTINPUTEVENT_TEXT_SIZE
				&& ((sdl_evt.key.keysym.unicode=='\r') || (sdl_evt.key.keysym.unicode=='\n')  || (sdl_evt.key.keysym.unicode=='\b') || (sdl_evt.key.keysym.unicode=='\t') )
#endif
			) {
				gpac_evt.character.unicode_char = sdl_evt.key.keysym.unicode;
				gpac_evt.type = GF_EVENT_TEXTINPUT;
				dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
			}
			break;

		/*mouse*/
		case SDL_MOUSEMOTION:
			ctx->last_mouse_move = SDL_GetTicks();
			gpac_evt.type = GF_EVENT_MOUSEMOVE;
			gpac_evt.mouse.x = sdl_evt.motion.x;
			gpac_evt.mouse.y = sdl_evt.motion.y;
			dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			ctx->last_mouse_move = SDL_GetTicks();
			gpac_evt.mouse.x = sdl_evt.motion.x;
			gpac_evt.mouse.y = sdl_evt.motion.y;
			gpac_evt.type = (sdl_evt.type==SDL_MOUSEBUTTONUP) ? GF_EVENT_MOUSEUP : GF_EVENT_MOUSEDOWN;
			switch (sdl_evt.button.button) {
			case SDL_BUTTON_LEFT: 
				gpac_evt.mouse.button = GF_MOUSE_LEFT;
				dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
				break;
			case SDL_BUTTON_MIDDLE: 
				gpac_evt.mouse.button = GF_MOUSE_MIDDLE;
				dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
				break;
			case SDL_BUTTON_RIGHT: 
				gpac_evt.mouse.button = GF_MOUSE_RIGHT;
				dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
				break;
#ifdef SDL_BUTTON_WHEELUP
			case SDL_BUTTON_WHEELUP:
			case SDL_BUTTON_WHEELDOWN:
				/*SDL handling is not perfect there, it just says up/down but no info on how much
				the wheel was rotated...*/
				gpac_evt.mouse.wheel_pos = (sdl_evt.button.button==SDL_BUTTON_WHEELUP) ? FIX_ONE : -FIX_ONE;
				gpac_evt.type = GF_EVENT_MOUSEWHEEL;
				dr->on_event(dr->evt_cbk_hdl, &gpac_evt);
				break;
#endif
			}
			break;
		}
	}
	return 1;
}

#ifdef	SDL_WINDOW_THREAD
u32 SDLVid_EventProc(void *par)
{
#if 0
	u32 last_mouse_move;
#endif
	Bool ret;
	GF_VideoOutput *dr = (GF_VideoOutput *)par;
	SDLVID();

	if (!SDLVid_InitializeWindow(ctx, dr)) {
		ctx->sdl_th_state = 3;
	}
	ctx->sdl_th_state = 1;

	while (ctx->sdl_th_state==1) {
		/*after much testing: we must ensure nothing is using the event queue when resizing window.
		-- under X, it throws Xlib "unexpected async reply" under linux, therefore we don't wait events,
		we check for events and execute them if any
		-- under Win32, the SDL_SetVideoMode deadlocks, so we don't force exclusive access to events
		*/
#ifndef WIN32
		gf_mx_p(ctx->evt_mx);
#endif

		ret = SDLVid_ProcessMessageQueue(ctx, dr);

#ifndef WIN32
		gf_mx_v(ctx->evt_mx);
#endif

		/*looks like this hides the cursor for ever when switching back from FS*/
#if 0
		if (ctx->fullscreen && (last_mouse_move + 2000 < SDL_GetTicks()) ) {
			if (cursor_on) SDL_ShowCursor(0);
			cursor_on = 0;
		} else if (!cursor_on) {
			SDL_ShowCursor(1);
			cursor_on = 1;
		}
#endif
	
		/*QUIT message has been processed*/
		if (!ret) break;

		gf_sleep(0);
	}

	SDLVid_ShutdownWindow(ctx);
	ctx->sdl_th_state = 3;
	return 0;
}
#endif /*SDL_WINDOW_THREAD*/


GF_Err SDLVid_Setup(struct _video_out *dr, void *os_handle, void *os_display, u32 init_flags)
{
	SDLVID();
	/*we don't allow SDL hack, not stable enough*/
	//if (os_handle) SDLVid_SetHack(os_handle, 1);

	ctx->os_handle = os_handle;
	ctx->is_init = 0;
	ctx->output_3d_type = 0;
	ctx->force_alpha = (init_flags & GF_TERM_WINDOW_TRANSPARENT) ? 1 : 0;

	if (!SDLOUT_InitSDL()) return GF_IO_ERR;

#ifdef	SDL_WINDOW_THREAD
	ctx->sdl_th_state = 0;
	gf_th_run(ctx->sdl_th, SDLVid_EventProc, dr);
	while (!ctx->sdl_th_state) gf_sleep(10);
	if (ctx->sdl_th_state==3) {
		SDLOUT_CloseSDL();
		ctx->sdl_th_state = 0;
		return GF_IO_ERR;
	}
#else
	if (!SDLVid_InitializeWindow(ctx, dr)) {
		SDLOUT_CloseSDL();
		return GF_IO_ERR;
	}
#endif

	ctx->is_init = 1;
	return GF_OK;
}

static void SDLVid_Shutdown(GF_VideoOutput *dr)
{
	SDLVID();
	/*remove all surfaces*/

	if (!ctx->is_init) return;
#ifdef	SDL_WINDOW_THREAD
	if (ctx->sdl_th_state==1) {
		SDL_Event evt;
		ctx->sdl_th_state = 2;
		evt.type = SDL_QUIT;
		SDL_PushEvent(&evt);
		while (ctx->sdl_th_state != 3) gf_sleep(100);
	}
#else
	SDLVid_ShutdownWindow(ctx);
#endif

	SDLOUT_CloseSDL();
	ctx->is_init = 0;
}


GF_Err SDLVid_SetFullScreen(GF_VideoOutput *dr, u32 bFullScreenOn, u32 *screen_width, u32 *screen_height)
{
	u32 bpp, pref_bpp;
	SDLVID();

	if (ctx->fullscreen==bFullScreenOn) return GF_OK;

	/*lock to get sure the event queue is not processed under X*/
	gf_mx_p(ctx->evt_mx);
	ctx->fullscreen = bFullScreenOn;

	pref_bpp = bpp = ctx->screen->format->BitsPerPixel;

	if (ctx->fullscreen) {
		u32 flags;
		Bool switch_res = 0;
		const char *sOpt = gf_modules_get_option((GF_BaseInterface *)dr, "Video", "SwitchResolution");
		if (sOpt && !stricmp(sOpt, "yes")) switch_res = 1;
		if (!dr->max_screen_width || !dr->max_screen_height) switch_res = 1;

		flags = (ctx->output_3d_type==1) ? SDL_GL_FULLSCREEN_FLAGS : SDL_FULLSCREEN_FLAGS;
		ctx->store_width = *screen_width;
		ctx->store_height = *screen_height;
		if (switch_res) {
			u32 i;
			ctx->fs_width = *screen_width;
			ctx->fs_height = *screen_height;
			for(i=0; i<nb_video_modes; i++) {
				if (ctx->fs_width<=video_modes[2*i] && ctx->fs_height<=video_modes[2*i + 1]) {
					if ((pref_bpp = SDL_VideoModeOK(video_modes[2*i], video_modes[2*i+1], bpp, flags))) {
						ctx->fs_width = video_modes[2*i];
						ctx->fs_height = video_modes[2*i + 1];
						break;
					}
				}
			}
		} else {
			ctx->fs_width = dr->max_screen_width;
			ctx->fs_height = dr->max_screen_height;
		}
		ctx->screen = SDL_SetVideoMode(ctx->fs_width, ctx->fs_height, pref_bpp, flags);
		/*we switched bpp, clean all objects*/
		if (bpp != pref_bpp) SDLVid_DestroyObjects(ctx);
		*screen_width = ctx->fs_width;
		*screen_height = ctx->fs_height;
		/*GL has changed*/
		if (ctx->output_3d_type==1) {
			GF_Event evt;
			evt.type = GF_EVENT_VIDEO_SETUP;
			dr->on_event(dr->evt_cbk_hdl, &evt);
		}
	} else {
		SDLVid_ResizeWindow(dr, ctx->store_width, ctx->store_height);
		*screen_width = ctx->store_width;
		*screen_height = ctx->store_height;
	}
	gf_mx_v(ctx->evt_mx);
	if (!ctx->screen) return GF_IO_ERR;
	return GF_OK;
}

GF_Err SDLVid_SetBackbufferSize(GF_VideoOutput *dr, u32 newWidth, u32 newHeight, Bool system_mem)
{
	u32 col;
	const char *opt;
	SDLVID();
	
	if (ctx->output_3d_type==1) return GF_BAD_PARAM;

	opt = gf_modules_get_option((GF_BaseInterface *)dr, "Video", "HardwareMemory");
	if (system_mem) {
		if (opt && !strcmp(opt, "Always")) system_mem = 0;
	} else {
		if (opt && !strcmp(opt, "Never")) system_mem = 1;
	}
	ctx->use_systems_memory = system_mem;


	/*clear screen*/
	col = SDL_MapRGB(ctx->screen->format, 0, 0, 0);
	SDL_FillRect(ctx->screen, NULL, col);
	SDL_Flip(ctx->screen);

	if (ctx->back_buffer && ((u32) ctx->back_buffer->w==newWidth) && ((u32) ctx->back_buffer->h==newHeight)) {
		return GF_OK;
	}
	if (ctx->back_buffer) SDL_FreeSurface(ctx->back_buffer);
	ctx->back_buffer = SDL_CreateRGBSurface(ctx->use_systems_memory ? SDL_SWSURFACE : SDL_HWSURFACE, newWidth, newHeight, ctx->screen->format->BitsPerPixel, ctx->screen->format->Rmask, ctx->screen->format->Gmask, ctx->screen->format->Bmask, 0);
	ctx->width = newWidth;
	ctx->height = newHeight;
	if (!ctx->back_buffer) return GF_IO_ERR;

	return GF_OK;
}

u32 SDLVid_MapPixelFormat(SDL_PixelFormat *format, Bool force_alpha)
{
	if (format->palette) return 0;
	switch (format->BitsPerPixel) {
	case 16:
		if ((format->Rmask==0x7c00) && (format->Gmask==0x03e0) && (format->Bmask==0x001f) ) return GF_PIXEL_RGB_555;
		if ((format->Rmask==0xf800) && (format->Gmask==0x07e0) && (format->Bmask==0x001f) ) return GF_PIXEL_RGB_565;
		return 0;
	case 24:
		if (format->Rmask==0x00FF0000) return GF_PIXEL_RGB_24;
		if (format->Rmask==0x000000FF) return GF_PIXEL_BGR_24;
		return 0;
	case 32:
		if (format->Amask==0xFF000000) return GF_PIXEL_ARGB;
		if (format->Rmask==0x00FF0000) return force_alpha ? GF_PIXEL_ARGB : GF_PIXEL_RGB_32;
		if (format->Rmask==0x000000FF) return force_alpha ? GF_PIXEL_RGBA : GF_PIXEL_BGR_32;
		return 0;
	default:
		return 0;
	}
}

#ifdef UNUSED_FUNC
static char szTYPE[5];
static const char *my_gf_4cc_to_str(u32 type)
{
	u32 ch, i;
	char *ptr, *name = (char *)szTYPE;
	ptr = name;
	for (i = 0; i < 4; i++, name++) {
		ch = type >> (8 * (3-i) ) & 0xff;
		if ( ch >= 0x20 && ch <= 0x7E ) {
			*name = ch;
		} else {
			*name = '.';
		}
	}
	*name = 0;
	return (const char *) ptr;
}
#endif /* UNUSED_FUNC */

static GF_Err SDLVid_LockBackBuffer(GF_VideoOutput *dr, GF_VideoSurface *video_info, u32 do_lock)
{
	SDLVID();
	if (!ctx->back_buffer) return GF_BAD_PARAM;
	if (do_lock) {
		if (!video_info) return GF_BAD_PARAM;
		if (SDL_LockSurface(ctx->back_buffer)<0) return GF_IO_ERR;
		video_info->width = ctx->back_buffer->w;
		video_info->height = ctx->back_buffer->h;
		video_info->pitch_x = 0;
		video_info->pitch_y = ctx->back_buffer->pitch;
		video_info->video_buffer = ctx->back_buffer->pixels;
		video_info->pixel_format = SDLVid_MapPixelFormat(ctx->back_buffer->format, ctx->force_alpha);
		video_info->is_hardware_memory = !ctx->use_systems_memory;
	} else {
		SDL_UnlockSurface(ctx->back_buffer);
	}
	return GF_OK;
}

static GF_Err SDLVid_Flush(GF_VideoOutput *dr, GF_Window *dest)
{
	SDL_Rect rc;
	SDLVID();
	/*if resizing don't process otherwise we may deadlock*/
	if (!ctx->screen) return GF_OK;

	if (ctx->output_3d_type==1) {
		SDL_GL_SwapBuffers();
		return GF_OK;
	}
	if (!ctx->back_buffer) return GF_BAD_PARAM;

	if ((dest->w != (u32) ctx->back_buffer->w) || (dest->h != (u32) ctx->back_buffer->h)) {
		GF_VideoSurface src, dst;

		SDL_LockSurface(ctx->back_buffer);
		SDL_LockSurface(ctx->screen);

		src.height = ctx->back_buffer->h;
		src.width = ctx->back_buffer->w;
		src.pitch_x = 0;
		src.pitch_y = ctx->back_buffer->pitch;
		src.pixel_format = SDLVid_MapPixelFormat(ctx->back_buffer->format, ctx->force_alpha);
		src.video_buffer = ctx->back_buffer->pixels;

		dst.height = ctx->screen->h;
		dst.width = ctx->screen->w;
		dst.pitch_x = 0;
		dst.pitch_y = ctx->screen->pitch;
		dst.pixel_format = SDLVid_MapPixelFormat(ctx->screen->format, 0);
		dst.video_buffer = ctx->screen->pixels;

		gf_stretch_bits(&dst, &src, dest, NULL, 0xFF, 0, NULL, NULL);

		SDL_UnlockSurface(ctx->back_buffer);
		SDL_UnlockSurface(ctx->screen);
	} else {
		rc.x = dest->x; rc.y = dest->y; rc.w = dest->w; rc.h = dest->h;
		SDL_BlitSurface(ctx->back_buffer, NULL, ctx->screen, &rc);
	}
	SDL_Flip(ctx->screen);
	return GF_OK;
}

static void SDLVid_SetCursor(GF_VideoOutput *dr, u32 cursor_type)
{
	SDLVID();
	switch (cursor_type) {
	case GF_CURSOR_ANCHOR:
	case GF_CURSOR_TOUCH:
	case GF_CURSOR_ROTATE:
	case GF_CURSOR_PROXIMITY:
	case GF_CURSOR_PLANE:
		SDL_SetCursor(ctx->curs_hand);
		break;
	case GF_CURSOR_COLLIDE:
		SDL_SetCursor(ctx->curs_collide);
		break;
	default:
		SDL_SetCursor(ctx->curs_def);
		break;
	}
}


#ifdef WIN32
u32 get_sys_col(int idx)
{
	u32 res;
	DWORD val = GetSysColor(idx);
	res = (val)&0xFF; res<<=8;
	res |= (val>>8)&0xFF; res<<=8;
	res |= (val>>16)&0xFF;
	return res;
}
#endif

static GF_Err SDLVid_ProcessEvent(GF_VideoOutput *dr, GF_Event *evt)
{
	if (!evt) {
#ifndef	SDL_WINDOW_THREAD
		SDLVID();
		SDLVid_ProcessMessageQueue(ctx, dr);
#endif		
		return GF_OK;
	}
	switch (evt->type) {
	case GF_EVENT_SET_CURSOR:
		SDLVid_SetCursor(dr, evt->cursor.cursor_type);
		break;
	case GF_EVENT_SET_CAPTION:
		SDL_WM_SetCaption(evt->caption.caption, NULL);
		break;
	case GF_EVENT_SHOWHIDE:
		/*the only way to have proper show/hide with SDL is to shutdown the video system and reset it up
		which we don't want to do since the setup MUST occur in the rendering thread for some configs (openGL)*/
		return GF_NOT_SUPPORTED;
	case GF_EVENT_SIZE:
		SDLVid_ResizeWindow(dr, evt->size.width, evt->size.height);
		break;
	case GF_EVENT_MOVE:
		break;
	case GF_EVENT_VIDEO_SETUP:
	{
		SDLVID();
		switch (evt->setup.opengl_mode) {
		case 0:
			/*force a resetup of the window*/
			if (ctx->output_3d_type) {
				ctx->width = ctx->height = 0;
			}
			ctx->output_3d_type = 0;
			return SDLVid_SetBackbufferSize(dr, evt->setup.width, evt->setup.height, evt->setup.system_memory);
		case 1:
			/*force a resetup of the window*/
			if (!ctx->output_3d_type) {
				ctx->width = ctx->height = 0;
			}
			ctx->output_3d_type = 1;
			return SDLVid_ResizeWindow(dr, evt->setup.width, evt->setup.height);
		case 2:
			/*find a way to do that in SDL*/
			ctx->output_3d_type = 2;
			GF_LOG(GF_LOG_ERROR, GF_LOG_MMIO, ("[SDL] 3D not supported with SDL.\n"));
			return GF_NOT_SUPPORTED;
		}
		default:
			GF_LOG(GF_LOG_ERROR, GF_LOG_MMIO, ("[SDL] Trying to set an Unknown Mode %d !\n", evt->setup.opengl_mode));
			return GF_NOT_SUPPORTED;
	}
		break;
	case GF_EVENT_SYS_COLORS:
#ifdef WIN32
		evt->sys_cols.sys_colors[0] = get_sys_col(COLOR_ACTIVEBORDER);
		evt->sys_cols.sys_colors[1] = get_sys_col(COLOR_ACTIVECAPTION);
		evt->sys_cols.sys_colors[2] = get_sys_col(COLOR_APPWORKSPACE);
		evt->sys_cols.sys_colors[3] = get_sys_col(COLOR_BACKGROUND);
		evt->sys_cols.sys_colors[4] = get_sys_col(COLOR_BTNFACE);
		evt->sys_cols.sys_colors[5] = get_sys_col(COLOR_BTNHIGHLIGHT);
		evt->sys_cols.sys_colors[6] = get_sys_col(COLOR_BTNSHADOW);
		evt->sys_cols.sys_colors[7] = get_sys_col(COLOR_BTNTEXT);
		evt->sys_cols.sys_colors[8] = get_sys_col(COLOR_CAPTIONTEXT);
		evt->sys_cols.sys_colors[9] = get_sys_col(COLOR_GRAYTEXT);
		evt->sys_cols.sys_colors[10] = get_sys_col(COLOR_HIGHLIGHT);
		evt->sys_cols.sys_colors[11] = get_sys_col(COLOR_HIGHLIGHTTEXT);
		evt->sys_cols.sys_colors[12] = get_sys_col(COLOR_INACTIVEBORDER);
		evt->sys_cols.sys_colors[13] = get_sys_col(COLOR_INACTIVECAPTION);
		evt->sys_cols.sys_colors[14] = get_sys_col(COLOR_INACTIVECAPTIONTEXT);
		evt->sys_cols.sys_colors[15] = get_sys_col(COLOR_INFOBK);
		evt->sys_cols.sys_colors[16] = get_sys_col(COLOR_INFOTEXT);
		evt->sys_cols.sys_colors[17] = get_sys_col(COLOR_MENU);
		evt->sys_cols.sys_colors[18] = get_sys_col(COLOR_MENUTEXT);
		evt->sys_cols.sys_colors[19] = get_sys_col(COLOR_SCROLLBAR);
		evt->sys_cols.sys_colors[20] = get_sys_col(COLOR_3DDKSHADOW);
		evt->sys_cols.sys_colors[21] = get_sys_col(COLOR_3DFACE);
		evt->sys_cols.sys_colors[22] = get_sys_col(COLOR_3DHIGHLIGHT);
		evt->sys_cols.sys_colors[23] = get_sys_col(COLOR_3DLIGHT);
		evt->sys_cols.sys_colors[24] = get_sys_col(COLOR_3DSHADOW);
		evt->sys_cols.sys_colors[25] = get_sys_col(COLOR_WINDOW);
		evt->sys_cols.sys_colors[26] = get_sys_col(COLOR_WINDOWFRAME);
		evt->sys_cols.sys_colors[27] = get_sys_col(COLOR_WINDOWTEXT);
		return GF_OK;
#else
		return GF_NOT_SUPPORTED;
#endif
	}
	return GF_OK;
}


static void copy_yuv(u8 *pYD, u8 *pVD, u8 *pUD, u32 pixel_format , u32 pitch_y, unsigned char *src, u32 src_stride, u32 src_pf,
								 u32 src_width, u32 src_height, const GF_Window *src_wnd)
{
	unsigned char *pY, *pU, *pV;
	pY = src;
	pU = src + src_stride * src_height;
	pV = src + 5*src_stride * src_height/4;


	pY = pY + src_stride * src_wnd->y + src_wnd->x;
	/*because of U and V downsampling by 2x2, working with odd Y offset will lead to a half-line shift between Y and UV components. We
	therefore force an even Y offset for U and V planes.*/
	pU = pU + (src_stride * (src_wnd->y / 2) + src_wnd->x) / 2;
	pV = pV + (src_stride * (src_wnd->y / 2) + src_wnd->x) / 2;


	/*complete source copy*/
	if ( (pitch_y == (s32) src_stride) && (src_wnd->w == src_width) && (src_wnd->h == src_height)) {
		assert(!src_wnd->x);
		assert(!src_wnd->y);
		memcpy(pYD, pY, sizeof(unsigned char)*src_width*src_height);
		memcpy(pVD, pV, sizeof(unsigned char)*src_width*src_height/4);
		memcpy(pUD, pU, sizeof(unsigned char)*src_width*src_height/4);
	} else {
		u32 i;
		unsigned char *dst, *src, *dst2, *src2, *dst3, *src3;

		src = pY;
		dst = pYD;
		
		src2 = (pixel_format != GF_PIXEL_YV12) ? pU : pV;
		dst2 = pVD;
		src3 = (pixel_format  != GF_PIXEL_YV12) ? pV : pU;
		dst3 = pUD;
		for (i=0; i<src_wnd->h; i++) {
			memcpy(dst, src, src_wnd->w);
			src += src_stride;
			dst += pitch_y;
			if (i<src_wnd->h/2) {
				memcpy(dst2, src2, src_wnd->w/2);
				src2 += src_stride/2;
				dst2 += pitch_y/2;
				memcpy(dst3, src3, src_wnd->w/2);
				src3 += src_stride/2;
				dst3 += pitch_y/2;
			}
		}
	}
}

static GF_Err SDL_Blit(GF_VideoOutput *dr, GF_VideoSurface *video_src, GF_Window *src_wnd, GF_Window *dst_wnd, u32 overlay_type)
{
	SDLVID();
	u32 amask = 0;
	u32 bpp, i;
	u8 *dst, *src;
	SDL_Rect srcrc, dstrc;
	SDL_Surface **pool;

	if (overlay_type) {
		if (!video_src) {
			if (ctx->yuv_overlay) {
				SDL_FreeYUVOverlay(ctx->yuv_overlay);
				ctx->yuv_overlay=NULL;
			}
			return GF_OK;
		}
		if (!ctx->yuv_overlay || (ctx->yuv_overlay->w != src_wnd->w) || (ctx->yuv_overlay->h != src_wnd->h) ) {
			if (ctx->yuv_overlay) SDL_FreeYUVOverlay(ctx->yuv_overlay);

			ctx->yuv_overlay = SDL_CreateYUVOverlay(src_wnd->w, src_wnd->h, SDL_YV12_OVERLAY, ctx->screen);
			if (!ctx->yuv_overlay) return GF_NOT_SUPPORTED;
		}
		/*copy pixels*/
		SDL_LockYUVOverlay(ctx->yuv_overlay);

		copy_yuv(ctx->yuv_overlay->pixels[0], ctx->yuv_overlay->pixels[1], ctx->yuv_overlay->pixels[2], GF_PIXEL_YV12, ctx->yuv_overlay->pitches[0], 
			video_src->video_buffer, video_src->pitch_y, video_src->pixel_format,
			video_src->width, video_src->height, src_wnd);

		SDL_UnlockYUVOverlay(ctx->yuv_overlay);

		dstrc.w = dst_wnd->w;
		dstrc.h = dst_wnd->h;
		dstrc.x = dst_wnd->x;
		dstrc.y = dst_wnd->y;
		SDL_DisplayYUVOverlay(ctx->yuv_overlay, &dstrc);
		return GF_OK;
	}

	/*SDL doesn't support stretching ...*/
	if ((src_wnd->w != dst_wnd->w) || (src_wnd->h!=dst_wnd->h)) return GF_NOT_SUPPORTED;

	switch (video_src->pixel_format) {
	case GF_PIXEL_RGB_24:
		pool = &ctx->pool_rgb;
		bpp = 3;
		break;
	case GF_PIXEL_RGBA:
		pool = &ctx->pool_rgba;
		amask = 0xFF000000;
		bpp = 4;
		break;
	default:
		return GF_NOT_SUPPORTED;
	}

	if (! *pool || ((*pool)->w < (int) src_wnd->w) || ((*pool)->h < (int) src_wnd->h) ) {
		if ((*pool)) SDL_FreeSurface((*pool));
		(*pool) = SDL_CreateRGBSurface(ctx->use_systems_memory ? SDL_SWSURFACE : SDL_HWSURFACE, 
						src_wnd->w, src_wnd->h, 8*bpp, 
						0x000000FF, 0x0000FF00, 0x00FF0000, amask);
		if (! (*pool) ) return GF_IO_ERR;
	}

	SDL_LockSurface(*pool);

	dst = (u8 *) ( (*pool)->pixels);
	src = video_src->video_buffer + video_src->pitch_y*src_wnd->y + src_wnd->x*bpp;
	for (i=0; i<src_wnd->h; i++) {
		memcpy(dst, src, bpp * src_wnd->w);
		src += video_src->pitch_y;
		dst += (*pool)->pitch;
	}
	SDL_UnlockSurface(*pool);

	srcrc.w = src_wnd->w;
	srcrc.h = src_wnd->h;
	srcrc.x = 0;
	srcrc.y = 0;

	dstrc.w = dst_wnd->w;
	dstrc.h = dst_wnd->h;
	dstrc.x = dst_wnd->x;
	dstrc.y = dst_wnd->y;

	SDL_BlitSurface(*pool, &srcrc, ctx->back_buffer, &dstrc);
	return GF_OK;
}



void *SDL_NewVideo()
{
	SDLVidCtx *ctx;
	GF_VideoOutput *driv;
	
	driv = gf_malloc(sizeof(GF_VideoOutput));
	memset(driv, 0, sizeof(GF_VideoOutput));
	GF_REGISTER_MODULE_INTERFACE(driv, GF_VIDEO_OUTPUT_INTERFACE, "SDL Video Output", "gpac distribution");

	ctx = gf_malloc(sizeof(SDLVidCtx));
	memset(ctx, 0, sizeof(SDLVidCtx));
#ifdef	SDL_WINDOW_THREAD
	ctx->sdl_th = gf_th_new("SDLVideo");
#endif
	ctx->evt_mx = gf_mx_new("SDLEvents");
	
	driv->opaque = ctx;
	driv->Setup = SDLVid_Setup;
	driv->Shutdown = SDLVid_Shutdown;
	driv->SetFullScreen = SDLVid_SetFullScreen;
	driv->Flush = SDLVid_Flush;
	driv->ProcessEvent = SDLVid_ProcessEvent;
	/*no offscreen opengl with SDL*/
	driv->hw_caps |= GF_VIDEO_HW_OPENGL;

	/*no YUV hardware blitting in SDL (only overlays)*/
	driv->hw_caps |= GF_VIDEO_HW_HAS_YUV_OVERLAY | GF_VIDEO_HW_HAS_RGB | GF_VIDEO_HW_HAS_RGBA;
	driv->Blit = SDL_Blit;
	driv->LockBackBuffer = SDLVid_LockBackBuffer;
	driv->LockOSContext = NULL;

	/*color keying with overlays are not supported in SDL ...*/
#if 0
	/*get YUV overlay key*/
	opt = gf_modules_get_option((GF_BaseInterface *)driv, "Video", "OverlayColorKey");
	/*no set is the default*/
	if (!opt) {
		opt = "0101FE";
		gf_modules_set_option((GF_BaseInterface *)driv, "Video", "OverlayColorKey", "0101FE");
	}
	sscanf(opt, "%06x", &driv->overlay_color_key);
	if (driv->overlay_color_key) driv->overlay_color_key |= 0xFF000000;
	GF_LOG(GF_LOG_INFO, GF_LOG_MMIO, ("[SDL Out] YUV Overlays enabled - ColorKey enabled: %s (key %x)\n", 
									driv->overlay_color_key ? "Yes" : "No", driv->overlay_color_key
							));
#endif
#ifndef SDL_TEXTINPUTEVENT_TEXT_SIZE
	SDL_EnableUNICODE(1);
#else
	SDL_StartTextInput();
#endif /* SDL_TEXTINPUTEVENT_TEXT_SIZE */
	return driv;
}

void SDL_DeleteVideo(void *ifce)
{
	GF_VideoOutput *dr = (GF_VideoOutput *)ifce;
	SDLVID();
#ifdef	SDL_WINDOW_THREAD
	gf_th_del(ctx->sdl_th);
#endif
	gf_mx_del(ctx->evt_mx);
	gf_free(ctx);
	gf_free(dr);
}

