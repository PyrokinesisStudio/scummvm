/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TITANIC_MOVIE_H
#define TITANIC_MOVIE_H

#include "titanic/core/list.h"
#include "titanic/core/resource_key.h"
#include "titanic/video_surface.h"

namespace Titanic {

class CMovie : public ListItem {
public:
	virtual void proc8() = 0;
	virtual void proc9() = 0;
	virtual void proc10() = 0;
	virtual void proc11() = 0;
	virtual void proc12() = 0;
	virtual void proc13() = 0;
	virtual void proc14() = 0;
	virtual void proc15() = 0;
	virtual void proc16() = 0;
	virtual void proc17() = 0;
	virtual void proc18() = 0;
	virtual void proc19() = 0;
	virtual void proc20() = 0;
	virtual void *proc21() = 0;
};

class OSMovie : public CMovie {
public:
	OSMovie(const CResourceKey &name, CVideoSurface *surface);

	virtual void proc8();
	virtual void proc9();
	virtual void proc10();
	virtual void proc11();
	virtual void proc12();
	virtual void proc13();
	virtual void proc14();
	virtual void proc15();
	virtual void proc16();
	virtual void proc17();
	virtual void proc18();
	virtual void proc19();
	virtual void proc20();
	virtual void *proc21();

	/**
	 * Set the current frame number
	 */
	void setFrame(uint frameNumber);
};

} // End of namespace Titanic

#endif /* TITANIC_MOVIE_H */
