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

#include "bladerunner/combat.h"

#include "bladerunner/actor.h"
#include "bladerunner/bladerunner.h"
#include "bladerunner/game_constants.h"
#include "bladerunner/savefile.h"
#include "bladerunner/settings.h"

namespace BladeRunner {

Combat::Combat(BladeRunnerEngine *vm) {
	_vm = vm;

	reset();
}

Combat::~Combat() {
}

void Combat::reset() {
	_active = false;
	_enabled = true;

	_ammoDamage[0] = 10;
	_ammoDamage[1] = 20;
	_ammoDamage[2] = 30;

	for (int i = 0; i < kSoundCount; i++) {
		_hitSoundId[i] = -1;
		_missSoundId[i] = -1;
	}
}

void Combat::activate() {
	if(_enabled) {
		_vm->_playerActor->combatModeOn(-1, -1, -1, -1, kAnimationModeCombatIdle, kAnimationModeCombatWalk, kAnimationModeCombatRun, -1, -1, -1, _vm->_combat->_ammoDamage[_vm->_settings->getAmmoType()], 0, 0);
		_active = true;
	}
}

void Combat::deactivate() {
	if (_enabled) {
		_vm->_playerActor->combatModeOff();
		_active = false;
	}
}

void Combat::change() {
	if (!_vm->_playerActor->inWalkLoop() && _enabled) {
		if (_active) {
			deactivate();
		} else {
			activate();
		}
	}
}

bool Combat::isActive() const{
	return _active;
}

void Combat::enable() {
	_enabled = true;
}

void Combat::disable() {
	_enabled = false;
}

void Combat::setHitSound(int ammoType, int column, int soundId) {
	_hitSoundId[ammoType * 3 + column] = soundId;
}

void Combat::setMissSound(int ammoType, int column, int soundId) {
	_missSoundId[ammoType * 3 + column] = soundId;
}

int Combat::getHitSound() {
	return _hitSoundId[3 * _vm->_settings->getAmmoType() + _vm->_rnd.getRandomNumber(2)];
}

int Combat::getMissSound() {
	return _hitSoundId[3 * _vm->_settings->getAmmoType() + _vm->_rnd.getRandomNumber(2)];
}

void Combat::shoot(int actorId, Vector3 &to, int screenX) {

}

void Combat::save(SaveFile &f) {
	f.write(_active);
	f.write(_enabled);
	for (int i = 0; i != 9; ++i) {
		f.write(_hitSoundId[i]);
	}
	for (int i = 0; i != 9; ++i) {
		f.write(_missSoundId[i]);
	}
}

} // End of namespace BladeRunner
