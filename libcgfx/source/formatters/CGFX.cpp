#include <formatters/CGFX.h>
#include <utils/endian.h>

#include <functional>

struct DICTNode {
	uint32_t refbit;
	uint16_t left, right;
	uint32_t nameOffset;
	uint32_t dataOffset;

	uint8_t* realNameOffset;
	uint8_t* realDataOffset;
};

struct DICT {
	uint32_t entriesCount;
	std::vector<DICTNode> nodes;
};

static DICT readDICT(const uint8_t* data, const bool diffEndian) {
	DICT out;

	// Check segment signature
	if (strncmp("DICT", (char*)data, 4) != 0) {
		std::fprintf(stderr, "PARSE ERR: Failed DICT signature check\r\n");
		return out;
	}

	// Read # of entries
	memcpy(&out.entriesCount, data + 0x08, 4);
	if (diffEndian) {
		endianSwap(out.entriesCount);
	}

	// Read each entry (entriesCount does not count root node, so add 1)
	static const size_t entryOff  = 0x0c;
	static const size_t entrySize = 0x10;
	for (uint32_t i = 0; i < out.entriesCount + 1; i++) {
		size_t nodeOffset = entryOff + entrySize * i;

		DICTNode node;
		memcpy(&node.refbit,     data + nodeOffset,        4);
		memcpy(&node.left,       data + nodeOffset + 0x04, 2);
		memcpy(&node.right,      data + nodeOffset + 0x06, 2);
		memcpy(&node.nameOffset, data + nodeOffset + 0x08, 4);
		memcpy(&node.dataOffset, data + nodeOffset + 0x0c, 4);
		if (diffEndian) {
			endianSwap(node.refbit);
			endianSwap(node.left);
			endianSwap(node.right);
			endianSwap(node.nameOffset);
			endianSwap(node.dataOffset);
		}

		node.realNameOffset = (uint8_t*)(data + nodeOffset + 0x08 + node.nameOffset);
		node.realDataOffset = (uint8_t*)(data + nodeOffset + 0x0c + node.dataOffset);

		out.nodes.push_back(node);
	}

	return out;
}

static cgfx::Node nodeFromDICT(const DICTNode& src, const std::vector<cgfx::Node*>& nodes) {
	return cgfx::Node{ src.refbit, nodes.at(src.left), nodes.at(src.right), std::string((const char*)src.realNameOffset) };
}

template<typename T>
static void readDICTMap(const uint8_t* data, std::function<T(const uint8_t*, bool)> readFn, std::map<cgfx::Node, T>& map, bool diffEndian) {
	uint32_t dictOffset;
	memcpy(&dictOffset, data, 4);
	if (diffEndian) {
		endianSwap(dictOffset);
	}

	DICT dict = readDICT(data + dictOffset, diffEndian);

	// Grab and parse each texture from the DICT
	std::vector<cgfx::Node*> nodeList = {};
	nodeList.resize(dict.nodes.size());
	for (const DICTNode& node : dict.nodes) {
		// Parse node and add to local node list
		cgfx::Node mapNode = nodeFromDICT(node, nodeList);
		nodeList.push_back(&mapNode);

		// Ignore root node
		if (node.refbit == 0xffffffff) {
			continue;
		}

		// Parse model and add to model list
		T mesh = readFn(node.realDataOffset, diffEndian);
		map[mapNode] = mesh;
	}
}

static cgfx::Vector3 readVec3(const uint8_t* data, const bool diffEndian) {
	cgfx::Vector3 vec;
	memcpy(&vec.x, data,     4);
	memcpy(&vec.y, data + 4, 4);
	memcpy(&vec.z, data + 8, 4);
	if (diffEndian) {
		endianSwap(vec.x);
		endianSwap(vec.y);
		endianSwap(vec.z);
	}
	return vec;
}

static cgfx::Mat43 readMat43(const uint8_t* data, const bool diffEndian) {
	cgfx::Mat43 mat;
	memcpy(&mat.a, data, 4*(4*3));

	if (diffEndian) {
		for (uint8_t i = 0; i < 4*3; i++) {
			endianSwap(mat.a[i]);
		}
	}

	return mat;
}

static cgfx::Mesh readMesh(const uint8_t* data, const bool diffEndian) {
	cgfx::Mesh mesh;

	// Check segment signature
	if (strncmp("SOBJ", (char*)(data + 0x04), 4) != 0) {
		std::fprintf(stderr, "PARSE ERR: Failed SOBJ signature check\r\n");
		return mesh;
	}

	// Copy basic data from header
	memcpy(&mesh.flags, data, 4);
	memcpy(&mesh.revision, data + 0x08, 4);
	if (diffEndian) {
		endianSwap(mesh.flags);
		endianSwap(mesh.revision);
	}

	// Get name from offset
	uint32_t nameOffset;
	memcpy(&nameOffset, data + 0x0c, 4);
	if (diffEndian) {
		endianSwap(nameOffset);
	}
	mesh.name = std::string((const char*)(data + 0x0c + nameOffset));

	return mesh;
}

static cgfx::Model readCMDL(const uint8_t* data, const bool diffEndian) {
	cgfx::Model mdl;

	// Check segment signature
	if (strncmp("CMDL", (char*)(data + 0x04), 4) != 0) {
		std::fprintf(stderr, "PARSE ERR: Failed CMDL signature check\r\n");
		return mdl;
	}

	// Copy basic data from header
	memcpy(&mdl.flags,    data,        4);
	memcpy(&mdl.revision, data + 0x08, 4);
	if (diffEndian) {
		endianSwap(mdl.flags);
		endianSwap(mdl.revision);
	}

	// Get name from offset
	uint32_t nameOffset;
	memcpy(&nameOffset, data + 0x0c, 4);
	if (diffEndian) {
		endianSwap(nameOffset);
	}
	mdl.name = std::string((const char*)(data + 0x0c + nameOffset));

	// Get position/rotation/scale
	mdl.scale    = readVec3(data + 0x30, diffEndian);
	mdl.rotation = readVec3(data + 0x3c, diffEndian);
	mdl.position = readVec3(data + 0x48, diffEndian);

	// Read local/world matrices
	mdl.local = readMat43(data + 0x54, diffEndian);
	mdl.world = readMat43(data + 0x84, diffEndian);

	// Read meshes
	uint32_t meshCount;
	memcpy(&meshCount, data + 0xb4, 4);
	if (diffEndian) {
		endianSwap(meshCount);
	}

	if (meshCount > 0) {
		readDICTMap<cgfx::Mesh>(data + 0xb8, readMesh, mdl.meshes, diffEndian);
	}

	//TODO Parse rest of the model (duh)

	return mdl;
}

static cgfx::Texture readTXOB(const uint8_t* data, const bool diffEndian) {
	cgfx::Texture tex;

	// Check segment signature
	if (strncmp("TXOB", (char*)(data + 0x04), 4) != 0) {
		std::fprintf(stderr, "PARSE ERR: Failed TXOB signature check\r\n");
		return tex;
	}

	// Copy basic data from header
	memcpy(&tex.flags,    data,        4);
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
	memcpy(&tex.width,  data + 0x18, 4);
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

bool cgfx::CGFX::loadFile(const uint8_t* data, const size_t size) {
	// Check file signature
	if (strncmp("CGFX", (char*)data, 4) != 0) {
		std::fprintf(stderr, "PARSE ERR: Failed CFGX signature check in CGFX file\r\n");
		return false;
	}

	// Get params from CGFX header
	memcpy(&cgdata.endianess,  data + 0x04, 2);
	diffEndian = cgdata.endianess == DiffEndian;

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
		std::fprintf(stderr, "PARSE ERR: Failed DATA signature check\r\n");
		//TODO Find a way to say what error we encountered
		return false;
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
		readDICTMap<Model>(data + dataOff + 0x0c, readCMDL, cgdata.models, diffEndian);
	}

	// Read and parse textures
	if (texCount > 0) {
		readDICTMap<Texture>(data + dataOff + 0x14, readTXOB, cgdata.textures, diffEndian);
	}

	hasLoaded = true;
	return true;
}

bool cgfx::CGFX::serialize(uint8_t** data, size_t* size) {
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