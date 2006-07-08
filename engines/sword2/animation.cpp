/* Copyright (C) 1994-1998 Revolution Software Ltd.
 * Copyright (C) 2003-2006 The ScummVM project
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
 * $URL$
 * $Id$
 */

#include "common/stdafx.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/system.h"

#include "sound/vorbis.h"
#include "sound/mp3.h"

#include "sword2/sword2.h"
#include "sword2/defs.h"
#include "sword2/header.h"
#include "sword2/maketext.h"
#include "sword2/mouse.h"
#include "sword2/resman.h"
#include "sword2/screen.h"
#include "sword2/sound.h"
#include "sword2/animation.h"

namespace Sword2 {

// TODO: The interaction between the basic cutscene player class and the
//       specific plyers is sometimes a bit awkward, since our classes for
//       DXA and MPEG decoding are so fundamentally different. The DXA decoder
//       is just a decoder, while the MPEG decoder has delusions of being a
//       player. This could probably be simplified quite a bit.

///////////////////////////////////////////////////////////////////////////////
// Basic movie player
///////////////////////////////////////////////////////////////////////////////

MovieInfo MoviePlayer::_movies[] = {
	{ "carib",    222, false },
	{ "escape",   187, false },
	{ "eye",      248, false },
	{ "finale",  1485, false },
	{ "guard",     75, false },
	{ "intro",   1800, false },
	{ "jungle",   186, false },
	{ "museum",   167, false },
	{ "pablo",     75, false },
	{ "pyramid",   60, false },
	{ "quaram",   184, false },
	{ "river",    656, false },
	{ "sailing",  138, false },
	{ "shaman",   788, true  },
	{ "stone1",    34, true  },
	{ "stone2",   282, false },
	{ "stone3",    65, true  },
	{ "demo",      60, false },
	{ "enddemo",  110, false }
};

MoviePlayer::MoviePlayer(Sword2Engine *vm) {
	_vm = vm;
	_mixer = _vm->_mixer;
	_system = _vm->_system;
	_textSurface = NULL;
	_bgSoundStream = NULL;
	_ticks = 0;
	_currentFrame = 0;
	_frameBuffer = NULL;
	_frameWidth = 0;
	_frameHeight = 0;
	_frameX = 0;
	_frameY = 0;
	_black = 1;
	_white = 255;
	_numFrames = 0;
	_leadOutFrame = (uint)-1;
	_seamless = false;
	_framesSkipped = 0;
	_forceFrame = false;
	_textList = NULL;
	_currentText = 0;
}

MoviePlayer::~MoviePlayer() {
}

void MoviePlayer::updatePalette(byte *pal, bool packed) {
	byte palette[4 * 256];
	byte *p = palette;

	uint32 maxWeight = 0;
	uint32 minWeight = 0xFFFFFFFF;

	for (int i = 0; i < 256; i++) {
		int r = *pal++;
		int g = *pal++;
		int b = *pal++;

		if (!packed)
			pal++;

		uint32 weight = 3 * r * r + 6 * g * g + 2 * b * b;

		if (weight >= maxWeight) {
			_black = i;
			maxWeight = weight;
		}

		if (weight <= minWeight) {
			_white = i;
			minWeight = i;
		}

		*p++ = r;
		*p++ = g;
		*p++ = b;
		*p++ = 0;
	}

	_vm->_screen->setPalette(0, 256, palette, RDPAL_INSTANT);
	_forceFrame = true;
}

void MoviePlayer::savePalette() {
	memcpy(_originalPalette, _vm->_screen->getPalette(), sizeof(_originalPalette));
}

void MoviePlayer::restorePalette() {
	_vm->_screen->setPalette(0, 256, _originalPalette, RDPAL_INSTANT);
}

void MoviePlayer::clearScreen() {
	_vm->_screen->clearScene();
	_system->copyRectToScreen(_vm->_screen->getScreen(), _vm->_screen->getScreenWide(), 0, 0, _vm->_screen->getScreenWide(), _vm->_screen->getScreenDeep());
}

void MoviePlayer::updateScreen() {
	_system->updateScreen();
}

bool MoviePlayer::checkSkipFrame() {
	if (_forceFrame) {
		_forceFrame = false;
		return false;
	}

	if (_framesSkipped > 10) {
		warning("Forced frame %d to be displayed", _currentFrame);
		_framesSkipped = 0;
		return false;
	}

	if (_bgSoundStream) {
		if ((_mixer->getSoundElapsedTime(_bgSoundHandle) * 12) / 1000 < _currentFrame + 1)
			return false;
	} else {
		if (_system->getMillis() <= _ticks)
			return false;
	}

	_framesSkipped++;
	return true;
}

void MoviePlayer::waitForFrame() {
	if (_bgSoundStream) {
		while (_mixer->isSoundHandleActive(_bgSoundHandle) && (_mixer->getSoundElapsedTime(_bgSoundHandle) * 12) / 1000 < _currentFrame) {
			_system->delayMillis(10);
		}

		// In case the background sound ends prematurely, update _ticks
		// so that we can still fall back on the no-sound sync case for
		// the subsequent frames.

		_ticks = _system->getMillis();
	} else {
		while (_system->getMillis() < _ticks) {
			_system->delayMillis(10);
		}
	}
}

void MoviePlayer::drawFrame() {
	_ticks += 83;

	if (checkSkipFrame()) {
		warning("Skipped frame %d", _currentFrame);
		return;
	}

	waitForFrame();

	int screenWidth = _vm->_screen->getScreenWide();

	_system->copyRectToScreen(_frameBuffer + _frameY * screenWidth + _frameX, screenWidth, _frameX, _frameY, _frameWidth, _frameHeight);
	_vm->_screen->setNeedFullRedraw();
}

void MoviePlayer::openTextObject(MovieTextObject *t) {
	if (t->textSprite) {
		_vm->_screen->createSurface(t->textSprite, &_textSurface);
	}
}

void MoviePlayer::closeTextObject(MovieTextObject *t) {
	if (_textSurface) {
		_vm->_screen->deleteSurface(_textSurface);
		_textSurface = NULL;
	}
}

void MoviePlayer::drawTextObject(MovieTextObject *t) {
	if (t->textSprite && _textSurface) {
		int screenWidth = _vm->_screen->getScreenWide();
		byte *src = t->textSprite->data;
		byte *dst = _frameBuffer + (_frameY + _frameHeight - t->textSprite->h - 20) * screenWidth + _frameX + (_frameWidth - t->textSprite->w) / 2;

		for (int y = 0; y < t->textSprite->h; y++) {
			for (int x = 0; x < t->textSprite->w; x++) {
				if (src[x] == 1)
					dst[x] = _white;
				else if (src[x] == 255)
					dst[x] = _black;
			}
			src += t->textSprite->w;
			dst += screenWidth;
		}
	}
}

void MoviePlayer::undrawTextObject(MovieTextObject *t) {
}

bool MoviePlayer::load(const char *name, MovieTextObject *text[]) {
	_bgSoundStream = NULL;
	_textList = text;
	_currentText = 0;
	_currentFrame = 0;

	for (int i = 0; i < ARRAYSIZE(_movies); i++) {
		if (scumm_stricmp(name, _movies[i].name) == 0) {
			_seamless = _movies[i].seamless;
			_numFrames = _movies[i].frames;
			if (_numFrames > 60)
				_leadOutFrame = _numFrames - 60;
			return true;
		}
	}

	return false;
}

void MoviePlayer::play(int32 leadIn, int32 leadOut) {
	bool terminate = false;
	bool textVisible = false;
	bool startNextText = false;
	byte *data;
	uint32 len;
	Audio::SoundHandle leadInHandle, leadOutHandle;
	uint32 flags = Audio::Mixer::FLAG_16BITS;

	// This happens if the user quits during the "eye" cutscene.
	if (_vm->_quit)
		return;

	if (leadIn) {
		data = _vm->_resman->openResource(leadIn);
		len = _vm->_resman->fetchLen(leadIn) - ResHeader::size();

		assert(_vm->_resman->fetchType(data) == WAV_FILE);

		data += ResHeader::size();

		_vm->_sound->playFx(&leadInHandle, data, len, Audio::Mixer::kMaxChannelVolume, 0, false, Audio::Mixer::kMusicSoundType);
	}

	savePalette();

	// Not all cutscenes cover the entire screen, so clear it. We will
	// always clear the game screen, no matter how the cutscene is to be
	// displayed.

	_vm->_mouse->closeMenuImmediately();

	if (!_seamless) {
		_vm->_screen->clearScene();
	}

	_vm->_screen->updateDisplay();

#ifndef SCUMM_BIG_ENDIAN
	flags |= Audio::Mixer::FLAG_LITTLE_ENDIAN;
#endif

	_framesSkipped = 0;

	_ticks = _system->getMillis();

	if (_bgSoundStream) {
		_mixer->playInputStream(Audio::Mixer::kSFXSoundType, &_bgSoundHandle, _bgSoundStream);
	}

	while (!terminate && _currentFrame < _numFrames && decodeFrame()) {
		_currentFrame++;

		// The frame has been decoded. Now draw the subtitles, if any,
		// before drawing it to the screen.

		if (_textList && _textList[_currentText]) {
			MovieTextObject *t = _textList[_currentText];

			if (_currentFrame == t->startFrame) {
				openTextObject(t);
				textVisible = true;

				if (t->speech) {
					startNextText = true;
				}
			}

			if (startNextText && !_mixer->isSoundHandleActive(_speechHandle)) {
				_mixer->playRaw(&_speechHandle, t->speech, t->speechBufferSize, 22050, flags);
				startNextText = false;
			}

			if (_currentFrame == t->endFrame) {
				undrawTextObject(t);
				closeTextObject(t);
				_currentText++;
				textVisible = false;
			}

			if (textVisible)
				drawTextObject(t);
		}

		if (leadOut && _currentFrame == _leadOutFrame) {
			data = _vm->_resman->openResource(leadOut);
			len = _vm->_resman->fetchLen(leadOut) - ResHeader::size();

			assert(_vm->_resman->fetchType(data) == WAV_FILE);

			data += ResHeader::size();

			_vm->_sound->playFx(&leadOutHandle, data, len, Audio::Mixer::kMaxChannelVolume, 0, false, Audio::Mixer::kMusicSoundType);
		}

		drawFrame();
		updateScreen();

		OSystem::Event event;

		while (_system->pollEvent(event)) {
			switch (event.type) {
			case OSystem::EVENT_SCREEN_CHANGED:
				handleScreenChanged();
				break;
			case OSystem::EVENT_QUIT:
				_vm->closeGame();
				terminate = true;
				break;
			case OSystem::EVENT_KEYDOWN:
				if (event.kbd.keycode == 27)
					terminate = true;
				break;
			default:
				break;
			}
		}
	}

	if (!_seamless) {
		// Most cutscenes fade to black on their own, but not all of
		// them. I think it looks better if they do.

		clearScreen();

		// If the sound is still playing, draw the subtitles one final
		// time. This happens in the "carib" cutscene.

		if (textVisible && _mixer->isSoundHandleActive(_speechHandle)) {
			drawTextObject(_textList[_currentText]);
		}

		updateScreen();
	}

	if (!terminate) {
		// Wait for the voice to stop playing. This is to make sure
		// that we don't cut off the speech in mid-sentence, and - even
		// more importantly - that we don't free the sound buffer while
		// it's still in use.

		while (_mixer->isSoundHandleActive(_speechHandle)) {
			_system->delayMillis(100);
		}

		while (_mixer->isSoundHandleActive(_bgSoundHandle)) {
			_system->delayMillis(100);
		}
	} else {
		_mixer->stopHandle(_speechHandle);
		_mixer->stopHandle(_bgSoundHandle);
	}

	if (!_seamless) {
		clearScreen();
		updateScreen();
	}

	// Setting the palette implies a full redraw.
	restorePalette();
}

#ifdef USE_ZLIB

///////////////////////////////////////////////////////////////////////////////
// Movie player for the new DXA movies
///////////////////////////////////////////////////////////////////////////////

MoviePlayerDXA::MoviePlayerDXA(Sword2Engine *vm) : MoviePlayer(vm) {
	debug(0, "Creating DXA cutscene player");
}

MoviePlayerDXA::~MoviePlayerDXA() {
	closeFile();
}

void MoviePlayerDXA::setPalette(byte *pal) {
	updatePalette(pal);
}

bool MoviePlayerDXA::decodeFrame() {
	decodeNextFrame();
	copyFrameToBuffer(_frameBuffer, _frameX, _frameY, _vm->_screen->getScreenWide());
	return true;
}

bool MoviePlayerDXA::load(const char *name, MovieTextObject *text[]) {
	if (!MoviePlayer::load(name, text))
		return false;

	char filename[20];

	snprintf(filename, sizeof(filename), "%s.dxa", name);

	if (loadFile(filename)) {
		// The Broken Sword games always use external audio tracks.
		if (_fd.readUint32BE() != MKID_BE('NULL'))
			return false;

		_frameBuffer = _vm->_screen->getScreen();

		_frameWidth = getWidth();
		_frameHeight = getHeight();

		_frameX = (_vm->_screen->getScreenWide() - _frameWidth) / 2;
		_frameY = (_vm->_screen->getScreenDeep() - _frameHeight) / 2;

		_bgSoundStream = Audio::AudioStream::openStreamFile(name);

		return true;
	}

	return false;
}

#endif

///////////////////////////////////////////////////////////////////////////////
// Movie player for the old MPEG movies
///////////////////////////////////////////////////////////////////////////////

MoviePlayerMPEG::MoviePlayerMPEG(Sword2Engine *vm) : MoviePlayer(vm) {
#ifdef BACKEND_8BIT
	debug(0, "Creating MPEG cutscene player (8-bit)");
#else
	debug(0, "Creating MPEG cutscene player (16-bit)");
#endif
}

MoviePlayerMPEG::~MoviePlayerMPEG() {
	delete _anim;
	_anim = NULL;
}

bool MoviePlayerMPEG::load(const char *name, MovieTextObject *text[]) {
	if (!MoviePlayer::load(name, text))
		return false;

	_anim = new AnimationState(_vm, this);

	if (!_anim->init(name)) {
		delete _anim;
		_anim = NULL;
		return false;
	}

#ifdef BACKEND_8BIT
	_frameBuffer = _vm->_screen->getScreen();
#endif

	return true;
}

bool MoviePlayerMPEG::checkSkipFrame() {
	return false;
}

void MoviePlayerMPEG::waitForFrame() {
}

bool MoviePlayerMPEG::decodeFrame() {
	bool result = _anim->decodeFrame();

#ifdef BACKEND_8BIT
	_frameWidth = _anim->getFrameWidth();
	_frameHeight = _anim->getFrameHeight();

	_frameX = (_vm->_screen->getScreenWide() - _frameWidth) / 2;
	_frameY = (_vm->_screen->getScreenDeep() - _frameHeight) / 2;
#endif

	return result;
}

#ifndef BACKEND_8BIT
void MoviePlayerMPEG::handleScreenChanged() {
	_anim->handleScreenChanged();
}

void MoviePlayerMPEG::clearScreen() {
	_anim->clearScreen();
}

void MoviePlayerMPEG::drawFrame() {
}

void MoviePlayerMPEG::updateScreen() {
	_anim->updateScreen();
}

void MoviePlayerMPEG::drawTextObject(MovieTextObject *t) {
	if (t->textSprite && _textSurface) {
		_anim->drawTextObject(t->textSprite, _textSurface);
	}
}
#endif

AnimationState::AnimationState(Sword2Engine *vm, MoviePlayer *player)
	: BaseAnimationState(vm->_mixer, vm->_system, 640, 480) {
	_vm = vm;
	_player = player;
}

AnimationState::~AnimationState() {
}

#ifdef BACKEND_8BIT
void AnimationState::setPalette(byte *pal) {
	_player->updatePalette(pal, false);
}
#else

void AnimationState::drawTextObject(SpriteInfo *s, byte *src) {
	int moviePitch = _movieScale * _movieWidth;
	int textX = _movieScale * s->x;
	int textY = _movieScale * (_frameHeight - s->h - 12);

	OverlayColor *dst = _overlay + textY * moviePitch + textX;

	OverlayColor pen = _sys->RGBToColor(255, 255, 255);
	OverlayColor border = _sys->RGBToColor(0, 0, 0);

	// TODO: Use the AdvMame scalers for the text? Pre-scale it?

	for (int y = 0; y < s->h; y++) {
		OverlayColor *ptr = dst;

		for (int x = 0; x < s->w; x++) {
			switch (src[x]) {
			case 1:
				*ptr++ = border;
				if (_movieScale > 1) {
					*ptr++ = border;
					if (_movieScale > 2)
						*ptr++ = border;
				}
				break;
			case 255:
				*ptr++ = pen;
				if (_movieScale > 1) {
					*ptr++ = pen;
					if (_movieScale > 2)
						*ptr++ = pen;
				}
				break;
			default:
				ptr += _movieScale;
				break;
			}
		}

		if (_movieScale > 1) {
			memcpy(dst + moviePitch, dst, _movieScale * s->w * sizeof(OverlayColor));
			if (_movieScale > 2)
				memcpy(dst + 2 * moviePitch, dst, _movieScale * s->w * sizeof(OverlayColor));
		}

		dst += _movieScale * moviePitch;
		src += s->w;
	}
}

#endif

void AnimationState::clearScreen() {
#ifdef BACKEND_8BIT
	memset(_vm->_screen->getScreen(), 0, _movieWidth * _movieHeight);
#else
	OverlayColor black = _sys->RGBToColor(0, 0, 0);

	for (int i = 0; i < _movieScale * _movieWidth * _movieScale * _movieHeight; i++)
		_overlay[i] = black;
#endif
}

void AnimationState::drawYUV(int width, int height, byte *const *dat) {
	_frameWidth = width;
	_frameHeight = height;

#ifdef BACKEND_8BIT
	byte *buf = _vm->_screen->getScreen() + ((480 - height) / 2) * RENDERWIDE + (640 - width) / 2;

	int x, y;

	int ypos = 0;
	int cpos = 0;
	int linepos = 0;

	for (y = 0; y < height; y += 2) {
		for (x = 0; x < width; x += 2) {
			int i = ((((dat[2][cpos] + ROUNDADD) >> SHIFT) * (BITDEPTH + 1)) + ((dat[1][cpos] + ROUNDADD) >> SHIFT)) * (BITDEPTH + 1);
			cpos++;

			buf[linepos               ] = _lut[i + ((dat[0][        ypos  ] + ROUNDADD) >> SHIFT)];
			buf[RENDERWIDE + linepos++] = _lut[i + ((dat[0][width + ypos++] + ROUNDADD) >> SHIFT)];
			buf[linepos               ] = _lut[i + ((dat[0][        ypos  ] + ROUNDADD) >> SHIFT)];
			buf[RENDERWIDE + linepos++] = _lut[i + ((dat[0][width + ypos++] + ROUNDADD) >> SHIFT)];
		}
		linepos += (2 * RENDERWIDE - width);
		ypos += width;
	}
#else
	plotYUV(width, height, dat);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Dummy player for subtitled speech only
///////////////////////////////////////////////////////////////////////////////

MoviePlayerDummy::MoviePlayerDummy(Sword2Engine *vm) : MoviePlayer(vm) {
	debug(0, "Creating Dummy cutscene player");
}

MoviePlayerDummy::~MoviePlayerDummy() {
}

bool MoviePlayerDummy::load(const char *name, MovieTextObject *text[]) {
	if (!MoviePlayer::load(name, text))
		return false;

	_frameBuffer = _vm->_screen->getScreen();

	_frameWidth = 640;
	_frameHeight = 400;
	_frameX = 0;
	_frameY = 40;

	return true;
}

bool MoviePlayerDummy::decodeFrame() {
	if (_currentFrame == 0 && _textList) {
		byte dummyPalette[] = {
			  0,   0,   0, 0,
			255, 255, 255, 0,
		};

		// 0 is always black
		// 1 is the border colour - black
		// 255 is the pen colour - white

		_system->setPalette(dummyPalette, 0, 1);
		_system->setPalette(dummyPalette, 1, 1);
		_system->setPalette(dummyPalette + 4, 255, 1);

		byte msgNoCutscenesRU[] = "Po\344uk - to\344\345ko pev\345: hagmute k\344abuwy Ucke\343n, u\344u nocetute ca\343t npoekta u ckava\343te budeo po\344uku";

#if defined(USE_MPEG2) || defined(USE_ZLIB)
		byte msgNoCutscenes[] = "Cutscene - Narration Only: Press ESC to exit, or visit www.scummvm.org to download cutscene videos";
#else
		byte msgNoCutscenes[] = "Cutscene - Narration Only: Press ESC to exit, or recompile ScummVM with MPEG2 or ZLib support");
#endif

		byte *msg;

		// Russian version substituted latin characters with Cyrillic.
		if (Common::parseLanguage(ConfMan.get("language")) == Common::RU_RUS) {
			msg = msgNoCutscenesRU;
		} else {
			msg = msgNoCutscenes;
		}

		byte *data = _vm->_fontRenderer->makeTextSprite(msg, RENDERWIDE, 255, _vm->_speechFontId);

		FrameHeader frame_head;
		SpriteInfo msgSprite;
		byte *msgSurface;

		frame_head.read(data);

		msgSprite.x = _vm->_screen->getScreenWide() / 2 - frame_head.width / 2;
		msgSprite.y = (480 - frame_head.height) / 2;
		msgSprite.w = frame_head.width;
		msgSprite.h = frame_head.height;
		msgSprite.type = RDSPR_NOCOMPRESSION;
		msgSprite.data = data + FrameHeader::size();

		_vm->_screen->createSurface(&msgSprite, &msgSurface);
		_vm->_screen->drawSurface(&msgSprite, msgSurface);
		_vm->_screen->deleteSurface(msgSurface);

		free(data);
		updateScreen();
	}

	// If we have played the final voice-over, skip ahead to the lead out

	if (_textList && !_textList[_currentText] && !_mixer->isSoundHandleActive(_speechHandle) && _leadOutFrame != (uint)-1 && _currentFrame < _leadOutFrame) {
		_currentFrame = _leadOutFrame - 1;
	}

	return true;
}

bool MoviePlayerDummy::checkSkipFrame() {
	return false;
}

void MoviePlayerDummy::waitForFrame() {
	if (!_textList || _currentFrame < _textList[0]->startFrame) {
		_ticks = _system->getMillis();
		return;
	}

	MoviePlayer::waitForFrame();
}

void MoviePlayerDummy::drawFrame() {
	_ticks += 83;
	waitForFrame();
}

void MoviePlayerDummy::drawTextObject(MovieTextObject *t) {
	_vm->_screen->drawSurface(t->textSprite, _textSurface);
}

void MoviePlayerDummy::undrawTextObject(MovieTextObject *t) {
	memset(_textSurface, 1, t->textSprite->w * t->textSprite->h);
	drawTextObject(t);
}

///////////////////////////////////////////////////////////////////////////////
// Factory function for creating the appropriate cutscene player
///////////////////////////////////////////////////////////////////////////////

MoviePlayer *makeMoviePlayer(Sword2Engine *vm, const char *name) {
	char filename[20];

#ifdef USE_ZLIB
	snprintf(filename, sizeof(filename), "%s.dxa", name);

	if (Common::File::exists(filename)) {
		return new MoviePlayerDXA(vm);
	}
#endif

#ifdef USE_MPEG2
	snprintf(filename, sizeof(filename), "%s.mp2", name);

	if (Common::File::exists(filename)) {
		return new MoviePlayerMPEG(vm);
	}
#endif

	return new MoviePlayerDummy(vm);
}

} // End of namespace Sword2
