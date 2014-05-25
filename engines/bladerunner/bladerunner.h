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

#ifndef BLADERUNNER_BLADERUNNER_H
#define BLADERUNNER_BLADERUNNER_H

#include "bladerunner/archive.h"

#include "common/array.h"
#include "common/stream.h"
#include "common/types.h"

#include "engines/engine.h"

#include "graphics/surface.h"

namespace BladeRunner {

const Graphics::PixelFormat RGB555(2, 5, 5, 5, 0, 10, 5, 0, 0);

class Chapters;
class Scene;
class Script;
class Settings;
class GameInfo;

class BladeRunnerEngine : public Engine {
public:
	bool      _gameIsRunning;
	bool      _windowIsActive;

	Chapters *_chapters;
	GameInfo *_gameInfo;
	Scene    *_scene;
	Script   *_script;
	Settings *_settings;

	int in_script_counter;

	Graphics::Surface _surface1;

private:
	static const int kArchiveCount = 10;
	MIXArchive _archives[kArchiveCount];

public:
	BladeRunnerEngine(OSystem *syst);

	bool hasFeature(EngineFeature f) const;

	Common::Error BladeRunnerEngine::run();

	bool startup();
	void initChapterAndScene();
	void shutdown();

	void loadSplash();
	bool init2();

	void gameLoop();
	void gameTick();
	void handleEvents();

	void outtakePlay(int id, bool no_localization, int container = -1);

	bool openArchive(const Common::String &name);
	bool closeArchive(const Common::String &name);
	bool isArchiveOpen(const Common::String &name);

	Common::SeekableReadStream *getResourceStream(const Common::String &name);

	void ISez(const char *str);
};

} // End of namespace BladeRunner

#endif
