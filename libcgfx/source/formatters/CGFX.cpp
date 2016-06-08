#include <formatters/CGFX.h>
#include <utils/endian.h>

struct DICTNode {
	uint32_t refbit;
	uint16_t left, right;
	uint32_t nameOffset;
	uint32_t dataOffset;
};

struct DICT {
	uint32_t sectionSize;
	uint32_t entriesCount;
	std::vector<DICTNode> nodes;
};

bool cgfx::CGFX::loadFile(const uint8_t* data, const size_t size) {
	// Check file signature
	if (strncmp("CGFX", (char*)data, 4) != 0) {
		//TODO Find a way to say what error we encountered
		return false;
	}

	// Get params from CGFX header
	memcpy(&cgdata.endianess,  data + 0x04, 2);
	memcpy(&cgdata.version,    data + 0x0a, 2);
	memcpy(&cgdata.blockCount, data + 0x10, 4);
	if (cgdata.endianess == DiffEndian) {
		endianSwap(cgdata.version);
		endianSwap(cgdata.blockCount);
	}

	// Get data from the DATA segment

	static const size_t dataOff = 0x14;
	// Check data signature
	if (strncmp("DATA", (char*)(data + dataOff), 4) != 0) {
		//TODO Find a way to say what error we encountered
		return false;
	}

	uint32_t modelCount, texCount;
	memcpy(&modelCount, data + dataOff + 0x08, 4);
	memcpy(&texCount,   data + dataOff + 0x10, 4);
	if (cgdata.endianess == DiffEndian) {
		endianSwap(modelCount);
		endianSwap(texCount);
	}

	cgdata.models.resize(modelCount);
	cgdata.textures.resize(texCount);

	// Read and parse models
	if (modelCount > 0) {
		//TODO
	}

	// Read and parse textures
	if (texCount > 0) {
		//TODO
	}

	hasLoaded = true;
	return true;
}