#include <formats/CGFX.h>
#include <utils/exceptions.h>
#include <utils/endian.h>

#include <functional>

using namespace cgfx;

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

	static DICT read(const uint8_t* data, const bool diffEndian);
	//TODO write(uint8_t** data, size_t* size);
};

DICT DICT::read(const uint8_t* data, const bool diffEndian) {
	DICT out;

	// Check segment signature
	if (strncmp("DICT", (char*)data, 4) != 0) {
		throw ParseException("DICT", "Failed DICT signature check");
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

static Node nodeFromDICT(const DICTNode& src, const std::vector<Node*>& nodes) {
	return Node{ src.refbit, nodes.at(src.left), nodes.at(src.right), std::string((const char*)src.realNameOffset) };
}

template<typename T>
static void readDICTMap(const uint8_t* data, bool diffEndian, std::function<T(const uint8_t*, bool)> readFn, std::map<Node, T>& map) {
	uint32_t dictOffset;
	memcpy(&dictOffset, data, 4);
	if (diffEndian) {
		endianSwap(dictOffset);
	}

	DICT dict = DICT::read(data + dictOffset, diffEndian);

	// Grab and parse each texture from the DICT
	std::vector<Node*> nodeList = {};
	nodeList.resize(dict.nodes.size());
	for (const DICTNode& node : dict.nodes) {
		// Parse node and add to local node list
		Node mapNode = nodeFromDICT(node, nodeList);
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

Vector3 Vector3::read(const uint8_t* data, const bool diffEndian) {
	Vector3 vec;
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

Mat43 Mat43::read(const uint8_t* data, const bool diffEndian) {
	Mat43 mat;
	memcpy(&mat.a, data, 4*(4*3));

	if (diffEndian) {
		for (uint8_t i = 0; i < 4*3; i++) {
			endianSwap(mat.a[i]);
		}
	}

	return mat;
}

Mesh Mesh::read(const uint8_t* data, const bool diffEndian) {
	Mesh mesh;

	// Check segment signature
	if (strncmp("SOBJ", (char*)(data + 0x04), 4) != 0) {
		throw ParseException("SOBJ", "Failed SOBJ signature check");
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

Model Model::read(const uint8_t* data, const bool diffEndian) {
	Model mdl;

	// Check segment signature
	if (strncmp("CMDL", (char*)(data + 0x04), 4) != 0) {
		throw ParseException("CMDL", "Failed CMDL signature check");
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
	mdl.scale    = Vector3::read(data + 0x30, diffEndian);
	mdl.rotation = Vector3::read(data + 0x3c, diffEndian);
	mdl.position = Vector3::read(data + 0x48, diffEndian);

	// Read local/world matrices
	mdl.local = Mat43::read(data + 0x54, diffEndian);
	mdl.world = Mat43::read(data + 0x84, diffEndian);

	// Read meshes
	uint32_t meshCount;
	memcpy(&meshCount, data + 0xb4, 4);
	if (diffEndian) {
		endianSwap(meshCount);
	}

	if (meshCount > 0) {
		readDICTMap<Mesh>(data + 0xb8, diffEndian, Mesh::read, mdl.meshes);
	}

	//TODO Parse rest of the model (duh)

	return mdl;
}

Texture Texture::read(const uint8_t* data, const bool diffEndian) {
	Texture tex;

	// Check segment signature
	if (strncmp("TXOB", (char*)(data + 0x04), 4) != 0) {
		throw ParseException("TXOB", "Failed TXOB signature check");
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
		readDICTMap<Model>(data + dataOff + 0x0c, diffEndian, Model::read, cgdata.models);
	}

	// Read and parse textures
	if (texCount > 0) {
		readDICTMap<Texture>(data + dataOff + 0x14, diffEndian, Texture::read, cgdata.textures);
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