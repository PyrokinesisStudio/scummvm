/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004 The ScummVM project
 *
 * The ReInherit Engine is (C)2000-2003 by Daniel Balsom.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

// Game interface module private header file

#ifndef SAGA_INTERFACE_H__
#define SAGA_INTERFACE_H__

#include "saga/sprite.h"
#include "saga/script_mod.h"

namespace Saga {

typedef enum INTERFACE_UPDATE_FLAGS_tag {
	UPDATE_MOUSEMOVE = 1,
	UPDATE_MOUSECLICK
} INTERFACE_UPDATE_FLAGS;

#define R_VERB_STRLIMIT 32

#define R_STATUS_TEXT_LEN 128

#define COMMAND_DEFAULT_BUTTON 1

// Inherit the Earth interface values
#define ITE_STATUS_Y      137
#define ITE_STATUS_W      320
#define ITE_STATUS_H      12
#define ITE_STATUS_TEXT_Y 2
#define ITE_STATUS_TXTCOL 186
#define ITE_STATUS_BGCOL  15

#define ITE_CMD_TEXT_COL       147
#define ITE_CMD_TEXT_SHADOWCOL 15
#define ITE_CMD_TEXT_HILITECOL 96

#define ITE_LPORTRAIT_X 5
#define ITE_LPORTRAIT_Y 4

// IHNMAIMS interface values
#define IHNM_STATUS_Y      304
#define IHNM_STATUS_W      640
#define IHNM_STATUS_H      24
#define IHNM_STATUS_TEXT_Y 8
#define IHNM_STATUS_TXTCOL 186
#define IHNM_STATUS_BGCOL  0

#define IHNM_CMD_TEXT_COL       147
#define IHNM_CMD_TEXT_SHADOWCOL 15
#define IHNM_CMD_TEXT_HILITECOL 96

#define IHNM_LPORTRAIT_X 5
#define IHNM_LPORTRAIT_Y 4

typedef enum R_PANEL_MODES_tag {
	PANEL_COMMAND,
	PANEL_DIALOGUE
} R_PANEL_MODES;

typedef enum R_BUTTON_FLAGS_tag {
	BUTTON_NONE = 0x0,
	BUTTON_LABEL = 0x01,
	BUTTON_BITMAP = 0x02,
	BUTTON_SET = 0x04

} R_BUTTON_FLAGS;

#define BUTTON_VERB ( BUTTON_LABEL | BUTTON_BITMAP | BUTTON_SET )

struct R_INTERFACE_BUTTON {
	int x1;
	int y1;
	int x2;
	int y2;
	const char *label;
	int inactive_sprite;
	int active_sprite;
	int flags;
	int data;
};

struct R_INTERFACE_PANEL {
	byte *res;
	size_t res_len;
	int x;
	int y;
	byte *img;
	size_t img_len;
	int img_w;
	int img_h;
	int set_button;
	int nbuttons;
	R_INTERFACE_BUTTON *buttons;
	R_SPRITELIST *sprites;
};

struct R_INTERFACE_DESC {
	int status_y;
	int status_w;
	int status_h;
	int status_txt_y;
	int status_txt_col;
	int status_bgcol;
	int cmd_txt_col;
	int cmd_txt_shadowcol;
	int cmd_txt_hilitecol;
	int cmd_defaultbutton;
	int lportrait_x;
	int lportrait_y;
};

struct R_INTERFACE_MODULE {
};

enum INTERFACE_VERBS {
	I_VERB_WALKTO,
	I_VERB_LOOKAT,
	I_VERB_PICKUP,
	I_VERB_TALKTO,
	I_VERB_OPEN,
	I_VERB_CLOSE,
	I_VERB_USE,
	I_VERB_GIVE
};

struct R_VERB_DATA {
	int i_verb;
	const char *verb_cvar;
	char verb_str[R_VERB_STRLIMIT];
	int s_verb;
};

class Interface {
 public:
	Interface(SagaEngine *vm);
	~Interface(void);

	int registerLang();
	int activate();
	int deactivate();
	int setStatusText(const char *new_txt);
	int draw();
	int update(R_POINT *imouse_pt, int update_flag);


 private:
	int hitTest(R_POINT *imouse_pt, int *ibutton);
	int drawStatusBar(R_SURFACE *ds);
	int handleCommandUpdate(R_SURFACE *ds, R_POINT *imouse_pt);
	int handleCommandClick(R_SURFACE *ds, R_POINT *imouse_pt);
	int handlePlayfieldUpdate(R_SURFACE *ds, R_POINT *imouse_pt);
	int handlePlayfieldClick(R_SURFACE *ds, R_POINT *imouse_pt);

 private:
	SagaEngine *_vm;

	bool _initialized;
	int _active;
	R_RSCFILE_CONTEXT *_interfaceContext;
	R_INTERFACE_DESC _iDesc;
	R_PANEL_MODES _panelMode;
	R_INTERFACE_PANEL _cPanel;
	R_INTERFACE_PANEL _dPanel;
	char _statusText[R_STATUS_TEXT_LEN];
	int _activePortrait;
	R_SPRITELIST *_defPortraits;
	int _activeVerb;
	R_SCRIPT_THREAD *_iThread;
};

} // End of namespace Saga

#endif				/* R_INTERFACE_H__ */
/* end "r_interface.h" */
