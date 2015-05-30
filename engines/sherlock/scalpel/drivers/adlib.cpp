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

#include "sherlock/sherlock.h"
#include "sherlock/scalpel/drivers/mididriver.h"

#include "common/file.h"
#include "common/system.h"
#include "common/textconsole.h"

#include "audio/fmopl.h"
#include "audio/softsynth/emumidi.h"

namespace Sherlock {

#define USE_SCI_MIDI_PLAYER 1

#define SHERLOCK_ADLIB_VOICES_COUNT 9
#define SHERLOCK_ADLIB_NOTES_COUNT 96

byte adlib_Operator1Register[SHERLOCK_ADLIB_VOICES_COUNT] = {
	0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11, 0x12
};

byte adlib_Operator2Register[SHERLOCK_ADLIB_VOICES_COUNT] = {
	0x03, 0x04, 0x05, 0x0B, 0x0C, 0x0D, 0x13, 0x14, 0x15
};

struct adlib_percussionChannelEntry {
	byte requiredNote;
	byte replacementNote;
};

// hardcoded, dumped from ADHOM.DRV
const adlib_percussionChannelEntry adlib_percussionChannelTable[SHERLOCK_ADLIB_VOICES_COUNT] = {
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
	{ 0x24, 0x0C },
	{ 0x38, 0x01 },
	{ 0x26, 0x1E }
};

struct adlib_InstrumentEntry {
	byte reg20op1;
	byte reg40op1;
	byte reg60op1;
	byte reg80op1;
	byte regE0op1;
	byte reg20op2;
	byte reg40op2;
	byte reg60op2;
	byte reg80op2;
	byte regE0op2;
	byte regC0;
	byte frequencyAdjust;
};

// hardcoded, dumped from ADHOM.DRV
const adlib_InstrumentEntry adlib_instrumentTable[] = {
	{ 0x71, 0x89, 0x51, 0x11, 0x00, 0x61, 0x23, 0x42, 0x15, 0x01, 0x02, 0xF4 },
	{ 0x22, 0x20, 0x97, 0x89, 0x00, 0xA2, 0x1F, 0x70, 0x07, 0x00, 0x0A, 0xF4 },
	{ 0x70, 0x1A, 0x64, 0x13, 0x00, 0x20, 0x1F, 0x53, 0x46, 0x00, 0x0E, 0xF4 },
	{ 0xB6, 0x4A, 0xB6, 0x32, 0x00, 0x11, 0x2B, 0xD1, 0x31, 0x00, 0x0E, 0xE8 },
	{ 0x71, 0x8B, 0x51, 0x11, 0x00, 0x61, 0x20, 0x32, 0x35, 0x01, 0x02, 0xF4 },
	{ 0x71, 0x8A, 0x51, 0x11, 0x00, 0x61, 0x20, 0x32, 0x25, 0x01, 0x02, 0xF4 },
	{ 0x23, 0x0F, 0xF4, 0x04, 0x02, 0x2F, 0x25, 0xF0, 0x43, 0x00, 0x06, 0xE8 },
	{ 0x71, 0x1C, 0x71, 0x03, 0x00, 0x21, 0x1F, 0x54, 0x17, 0x00, 0x0E, 0xF4 },
	{ 0x71, 0x8A, 0x6E, 0x17, 0x00, 0x25, 0x27, 0x6B, 0x0E, 0x00, 0x02, 0xF4 },
	{ 0x71, 0x1D, 0x81, 0x03, 0x00, 0x21, 0x1F, 0x64, 0x17, 0x00, 0x0E, 0xF4 },
	{ 0x01, 0x4B, 0xF1, 0x50, 0x00, 0x01, 0x23, 0xD2, 0x76, 0x00, 0x06, 0xF4 },
	{ 0x2F, 0xCA, 0xF8, 0xE5, 0x00, 0x21, 0x1F, 0xC0, 0xFF, 0x00, 0x00, 0xF4 },
	{ 0x29, 0xCD, 0xF0, 0x91, 0x00, 0x21, 0x1F, 0xE0, 0x86, 0x00, 0x02, 0xF4 },
	{ 0x24, 0xD0, 0xF0, 0x01, 0x00, 0x21, 0x1F, 0xE0, 0x86, 0x00, 0x02, 0xF4 },
	{ 0x23, 0xC8, 0xF0, 0x01, 0x00, 0x21, 0x1F, 0xE0, 0x86, 0x00, 0x02, 0xF4 },
	{ 0x64, 0xC9, 0xB0, 0x01, 0x00, 0x61, 0x1F, 0xF0, 0x86, 0x00, 0x02, 0xF4 },
	{ 0x33, 0x85, 0xA1, 0x10, 0x00, 0x15, 0x9F, 0x72, 0x23, 0x00, 0x08, 0xF4 },
	{ 0x31, 0x85, 0xA1, 0x10, 0x00, 0x15, 0x9F, 0x73, 0x33, 0x00, 0x08, 0xF4 },
	{ 0x31, 0x81, 0xA1, 0x30, 0x00, 0x16, 0x9F, 0xC2, 0x74, 0x00, 0x08, 0xF4 },
	{ 0x03, 0x8A, 0xF0, 0x7B, 0x00, 0x02, 0x9F, 0xF4, 0x7B, 0x00, 0x08, 0xF4 },
	{ 0x03, 0x8A, 0xF0, 0x7B, 0x00, 0x01, 0x9F, 0xF4, 0x7B, 0x00, 0x08, 0xF4 },
	{ 0x23, 0x8A, 0xF2, 0x7B, 0x00, 0x01, 0x9F, 0xF4, 0x7B, 0x00, 0x08, 0xF4 },
	{ 0x32, 0x80, 0x01, 0x10, 0x00, 0x12, 0x9F, 0x72, 0x33, 0x00, 0x08, 0xF4 },
	{ 0x32, 0x80, 0x01, 0x10, 0x00, 0x14, 0x9F, 0x73, 0x33, 0x00, 0x08, 0xF4 },
	{ 0x31, 0x16, 0x73, 0x8E, 0x00, 0x21, 0x1F, 0x80, 0x9E, 0x00, 0x0E, 0xF4 },
	{ 0x30, 0x16, 0x73, 0x7E, 0x00, 0x21, 0x1F, 0x80, 0x9E, 0x00, 0x0E, 0x00 },
	{ 0x31, 0x94, 0x33, 0x73, 0x00, 0x21, 0x1F, 0xA0, 0x97, 0x00, 0x0E, 0xF4 },
	{ 0x31, 0x94, 0xD3, 0x73, 0x00, 0x21, 0x20, 0xA0, 0x97, 0x00, 0x0E, 0xF4 },
	{ 0x31, 0x45, 0xF1, 0x53, 0x00, 0x32, 0x1F, 0xF2, 0x27, 0x00, 0x06, 0xF4 },
	{ 0x13, 0x0C, 0xF2, 0x01, 0x00, 0x15, 0x2F, 0xF2, 0xB6, 0x00, 0x08, 0xF4 },
	{ 0x11, 0x0C, 0xF2, 0x01, 0x00, 0x11, 0x1F, 0xF2, 0xB6, 0x00, 0x08, 0xF4 },
	{ 0x11, 0x0A, 0xFE, 0x04, 0x00, 0x11, 0x1F, 0xF2, 0xBD, 0x00, 0x08, 0xF4 },
	{ 0x16, 0x4D, 0xFA, 0x11, 0x00, 0xE1, 0x20, 0xF1, 0xF1, 0x00, 0x08, 0xF4 },
	{ 0x16, 0x40, 0xBA, 0x11, 0x00, 0xF1, 0x20, 0x24, 0x31, 0x00, 0x08, 0xF4 },
	{ 0x61, 0xA7, 0x72, 0x8E, 0x00, 0xE1, 0x9F, 0x50, 0x1A, 0x00, 0x02, 0xF4 },
	{ 0x18, 0x4D, 0x32, 0x13, 0x00, 0xE1, 0x20, 0x51, 0xE3, 0x00, 0x08, 0xF4 },
	{ 0x17, 0xC0, 0x12, 0x41, 0x00, 0x31, 0x9F, 0x13, 0x31, 0x00, 0x06, 0xF4 },
	{ 0x03, 0x8F, 0xF5, 0x55, 0x00, 0x21, 0x9F, 0xF3, 0x33, 0x00, 0x00, 0xF4 },
	{ 0x13, 0x4D, 0xFA, 0x11, 0x00, 0xE1, 0x20, 0xF1, 0xF1, 0x00, 0x08, 0xF4 },
	{ 0x11, 0x43, 0x20, 0x15, 0x00, 0xF1, 0x20, 0x31, 0xF8, 0x00, 0x08, 0xF4 },
	{ 0x11, 0x03, 0x82, 0x97, 0x00, 0xE4, 0x60, 0xF0, 0xF2, 0x00, 0x08, 0xF4 },
	{ 0x05, 0x40, 0xD1, 0x53, 0x00, 0x14, 0x1F, 0x51, 0x71, 0x00, 0x06, 0xF4 },
	{ 0xF1, 0x01, 0x77, 0x17, 0x00, 0x21, 0x1F, 0x81, 0x18, 0x00, 0x02, 0xF4 },
	{ 0xF1, 0x18, 0x32, 0x11, 0x00, 0xE1, 0x1F, 0xF1, 0x13, 0x00, 0x00, 0xF4 },
	{ 0x73, 0x48, 0xF1, 0x53, 0x00, 0x71, 0x1F, 0xF1, 0x06, 0x00, 0x08, 0xF4 },
	{ 0x71, 0x8D, 0x71, 0x11, 0x00, 0x61, 0x5F, 0x72, 0x15, 0x00, 0x06, 0xF4 },
	{ 0xD7, 0x4F, 0xF2, 0x61, 0x00, 0xD2, 0x1F, 0xF1, 0xB2, 0x00, 0x08, 0xF4 },
	{ 0x01, 0x11, 0xF0, 0xFF, 0x00, 0x01, 0x1F, 0xF0, 0xF8, 0x00, 0x0A, 0xF4 },
	{ 0x31, 0x8B, 0x41, 0x11, 0x00, 0x61, 0x1F, 0x22, 0x13, 0x00, 0x06, 0xF4 },
	{ 0x71, 0x1C, 0x71, 0x03, 0x00, 0x21, 0x1F, 0x64, 0x07, 0x00, 0x0E, 0xF4 },
	{ 0x31, 0x8B, 0x41, 0x11, 0x00, 0x61, 0x1F, 0x32, 0x15, 0x00, 0x02, 0xF4 },
	{ 0x71, 0x1C, 0xFD, 0x13, 0x00, 0x21, 0x1F, 0xE7, 0xD6, 0x00, 0x0E, 0xF4 },
	{ 0x71, 0x1C, 0x51, 0x03, 0x00, 0x21, 0x1F, 0x54, 0x67, 0x00, 0x0E, 0xF4 },
	{ 0x71, 0x1C, 0x51, 0x03, 0x00, 0x21, 0x1F, 0x54, 0x17, 0x00, 0x0E, 0xF4 },
	{ 0x71, 0x1C, 0x54, 0x15, 0x00, 0x21, 0x1F, 0x53, 0x49, 0x00, 0x0E, 0xF4 },
	{ 0x71, 0x56, 0x51, 0x03, 0x00, 0x61, 0x1F, 0x54, 0x17, 0x00, 0x0E, 0xF4 },
	{ 0x71, 0x1C, 0x51, 0x03, 0x00, 0x21, 0x1F, 0x54, 0x17, 0x00, 0x0E, 0xF4 },
	{ 0x02, 0x29, 0xF5, 0x75, 0x00, 0x01, 0x9F, 0xF2, 0xF3, 0x00, 0x00, 0xF4 },
	{ 0x02, 0x29, 0xF0, 0x75, 0x00, 0x01, 0x9F, 0xF4, 0x33, 0x00, 0x00, 0xF4 },
	{ 0x01, 0x49, 0xF1, 0x53, 0x00, 0x11, 0x1F, 0xF1, 0x74, 0x00, 0x06, 0xF4 },
	{ 0x01, 0x89, 0xF1, 0x53, 0x00, 0x11, 0x1F, 0xF1, 0x74, 0x00, 0x06, 0xF4 },
	{ 0x02, 0x89, 0xF1, 0x53, 0x00, 0x11, 0x1F, 0xF1, 0x74, 0x00, 0x06, 0xF4 },
	{ 0x02, 0x80, 0xF1, 0x53, 0x00, 0x11, 0x1F, 0xF1, 0x74, 0x00, 0x06, 0xF4 },
	{ 0x01, 0x40, 0xF1, 0x53, 0x00, 0x08, 0x5F, 0xF1, 0x53, 0x00, 0x00, 0xF4 },
	{ 0x21, 0x15, 0xD3, 0x2C, 0x00, 0x21, 0x9F, 0xC3, 0x2C, 0x00, 0x0A, 0xF4 },
	{ 0x01, 0x18, 0xD4, 0xF2, 0x00, 0x21, 0x9F, 0xC4, 0x8A, 0x00, 0x0A, 0xF4 },
	{ 0x01, 0x4E, 0xF0, 0x7B, 0x00, 0x11, 0x1F, 0xF4, 0xC8, 0x00, 0x04, 0xF4 },
	{ 0x01, 0x44, 0xF0, 0xAB, 0x00, 0x11, 0x1F, 0xF3, 0xAB, 0x00, 0x04, 0xF4 },
	{ 0x53, 0x0E, 0xF4, 0xC8, 0x00, 0x11, 0x1F, 0xF1, 0xBB, 0x00, 0x04, 0xF4 },
	{ 0x53, 0x0B, 0xF2, 0xC8, 0x00, 0x11, 0x1F, 0xF2, 0xC5, 0x00, 0x04, 0xF4 },
	{ 0x21, 0x15, 0xB4, 0x4C, 0x00, 0x21, 0x1F, 0x94, 0xAC, 0x00, 0x0A, 0xF4 },
	{ 0x21, 0x15, 0x94, 0x1C, 0x00, 0x21, 0x1F, 0x64, 0xAC, 0x00, 0x0A, 0xF4 },
	{ 0x22, 0x1B, 0x97, 0x89, 0x00, 0xA2, 0x1F, 0x70, 0x07, 0x00, 0x0A, 0xF4 },
	{ 0x21, 0x19, 0x77, 0xBF, 0x00, 0xA1, 0x9F, 0x60, 0x2A, 0x00, 0x06, 0xF4 },
	{ 0xA1, 0x13, 0xD6, 0xAF, 0x00, 0xE2, 0x9F, 0x60, 0x2A, 0x00, 0x02, 0xF4 },
	{ 0xA2, 0x1D, 0x95, 0x24, 0x00, 0xE2, 0x9F, 0x60, 0x2A, 0x00, 0x02, 0xF4 },
	{ 0x32, 0x9A, 0x51, 0x19, 0x00, 0x61, 0x9F, 0x60, 0x39, 0x00, 0x0C, 0xF4 },
	{ 0xA4, 0x12, 0xF4, 0x30, 0x00, 0xE2, 0x9F, 0x60, 0x2A, 0x00, 0x02, 0xF4 },
	{ 0x21, 0x16, 0x63, 0x0E, 0x00, 0x21, 0x1F, 0x63, 0x0E, 0x00, 0x0C, 0xF4 },
	{ 0x31, 0x16, 0x63, 0x0A, 0x00, 0x21, 0x1F, 0x63, 0x0B, 0x00, 0x0C, 0xF4 },
	{ 0x21, 0x1B, 0x63, 0x0A, 0x00, 0x21, 0x1F, 0x63, 0x0B, 0x00, 0x0C, 0xF4 },
	{ 0x20, 0x1B, 0x63, 0x0A, 0x00, 0x21, 0x1F, 0x63, 0x0B, 0x00, 0x0C, 0xF4 },
	{ 0x32, 0x1C, 0x82, 0x18, 0x00, 0x61, 0x9F, 0x60, 0x07, 0x00, 0x0C, 0xF4 },
	{ 0x32, 0x18, 0x61, 0x14, 0x00, 0xE1, 0x9F, 0x72, 0x16, 0x00, 0x0C, 0xF4 },
	{ 0x31, 0xC0, 0x77, 0x17, 0x00, 0x22, 0x1F, 0x6B, 0x09, 0x00, 0x02, 0xF4 },
	{ 0x71, 0xC3, 0x8E, 0x17, 0x00, 0x22, 0x24, 0x8B, 0x0E, 0x00, 0x02, 0xF4 },
	{ 0x70, 0x8D, 0x6E, 0x17, 0x00, 0x22, 0x1F, 0x6B, 0x0E, 0x00, 0x02, 0xF4 },
	{ 0x24, 0x4F, 0xF2, 0x06, 0x00, 0x31, 0x1F, 0x52, 0x06, 0x00, 0x0E, 0xF4 },
	{ 0x31, 0x1B, 0x64, 0x07, 0x00, 0x61, 0x1F, 0xD0, 0x67, 0x00, 0x0E, 0xF4 },
	{ 0x31, 0x1B, 0x61, 0x06, 0x00, 0x61, 0x1F, 0xD2, 0x36, 0x00, 0x0C, 0xF4 },
	{ 0x31, 0x1F, 0x31, 0x06, 0x00, 0x61, 0x1F, 0x50, 0x36, 0x00, 0x0C, 0xF4 },
	{ 0x31, 0x1F, 0x41, 0x06, 0x00, 0x61, 0x1F, 0xA0, 0x36, 0x00, 0x0C, 0xF4 },
	{ 0x21, 0x9A, 0x53, 0x56, 0x00, 0x21, 0x9F, 0xA0, 0x16, 0x00, 0x0E, 0xF4 },
	{ 0x21, 0x9A, 0x53, 0x56, 0x00, 0x21, 0x9F, 0xA0, 0x16, 0x00, 0x0E, 0xF4 },
	{ 0x61, 0x19, 0x53, 0x58, 0x00, 0x21, 0x1F, 0xA0, 0x18, 0x00, 0x0C, 0xF4 },
	{ 0x61, 0x19, 0x73, 0x57, 0x00, 0x21, 0x1F, 0xA0, 0x17, 0x00, 0x0C, 0xF4 },
	{ 0x21, 0x1B, 0x71, 0xA6, 0x00, 0x21, 0x1F, 0xA1, 0x96, 0x00, 0x0E, 0xF4 },
	{ 0x85, 0x91, 0xF5, 0x44, 0x00, 0xA1, 0x1F, 0xF0, 0x45, 0x00, 0x06, 0xF4 },
	{ 0x07, 0x51, 0xF5, 0x33, 0x00, 0x61, 0x1F, 0xF0, 0x25, 0x00, 0x06, 0xF4 },
	{ 0x13, 0x8C, 0xFF, 0x21, 0x00, 0x11, 0x9F, 0xFF, 0x03, 0x00, 0x0E, 0xF4 },
	{ 0x38, 0x8C, 0xF3, 0x0D, 0x00, 0xB1, 0x5F, 0xF5, 0x33, 0x00, 0x0E, 0xF4 },
	{ 0x87, 0x91, 0xF5, 0x55, 0x00, 0x22, 0x1F, 0xF0, 0x54, 0x00, 0x06, 0xF4 },
	{ 0xB6, 0x4A, 0xB6, 0x32, 0x00, 0x11, 0x2B, 0xD1, 0x31, 0x00, 0x0E, 0xF4 },
	{ 0x04, 0x00, 0xFE, 0xF0, 0x00, 0xC2, 0x1F, 0xF6, 0xB5, 0x00, 0x0E, 0xF4 },
	{ 0x05, 0x4E, 0xDA, 0x15, 0x00, 0x01, 0x9F, 0xF0, 0x13, 0x00, 0x0A, 0xF4 },
	{ 0x31, 0x44, 0xF2, 0x9A, 0x00, 0x32, 0x1F, 0xF0, 0x27, 0x00, 0x06, 0xF4 },
	{ 0xB0, 0xC4, 0xA4, 0x02, 0x00, 0xD7, 0x9F, 0x40, 0x42, 0x00, 0x00, 0xF4 },
	{ 0xCA, 0x84, 0xF0, 0xF0, 0x00, 0xCF, 0x1F, 0x59, 0x62, 0x00, 0x0C, 0xF4 },
	{ 0x30, 0x35, 0xF5, 0xF0, 0x00, 0x35, 0x1F, 0xF0, 0x9B, 0x00, 0x02, 0xF4 },
	{ 0x63, 0x0F, 0xF4, 0x04, 0x02, 0x6F, 0x1F, 0xF0, 0x43, 0x00, 0x06, 0xF4 },
	{ 0x07, 0x40, 0x09, 0x53, 0x00, 0x05, 0x1F, 0xF6, 0x94, 0x00, 0x0E, 0xF4 },
	{ 0x09, 0x4E, 0xDA, 0x25, 0x00, 0x01, 0x1F, 0xF1, 0x15, 0x00, 0x0A, 0xF4 },
	{ 0x04, 0x00, 0xF3, 0xA0, 0x02, 0x04, 0x1F, 0xF8, 0x46, 0x00, 0x0E, 0xF4 },
	{ 0x07, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x1F, 0x5C, 0xDC, 0x00, 0x0E, 0xF4 },
	{ 0x1F, 0x1E, 0xE5, 0x5B, 0x00, 0x0F, 0x1F, 0x5D, 0xFA, 0x00, 0x0E, 0xF4 },
	{ 0x11, 0x8A, 0xF1, 0x11, 0x00, 0x01, 0x5F, 0xF1, 0xB3, 0x00, 0x06, 0xF4 },
	{ 0x00, 0x40, 0xD1, 0x53, 0x00, 0x00, 0x1F, 0xF2, 0x56, 0x00, 0x0E, 0xF4 },
	{ 0x32, 0x44, 0xF8, 0xFF, 0x00, 0x11, 0x1F, 0xF5, 0x7F, 0x00, 0x0E, 0xF4 },
	{ 0x00, 0x40, 0x09, 0x53, 0x00, 0x02, 0x1F, 0xF7, 0x94, 0x00, 0x0E, 0xF4 },
	{ 0x11, 0x86, 0xF2, 0xA8, 0x00, 0x01, 0x9F, 0xA0, 0xA8, 0x00, 0x08, 0xF4 },
	{ 0x00, 0x50, 0xF2, 0x70, 0x00, 0x13, 0x1F, 0xF2, 0x72, 0x00, 0x0E, 0xF4 },
	{ 0xF0, 0x00, 0x11, 0x11, 0x00, 0xE0, 0xDF, 0x11, 0x11, 0x00, 0x0E, 0xF4 }
};

// hardcoded, dumped from ADHOM.DRV
uint16 adlib_FrequencyLookUpTable[SHERLOCK_ADLIB_NOTES_COUNT] = {
	0x0158, 0x016C, 0x0182, 0x0199, 0x01B1, 0x01CB, 0x01E6, 0x0203, 0x0222, 0x0242,
	0x0265, 0x0289, 0x0558, 0x056C, 0x0582, 0x0599, 0x05B1, 0x05CB, 0x05E6, 0x0603,
	0x0622, 0x0642, 0x0665, 0x0689, 0x0958, 0x096C, 0x0982, 0x0999, 0x09B1, 0x09CB,
	0x09E6, 0x0A03, 0x0A22, 0x0A42, 0x0A65, 0x0A89, 0x0D58, 0x0D6C, 0x0D82, 0x0D99,
	0x0DB1, 0x0DCB, 0x0DE6, 0x0E03, 0x0E22, 0x0E42, 0x0E65, 0x0E89, 0x1158, 0x116C,
	0x1182, 0x1199, 0x11B1, 0x11CB, 0x11E6, 0x1203, 0x1222, 0x1242, 0x1265, 0x1289,
	0x1558, 0x156C, 0x1582, 0x1599, 0x15B1, 0x15CB, 0x15E6, 0x1603, 0x1622, 0x1642,
	0x1665, 0x1689, 0x1958, 0x196C, 0x1982, 0x1999, 0x19B1, 0x19CB, 0x19E6, 0x1A03,
	0x1A22, 0x1A42, 0x1A65, 0x1A89, 0x1D58, 0x1D6C, 0x1D82, 0x1D99, 0x1DB1, 0x1DCB,
	0x1DE6, 0x1E03, 0x1E22, 0x1E42, 0x1E65, 0x1E89
};

class MidiDriver_AdLib : public MidiDriver_Emulated {
public:
	MidiDriver_AdLib(Audio::Mixer *mixer)
		: MidiDriver_Emulated(mixer), _masterVolume(15), _opl(0) {
		memset(_voiceChannelMapping, 0, sizeof(_voiceChannelMapping));
	}
	virtual ~MidiDriver_AdLib() { }

	// MidiDriver
	int open();
	void close();
	void send(uint32 b);
	MidiChannel *allocateChannel() { return NULL; }
	MidiChannel *getPercussionChannel() { return NULL; }

	// AudioStream
	bool isStereo() const { return false; }
	int getRate() const { return _mixer->getOutputRate(); }
	int getPolyphony() const { return SHERLOCK_ADLIB_VOICES_COUNT; }
	bool hasRhythmChannel() const { return false; }

	// MidiDriver_Emulated
	void generateSamples(int16 *buf, int len);

	void setVolume(byte volume);
	virtual uint32 property(int prop, uint32 param);

	void newMusicData(byte *musicData, int32 musicDataSize);

private:
	struct adlib_ChannelEntry {
		bool   inUse;
		uint16 inUseTimer;
		const  adlib_InstrumentEntry *currentInstrumentPtr;
		byte   currentNote;
		byte   currentA0hReg;
		byte   currentB0hReg;

		adlib_ChannelEntry() : inUse(false), inUseTimer(0), currentInstrumentPtr(NULL), currentNote(0),
								currentA0hReg(0), currentB0hReg(0) { }
	};

	OPL::OPL *_opl;
	int _masterVolume;

	// points to a MIDI channel for each of the new voice channels
	byte _voiceChannelMapping[SHERLOCK_ADLIB_VOICES_COUNT];

	// stores information about all FM voice channels
	adlib_ChannelEntry _channels[SHERLOCK_ADLIB_VOICES_COUNT];

	void updateChannelInUseTimers();

	void resetAdLib();
	void resetAdLib_OperatorRegisters(byte baseRegister, byte value);
	void resetAdLib_FMVoiceChannelRegisters(byte baseRegister, byte value);

	void programChange(byte MIDIchannel, byte parameter);
	void setRegister(int reg, int value);
	void noteOn(byte MIDIchannel, byte note, byte velocity);
	void noteOff(byte MIDIchannel, byte note);
	void voiceOnOff(byte FMVoiceChannel, bool KeyOn, byte note, byte velocity);

	void pitchBendChange(byte MIDIchannel, byte parameter1, byte parameter2);
};

#if USE_SCI_MIDI_PLAYER
class MidiPlayer_AdLib : public MidiPlayer {
public:
	MidiPlayer_AdLib() : MidiPlayer() { _driver = new MidiDriver_AdLib(g_system->getMixer()); }
	~MidiPlayer_AdLib() {
		delete _driver;
		_driver = 0;
	}

	int open();
	void close();

	byte getPlayId() const;
	int getPolyphony() const { return SHERLOCK_ADLIB_VOICES_COUNT; }
	bool hasRhythmChannel() const { return false; }
	void setVolume(byte volume) { static_cast<MidiDriver_AdLib *>(_driver)->setVolume(volume); }

	//int getLastChannel() const { return (static_cast<const MidiDriver_AdLib *>(_driver)->useRhythmChannel() ? 8 : 15); }

	void newMusicData(byte *musicData, int32 musicDataSize) { static_cast<MidiDriver_AdLib *>(_driver)->newMusicData(musicData, musicDataSize); }
};
#endif

int MidiDriver_AdLib::open() {
	int rate = _mixer->getOutputRate();

	debug(3, "ADLIB: Starting driver");

	_opl = OPL::Config::create(OPL::Config::kOpl2);

	if (!_opl)
		return -1;

	_opl->init(rate);

	MidiDriver_Emulated::open();

	_mixer->playStream(Audio::Mixer::kPlainSoundType, &_mixerSoundHandle, this, -1, _mixer->kMaxChannelVolume, 0, DisposeAfterUse::NO);

	return 0;
}

void MidiDriver_AdLib::close() {
	_mixer->stopHandle(_mixerSoundHandle);

	delete _opl;
}

void MidiDriver_AdLib::setVolume(byte volume) {
	_masterVolume = volume;
	//renewNotes(-1, true);
}

// this should normally get called per tick
// but calling it per send() shouldn't be a problem
// TODO: maybe change inUseTimer to a 32-bit integer to make sure there are no overruns?!?!
void MidiDriver_AdLib::updateChannelInUseTimers() {
	for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
		if (_channels[FMvoiceChannel].inUse) {
			_channels[FMvoiceChannel].inUseTimer++;
		}
	}
}

// Called when a music track got loaded into memory
void MidiDriver_AdLib::newMusicData(byte *musicData, int32 musicDataSize) {
	assert(musicDataSize >= 0x7F);
	// MIDI Channel <-> FM Voice Channel mapping at offset 0x22 of music data
	memcpy(&_voiceChannelMapping, musicData + 0x22, 9);

	// reset OPL
	resetAdLib();

	// reset current channel data
	memset(&_channels, 0, sizeof(_channels));
}

void MidiDriver_AdLib::resetAdLib() {

	setRegister(0x01, 0x20); // enable waveform control on both operators
	setRegister(0x04, 0xE0); // Timer control

	setRegister(0x08, 0); // select FM music mode
	setRegister(0xBD, 0); // disable Rhythm

	// reset FM voice instrument data
	resetAdLib_OperatorRegisters(0x20, 0);
	resetAdLib_OperatorRegisters(0x60, 0);
	resetAdLib_OperatorRegisters(0x80, 0);
	resetAdLib_FMVoiceChannelRegisters(0xA0, 0);
	resetAdLib_FMVoiceChannelRegisters(0xB0, 0);
	resetAdLib_FMVoiceChannelRegisters(0xC0, 0);
	resetAdLib_OperatorRegisters(0xE0, 0);
	resetAdLib_OperatorRegisters(0x40, 0x3F);
}

void MidiDriver_AdLib::resetAdLib_OperatorRegisters(byte baseRegister, byte value) {
	byte operatorIndex;

	for (operatorIndex = 0; operatorIndex < 0x16; operatorIndex++) {
		switch (operatorIndex) {
		case 0x06:
		case 0x07:
		case 0x0E:
		case 0x0F:
			break;
		default:
			setRegister(baseRegister + operatorIndex, value);
		}
	}
}

void MidiDriver_AdLib::resetAdLib_FMVoiceChannelRegisters(byte baseRegister, byte value) {
	byte FMvoiceChannel;

	for (FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
		setRegister(baseRegister + FMvoiceChannel, value);
	}
}

// MIDI messages can be found at http://www.midi.org/techspecs/midimessages.php
void MidiDriver_AdLib::send(uint32 b) {
	byte command = b & 0xf0;
	byte channel = b & 0xf;
	byte op1 = (b >> 8) & 0xff;
	byte op2 = (b >> 16) & 0xff;

	updateChannelInUseTimers();

	switch (command) {
	case 0x80:
		noteOff(channel, op1);
		break;
	case 0x90:
		noteOn(channel, op1, op2);
		break;
	case 0xb0: // Control change
		// Doesn't seem to be implemented in the Sherlock Holmes adlib driver
		break;
	case 0xc0: // Program Change
		programChange(channel, op1);
		break;
	case 0xa0: // Polyphonic key pressure (aftertouch)
	case 0xd0: // Channel pressure (aftertouch)
		// Aftertouch doesn't seem to be implemented in the Sherlock Holmes adlib driver
		break;
	case 0xe0:
		warning("pitch bend change");
		pitchBendChange(channel, op1, op2);
		break;
	case 0xf0: // SysEx
		warning("SysEx: %x", b);
		break;
	default:
		warning("ADLIB: Unknown event %02x", command);
	}
}

void MidiDriver_AdLib::generateSamples(int16 *data, int len) {
	_opl->readBuffer(data, len);
}

void MidiDriver_AdLib::noteOn(byte MIDIchannel, byte note, byte velocity) {
	int16  oldestInUseChannel = -1;
	uint16 oldestInUseTimer   = 0;

	if (velocity == 0)
		return noteOff(MIDIchannel, note);

	if (MIDIchannel != 9) {
		// Not Percussion
		for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
			if (_voiceChannelMapping[FMvoiceChannel] == MIDIchannel) {
				if (!_channels[FMvoiceChannel].inUse) {
					_channels[FMvoiceChannel].inUse = true;
					_channels[FMvoiceChannel].currentNote = note;

					voiceOnOff(FMvoiceChannel, true, note, velocity);
					return;
				}
			}
		}

		// Look for oldest in-use channel
		for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
			if (_voiceChannelMapping[FMvoiceChannel] == MIDIchannel) {
				if (_channels[FMvoiceChannel].inUseTimer > oldestInUseTimer) {
					oldestInUseTimer   = _channels[FMvoiceChannel].inUseTimer;
					oldestInUseChannel = FMvoiceChannel;
				}
			}
		}
		if (oldestInUseChannel >= 0) {
			// channel found
			warning("used In-Use channel");
			voiceOnOff(oldestInUseChannel, false, 0, 0);

			_channels[oldestInUseChannel].inUse       = true;
			_channels[oldestInUseChannel].inUseTimer  = 0; // safety, original driver also did this
			_channels[oldestInUseChannel].currentNote = note;
			voiceOnOff(oldestInUseChannel, true, note, velocity);
			return;
		}
		warning("MIDI channel not mapped/all FM voice channels busy %d", MIDIchannel);

	} else {
		// Percussion channel
		warning("percussion!");
		for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
			if (_voiceChannelMapping[FMvoiceChannel] == MIDIchannel) {
				if (note == adlib_percussionChannelTable[FMvoiceChannel].requiredNote) {
					_channels[FMvoiceChannel].inUse = true;
					_channels[FMvoiceChannel].currentNote = note;

					voiceOnOff(FMvoiceChannel, true, adlib_percussionChannelTable[FMvoiceChannel].replacementNote, velocity);
					return;
				}
			}
		}
		warning("percussion MIDI channel not mapped/all FM voice channels busy");
	}
}

void MidiDriver_AdLib::noteOff(byte MIDIchannel, byte note) {
	for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
		if (_voiceChannelMapping[FMvoiceChannel] == MIDIchannel) {
			if (_channels[FMvoiceChannel].currentNote == note) {
				_channels[FMvoiceChannel].inUse = false;
				_channels[FMvoiceChannel].inUseTimer  = 0;
				_channels[FMvoiceChannel].currentNote = 0;

				if (MIDIchannel != 9) {
					// not-percussion
					voiceOnOff(FMvoiceChannel, false, note, 0);
				} else {
					voiceOnOff(FMvoiceChannel, false, adlib_percussionChannelTable[FMvoiceChannel].replacementNote, 0);
				}
				return;
			}
		}
	}
}

void MidiDriver_AdLib::voiceOnOff(byte FMvoiceChannel, bool keyOn, byte note, byte velocity) {
	byte frequencyOffset = 0;
	uint16 frequency = 0;
	byte op2RegAdjust = 0;
	byte regValue40h = 0;
	byte regValueA0h = 0;
	byte regValueB0h = 0;

	// Look up frequency
	if (_channels[FMvoiceChannel].currentInstrumentPtr) {
		frequencyOffset = note + _channels[FMvoiceChannel].currentInstrumentPtr->frequencyAdjust;
	} else {
		frequencyOffset = note;
	}
	if (frequencyOffset >= SHERLOCK_ADLIB_NOTES_COUNT) {
		error("bad note!");
	}
	frequency = adlib_FrequencyLookUpTable[frequencyOffset];

	if (keyOn) {
		// adjust register 40h
		if (_channels[FMvoiceChannel].currentInstrumentPtr) {
			regValue40h = _channels[FMvoiceChannel].currentInstrumentPtr->reg40op2;
		}
		regValue40h = regValue40h - (velocity >> 3);
		op2RegAdjust = adlib_Operator2Register[FMvoiceChannel];
		setRegister(0x40 + op2RegAdjust, regValue40h);
	}

	regValueA0h = frequency & 0xFF;
	regValueB0h = frequency >> 8;
	if (keyOn) {
		regValueB0h |= 0x20; // set Key-On flag
	}

	setRegister(0xA0 + FMvoiceChannel, regValueA0h);
	setRegister(0xB0 + FMvoiceChannel, regValueB0h);
	_channels[FMvoiceChannel].currentA0hReg = regValueA0h;
	_channels[FMvoiceChannel].currentB0hReg = regValueB0h;
}

void MidiDriver_AdLib::pitchBendChange(byte MIDIchannel, byte parameter1, byte parameter2) {
	uint16 channelFrequency              = 0;
	byte   channelRegB0hWithoutFrequency = 0;
	uint16 parameter                     = 0;
	byte   regValueA0h = 0;
	byte   regValueB0h = 0;

	for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
		if (_voiceChannelMapping[FMvoiceChannel] == MIDIchannel) {
			if (_channels[FMvoiceChannel].inUse) {
				// FM voice channel found and it's currently in use -> apply pitch bend change

				// Remove frequency bits from current channel B0h-register
				channelFrequency              = ((_channels[FMvoiceChannel].currentB0hReg << 8) | (_channels[FMvoiceChannel].currentA0hReg)) & 0x3FF;
				channelRegB0hWithoutFrequency = _channels[FMvoiceChannel].currentB0hReg & 0xFC;

				if (parameter2 < 0x40) {
					channelFrequency = channelFrequency / 2;
				} else {
					parameter2 = parameter2 - 0x40;
				}
				parameter1 = parameter1 * 2;
				parameter = parameter1 | (parameter2 << 8);
				parameter = parameter * 4;

				parameter = (parameter >> 8) + 0xFF;
				channelFrequency = channelFrequency * parameter;
				channelFrequency = (channelFrequency >> 8) | (parameter << 8);

				regValueA0h = channelFrequency & 0xFF;
				regValueB0h = (channelFrequency >> 8) | channelRegB0hWithoutFrequency;

				setRegister(0xA0 + FMvoiceChannel, regValueA0h);
				setRegister(0xB0 + FMvoiceChannel, regValueB0h);
			}
		}
	}
}

void MidiDriver_AdLib::programChange(byte MIDIchannel, byte op1) {
	const adlib_InstrumentEntry *instrumentPtr;
	byte op1Reg = 0;
	byte op2Reg = 0;

	// setup instrument
	instrumentPtr = &adlib_instrumentTable[op1];
	//warning("program change for MIDI channel %d, instrument id %d", MIDIchannel, op1);

	for (byte FMvoiceChannel = 0; FMvoiceChannel < SHERLOCK_ADLIB_VOICES_COUNT; FMvoiceChannel++) {
		if (_voiceChannelMapping[FMvoiceChannel] == MIDIchannel) {

			op1Reg = adlib_Operator1Register[FMvoiceChannel];
			op2Reg = adlib_Operator2Register[FMvoiceChannel];

			setRegister(0x20 + op1Reg, instrumentPtr->reg20op1);
			setRegister(0x40 + op1Reg, instrumentPtr->reg40op1);
			setRegister(0x60 + op1Reg, instrumentPtr->reg60op1);
			setRegister(0x80 + op1Reg, instrumentPtr->reg80op1);
			setRegister(0xE0 + op1Reg, instrumentPtr->regE0op1);

			setRegister(0x20 + op2Reg, instrumentPtr->reg20op2);
			setRegister(0x40 + op2Reg, instrumentPtr->reg40op2);
			setRegister(0x60 + op2Reg, instrumentPtr->reg60op2);
			setRegister(0x80 + op2Reg, instrumentPtr->reg80op2);
			setRegister(0xE0 + op2Reg, instrumentPtr->regE0op2);

			setRegister(0xC0 + FMvoiceChannel, instrumentPtr->regC0);

			// Remember instrument
			_channels[FMvoiceChannel].currentInstrumentPtr = instrumentPtr;
		}
	}
}
void MidiDriver_AdLib::setRegister(int reg, int value) {
	_opl->write(0x220, reg);
	_opl->write(0x221, value);
}

uint32 MidiDriver_AdLib::property(int prop, uint32 param) {
#if 0
	switch(prop) {
	case MIDI_PROP_MASTER_VOLUME:
		if (param != 0xffff)
			_masterVolume = param;
		return _masterVolume;
	default:
		break;
	}
#endif
	return 0;
}

#if USE_SCI_MIDI_PLAYER
int MidiPlayer_AdLib::open() {
	return static_cast<MidiDriver_AdLib *>(_driver)->open();
}

void MidiPlayer_AdLib::close() {
	if (_driver) {
		_driver->close();
	}
}

byte MidiPlayer_AdLib::getPlayId() const {
	return 0x00;
}

MidiPlayer *MidiPlayer_AdLib_create() {
	return new MidiPlayer_AdLib();
}

void MidiPlayer_AdLib_newMusicData(MidiPlayer *driver, byte *musicData, int32 musicDataSize) {
	static_cast<MidiPlayer_AdLib *>(driver)->newMusicData(musicData, musicDataSize);
}
#endif

MidiDriver *MidiDriver_AdLib_create() {
	return new MidiDriver_AdLib(g_system->getMixer());
}

void MidiDriver_AdLib_newMusicData(MidiDriver *driver, byte *musicData, int32 musicDataSize) {
	static_cast<MidiDriver_AdLib *>(driver)->newMusicData(musicData, musicDataSize);
}

} // End of namespace Sci
