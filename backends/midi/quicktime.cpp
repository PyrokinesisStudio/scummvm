/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001/2002 The ScummVM project
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 */

#if defined(MACOSX) || defined(macintosh)


#include "sound/mpu401.h"
#include "common/engine.h"	// for warning/error/debug

#if defined(MACOSX)
#include <QuickTime/QuickTimeComponents.h>
#include <QuickTime/QuickTimeMusic.h>
#else
#include <QuickTimeComponents.h>
#include <QuickTimeMusic.h>
#endif


// FIXME - the following disables reverb support in the QuickTime / CoreAudio
// midi backends. For some reasons, reverb will suck away a *lot* of CPU time.
// Until we know for sure what is causing this and if there is a better way to
// fix the problem, we just disable all reverb for these backends.
#define COREAUDIO_REVERB_HACK


/* QuickTime MIDI driver
 * Original QuickTime support by Florent Boudet <flobo@ifrance.com>
 * Modified by Max Horn
 */
class MidiDriver_QT : public MidiDriver_MPU401 {
public:
	int open(int mode);
	void close();
	void send(uint32 b);
	void pause(bool p) { }
	void set_stream_callback(void *param, StreamCallback *sc);
	void setPitchBendRange (byte channel, uint range);

private:
	NoteAllocator qtNoteAllocator;
	NoteChannel qtNoteChannel[16];
	NoteRequest simpleNoteRequest;

	StreamCallback *_stream_proc;
	void *_stream_param;
	int _mode;

	// Pitch bend tracking. Necessary since QTMA handles
	// pitch bending so differently from MPU401.
	uint16 _pitchbend [16];
	byte _pitchbend_range [16];
};

void MidiDriver_QT::set_stream_callback(void *param, StreamCallback *sc)
{
	_stream_param = param;
	_stream_proc = sc;
}

int MidiDriver_QT::open(int mode)
{
	ComponentResult qtErr = noErr;
	int i;

	qtNoteAllocator = NULL;

	if (mode == MO_STREAMING)
		return MERR_STREAMING_NOT_AVAILABLE;

	_mode = mode;

	for (i = 0; i < 15; i++)
		qtNoteChannel[i] = NULL;

	qtNoteAllocator = OpenDefaultComponent(kNoteAllocatorComponentType, 0);
	if (qtNoteAllocator == NULL)
		goto bail;

	simpleNoteRequest.info.flags = 0;
	*(short *)(&simpleNoteRequest.info.polyphony) = EndianS16_NtoB(15);	// simultaneous tones
	*(Fixed *) (&simpleNoteRequest.info.typicalPolyphony) = EndianU32_NtoB(0x00010000);

	qtErr = NAStuffToneDescription(qtNoteAllocator, 1, &simpleNoteRequest.tone);
	if (qtErr != noErr)
		goto bail;

	for (i = 0; i < 15; i++) {
		qtErr = NANewNoteChannel(qtNoteAllocator, &simpleNoteRequest, &(qtNoteChannel[i]));
		if ((qtErr != noErr) || (qtNoteChannel == NULL))
			goto bail;
	}
	return 0;

bail:
	error("Init QT failed %x %x %d\n", (int)qtNoteAllocator, (int)qtNoteChannel, (int)qtErr);
	for (i = 0; i < 15; i++) {
		if (qtNoteChannel[i] != NULL)
			NADisposeNoteChannel(qtNoteAllocator, qtNoteChannel[i]);
		qtNoteChannel[i] = NULL;
	}

	if (qtNoteAllocator != NULL) {
		CloseComponent(qtNoteAllocator);
		qtNoteAllocator = NULL;
	}

	return MERR_DEVICE_NOT_AVAILABLE;
}

void MidiDriver_QT::close()
{
	_mode = 0;

	for (int i = 0; i < 15; i++) {
		if (qtNoteChannel[i] != NULL)
			NADisposeNoteChannel(qtNoteAllocator, qtNoteChannel[i]);
		qtNoteChannel[i] = NULL;
	}

	if (qtNoteAllocator != NULL) {
		CloseComponent(qtNoteAllocator);
		qtNoteAllocator = NULL;
	}
}

void MidiDriver_QT::send(uint32 b)
{
	if (_mode != MO_SIMPLE)
		error("MidiDriver_QT:send called but driver is not in simple mode");

	MusicMIDIPacket midPacket;
	unsigned char *midiCmd = midPacket.data;
	midPacket.length = 3;
	midiCmd[3] = (b & 0xFF000000) >> 24;
	midiCmd[2] = (b & 0x00FF0000) >> 16;
	midiCmd[1] = (b & 0x0000FF00) >> 8;
	midiCmd[0] = (b & 0x000000FF);

	unsigned char chanID = midiCmd[0] & 0x0F;
	switch (midiCmd[0] & 0xF0) {
	case 0x80:										// Note off
		NAPlayNote(qtNoteAllocator, qtNoteChannel[chanID], midiCmd[1], 0);
		break;

	case 0x90:										// Note on
		NAPlayNote(qtNoteAllocator, qtNoteChannel[chanID], midiCmd[1], midiCmd[2]);
		break;

	case 0xB0:										// Effect
		switch (midiCmd[1]) {
		case 0x01:									// Modulation
			NASetController(qtNoteAllocator, qtNoteChannel[chanID],
											kControllerModulationWheel, midiCmd[2] << 8);
			break;

		case 0x07:									// Volume
			NASetController(qtNoteAllocator, qtNoteChannel[chanID], kControllerVolume, midiCmd[2] * 300);
			break;

		case 0x0A:									// Pan
			NASetController(qtNoteAllocator, qtNoteChannel[chanID], kControllerPan,
											(midiCmd[2] << 1) + 0xFF);
			break;

		case 0x40:									// Sustain on/off
			NASetController(qtNoteAllocator, qtNoteChannel[chanID], kControllerSustain, midiCmd[2]);
			break;

		case 0x5b:									// ext effect depth
#if !defined(COREAUDIO_REVERB_HACK)
			NASetController(qtNoteAllocator, qtNoteChannel[chanID], kControllerReverb, midiCmd[2] << 8);
#endif
			break;

		case 0x5d:									// chorus depth
			NASetController(qtNoteAllocator, qtNoteChannel[chanID], kControllerChorus, midiCmd[2] << 8);
			break;

		case 0x7b:									// mode message all notes off
			for (int i = 0; i < 128; i++)
				NAPlayNote(qtNoteAllocator, qtNoteChannel[chanID], i, 0);
			break;
		case 0x64:
		case 0x65:
		case 0x06:
		case 0x26:
			// pitch bend changes - ignore those for now
			break;

		case 0x12:	// Occurs in Scumm games
		case 0x79:	// Occurs in Simon1
			// What are these ?!? Ignore it for now
			break;

		default:
			// Error: Unknown MIDI effect: 007f76b3
			warning("Unknown MIDI effect: %08x", (int)b);
			break;
		}
		break;

	case 0xC0:										// Program change
		NASetInstrumentNumber(qtNoteAllocator, qtNoteChannel[chanID], midiCmd[1] + 1);
		break;

	case 0xE0:{									// Pitch bend
			// QuickTime specifies pitchbend in semitones, using 8.8 fixed point values;
			// but iMuse sends us the pitch bend data as 0-16383. which has to be mapped
			// to +/- 12 semitones. Based on this, we first center the input data, then
			// multiply it by a factor. If all was right, the factor would be 3/8, but for
			// mysterious reasons the actual factor we have to use is more like 1/32 or 3/64.
			// Maybe the QT docs are right, and 
			_pitchbend[chanID] = ((uint16) midiCmd[1] | (uint16) (midiCmd[2] << 7));
 			long theBend = ((long) _pitchbend[chanID] - 0x2000) * _pitchbend_range[chanID] / 32;

			NASetController(qtNoteAllocator, qtNoteChannel[chanID], kControllerPitchBend, theBend);
		}
		break;

	default:
		error("Unknown Command: %08x", (int)b);
		NASendMIDI(qtNoteAllocator, qtNoteChannel[chanID], &midPacket);
		break;
	}
}

void MidiDriver_QT::setPitchBendRange (byte channel, uint range)
{
	if (_pitchbend_range[channel] == range)
		return;
	_pitchbend_range[channel] = range;

	long theBend = _pitchbend[channel];
	theBend = (theBend - 0x2000) * range / 32;
	NASetController(qtNoteAllocator, qtNoteChannel[channel], kControllerPitchBend, theBend);
}

MidiDriver *MidiDriver_QT_create()
{
	return new MidiDriver_QT();
}

#endif // MACOSX || macintosh
