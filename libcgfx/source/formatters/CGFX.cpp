#include <formats/CGFX.h>
#include <utils/exceptions.h>
#include <utils/endian.h>

using namespace cgfx;

CGFX CGFX::read(const uint8_t* data) {
	CGFX cgdata;

	// Check file signature
	if (strncmp("CGFX", (char*)data, 4) != 0) {
		throw ParseException("CGFX", "Failed CFGX signature check in CGFX file");
	}

	// Get params from CGFX header
	memcpy(&cgdata.endianess,  data + 0x04, 2);
	bool diffEndian = cgdata.endianess == DiffEndian;

	memcpy(&cgdata.version,    data + 0x0a, 2);
	memcpy(&cgdata.blockCount, data + 0x10, 4);
	if (diffEndian) {
		endianSwap(cgdata.version);
		endianSwap(cgdata.blockCount);
	}

	// Get data from the DATA segment

	static const size_t dataOff = 0x14;
	// Check data signature
	if (strncmp("DATA", (char*)(data + dataOff), 4) != 0) {
		throw ParseException("CGFX", "Failed DATA signature check");
	}

	uint32_t modelCount, texCount;
	memcpy(&modelCount, data + dataOff + 0x08, 4);
	memcpy(&texCount,   data + dataOff + 0x10, 4);
	if (diffEndian) {
		endianSwap(modelCount);
		endianSwap(texCount);
	}

	// Read and parse models
	if (modelCount > 0) {
		readDictMap<Model>(data + dataOff + 0x0c, diffEndian, Model::read, cgdata.models);
	}

	// Read and parse textures
	if (texCount > 0) {
		readDictMap<Texture>(data + dataOff + 0x14, diffEndian, Texture::read, cgdata.textures);
	}

	return cgdata;
}

bool CGFX::write(uint8_t** data, size_t* size) {
	//TODO Serialize everything else first

	// Initialize with signature and little-endianess
	std::vector<uint8_t> bytes = { 'C', 'G', 'F', 'X', 0xff, 0xfe };
	//TODO Version and block count

	// Copy serialized data to buffer
	*size = bytes.size();
	*data = (uint8_t*)malloc(*size);
	std::copy(bytes.begin(), bytes.end(), *data);

	return true;
}