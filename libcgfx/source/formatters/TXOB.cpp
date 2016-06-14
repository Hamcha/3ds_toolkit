#include <formats/CGFX.h>
#include <utils/exceptions.h>
#include <utils/endian.h>

using namespace cgfx;

Texture Texture::read(const uint8_t* data, const bool diffEndian) {
	Texture tex;

	// Check segment signature
	if (strncmp("TXOB", (char*)(data + 0x04), 4) != 0) {
		throw ParseException("TXOB", "Failed TXOB signature check");
	}

	// Copy basic data from header
	memcpy(&tex.flags, data, 4);
	memcpy(&tex.revision, data + 0x08, 4);
	if (diffEndian) {
		endianSwap(tex.flags);
		endianSwap(tex.revision);
	}

	// Get name from offset
	uint32_t nameOffset;
	memcpy(&nameOffset, data + 0x0c, 4);
	if (diffEndian) {
		endianSwap(nameOffset);
	}
	tex.name = std::string((const char*)(data + 0x0c + nameOffset));

	// Get width/height
	memcpy(&tex.width, data + 0x18, 4);
	memcpy(&tex.height, data + 0x1c, 4);
	if (diffEndian) {
		endianSwap(tex.width);
		endianSwap(tex.height);
	}

	// Get texture format
	memcpy(&tex.format, data + 0x34, 4);
	if (diffEndian) {
		endianSwap((uint32_t&)tex.format);
	}

	// Get texture data size
	uint32_t size;
	memcpy(&size, data + 0x44, 4);
	if (diffEndian) {
		endianSwap(size);
	}

	// Copy texture data into data vector
	tex.data = std::vector<uint8_t>(data + 0x48, data + 0x48 + size);

	//TODO Parse rest of the texture (duh)

	return tex;
}
