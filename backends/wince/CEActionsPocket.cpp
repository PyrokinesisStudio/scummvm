/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001-2004 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */


#include "stdafx.h"
#include "CEActionsPocket.h"
#include "EventsBuffer.h"

#include "gui/message.h"

#include "scumm/scumm.h"

#include "common/config-manager.h"

const String pocketActionNames[] = { 
	"Pause", 
	"Save", 
	"Quit",
	"Skip",
	"Hide",
	"Keyboard",
	"Sound",
	"Right click",
	"Cursor",
	"Free look",
	"Zoom up",
	"Zoom down"
};

void CEActionsPocket::init(GameDetector &detector) {
	_instance = new CEActionsPocket(detector);
}


String CEActionsPocket::actionName(ActionType action) {
	return pocketActionNames[action];
}

int CEActionsPocket::size() {
	return POCKET_ACTION_LAST;
}

String CEActionsPocket::domain() {
	return "pocketpc";
}

int CEActionsPocket::version() {
	return POCKET_ACTION_VERSION;
}

CEActionsPocket::CEActionsPocket(GameDetector &detector) :
	CEActions(detector)
{
	int i;

	_right_click_needed = false;
	_hide_toolbar_needed = false;
	_zoom_needed = false;

	for (i=0; i<POCKET_ACTION_LAST; i++) {
		_action_mapping[i] = 0;
		_action_enabled[i] = false;
	}

}

void CEActionsPocket::initInstance(OSystem_WINCE3 *mainSystem) {
	bool is_simon = (strncmp(_detector->_targetName.c_str(), "simon", 5) == 0);
	bool is_sword1 = (_detector->_targetName == "sword1");
	bool is_sword2 = (strcmp(_detector->_targetName.c_str(), "sword2") == 0);
	bool is_queen = (_detector->_targetName == "queen");
	bool is_sky = (_detector->_targetName == "sky");
	bool is_comi = (strncmp(_detector->_targetName.c_str(), "comi", 4) == 0);

	CEActions::initInstance(mainSystem);

	// See if a right click mapping could be needed
	if (is_sword1 || is_sword2 || is_sky || is_queen || is_comi ||
		_detector->_targetName == "samnmax")
		_right_click_needed = true;

	// See if a "hide toolbar" mapping could be needed
	if (is_sword1 || is_sword2 || is_comi)
		_hide_toolbar_needed = true;

	// Initialize keys for different actions
	// Pause
	_key_action[POCKET_ACTION_PAUSE].setAscii(VK_SPACE);
	_action_enabled[POCKET_ACTION_PAUSE] = true;
	// Save
	if (is_simon || is_sword2) 
		_action_enabled[POCKET_ACTION_SAVE] = false;
	else
	if (is_queen) {
		_action_enabled[POCKET_ACTION_SAVE] = true;
		_key_action[POCKET_ACTION_SAVE].setAscii(282); // F1 key
	}
	else
	if (is_sky) {
		_action_enabled[POCKET_ACTION_SAVE] = true;
		_key_action[POCKET_ACTION_SAVE].setAscii(63); 
	}
	else {
		_action_enabled[POCKET_ACTION_SAVE] = true;
		_key_action[POCKET_ACTION_SAVE].setAscii(319); // F5 key
	}
	// Quit
	_action_enabled[POCKET_ACTION_QUIT] = true;
	// Skip
	_action_enabled[POCKET_ACTION_SKIP] = true;
	if (is_simon || is_sky || is_sword2 || is_queen || is_sword1)
		_key_action[POCKET_ACTION_SKIP].setAscii(VK_ESCAPE);
	else
		_key_action[POCKET_ACTION_SKIP].setAscii(Scumm::KEY_ALL_SKIP);
	// Hide
	_action_enabled[POCKET_ACTION_HIDE] = true;
	// Keyboard 
	_action_enabled[POCKET_ACTION_KEYBOARD] = true;
	// Sound
	_action_enabled[POCKET_ACTION_SOUND] = true;
	// RightClick
	_action_enabled[POCKET_ACTION_RIGHTCLICK] = true;
	// Cursor
	_action_enabled[POCKET_ACTION_CURSOR] = true;
	// Freelook
	_action_enabled[POCKET_ACTION_FREELOOK] = true;
	// Zoom
	if (is_sword1 || is_sword2 || is_comi) {
		_zoom_needed = true;
		_action_enabled[POCKET_ACTION_ZOOM_UP] = true;
		_action_enabled[POCKET_ACTION_ZOOM_DOWN] = true;
	}
}


CEActionsPocket::~CEActionsPocket() {
}

bool CEActionsPocket::perform(ActionType action, bool pushed) {
	if (!pushed)
		return false;

	switch (action) {
		case POCKET_ACTION_PAUSE:
		case POCKET_ACTION_SAVE:
		case POCKET_ACTION_SKIP:
			EventsBuffer::simulateKey(&_key_action[action]);
			return true;
		case POCKET_ACTION_KEYBOARD:
			_mainSystem->swap_panel();
			return true;
		case POCKET_ACTION_HIDE:
			_mainSystem->swap_panel_visibility();
			return true;
		case POCKET_ACTION_SOUND:
			_mainSystem->swap_sound_master();
			return true;
		case POCKET_ACTION_RIGHTCLICK:
			_mainSystem->add_right_click();
			return true;
		case POCKET_ACTION_CURSOR:
			_mainSystem->swap_mouse_visibility();
			return true;
		case POCKET_ACTION_ZOOM_UP:
			_mainSystem->swap_zoom_up();
			return true;
		case POCKET_ACTION_ZOOM_DOWN:
			_mainSystem->swap_zoom_down();
			return true;
		case POCKET_ACTION_QUIT:
			GUI::MessageDialog alert("Do you want to quit ?", "Yes", "No");
			if (alert.runModal() == 1)
				_mainSystem->quit();
			return true;
	}
	return false;
}

bool CEActionsPocket::needsRightClickMapping() {
	if (!_right_click_needed)
		return false;
	else
		return (_action_mapping[POCKET_ACTION_RIGHTCLICK] == 0);
}

bool CEActionsPocket::needsHideToolbarMapping() {
	if (!_hide_toolbar_needed)
		return false;
	else
		return (_action_mapping[POCKET_ACTION_HIDE] == 0);
}


bool CEActionsPocket::needsZoomMapping() {
	if (!_zoom_needed)
		return false;
	else
		return (_action_mapping[POCKET_ACTION_ZOOM_UP] == 0 || _action_mapping[POCKET_ACTION_ZOOM_DOWN] == 0);
}

