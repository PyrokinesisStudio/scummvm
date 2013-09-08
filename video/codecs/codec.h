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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef VIDEO_CODECS_CODEC_H
#define VIDEO_CODECS_CODEC_H

#include "graphics/surface.h"
#include "graphics/pixelformat.h"

namespace Common {
class SeekableReadStream;
}

namespace Video {

/**
 * An abstract representation of a video codec used for decoding
 * video frames.
 *
 * Used in video:
 *  - AVIDecoder
 *  - QuickTimeDecoder
 *  - VMDDecoder
 */
class Codec {
public:
	Codec() {}
	virtual ~Codec() {}

	/**
	 * Decode the frame for the given data and return a pointer to a surface
	 * containing the decoded frame.
	 *
	 * @return a pointer to the decoded frame
	 * @note stream is not deleted
	 */
	virtual const Graphics::Surface *decodeImage(Common::SeekableReadStream *stream) = 0;

	/**
	 * Get the format that the surface returned from decodeImage() will
	 * be in.
	 */
	virtual Graphics::PixelFormat getPixelFormat() const = 0;

	/**
	 * Can this codec's frames contain a palette?
	 */
	virtual bool containsPalette() const { return false; }

	/**
	 * Get the palette last decoded from decodeImage
	 */
	virtual const byte *getPalette() { return 0; }

	/**
	 * Does the codec have a dirty palette?
	 */
	virtual bool hasDirtyPalette() const { return false; }
};

} // End of namespace Video

#endif
