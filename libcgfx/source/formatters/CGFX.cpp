#include <formatters/CGFX.h>
#include <utils/endian.h>

#include <vector>

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
		uint32_t dictOffset;
		memcpy(&dictOffset, data + dataOff + 0x0c, 4);
		if (diffEndian) {
			endianSwap(dictOffset);
		}

		DICT modelDict = readDICT(data + dataOff + 0x0c + dictOffset, diffEndian);
		if (modelDict.nodes.size() < 1) {
			return false;
		}
		
		// Grab and parse each model from the DICT
		std::vector<Node*> nodeList = {};
		nodeList.resize(modelDict.nodes.size());
		for (const DICTNode& node : modelDict.nodes) {
			// Parse node and add to local node list
			Node mapNode = nodeFromDICT(node, nodeList);
			nodeList.push_back(&mapNode);

			// Ignore root node
			if (node.refbit == 0xffffffff) {
				continue;
			}

			// Parse model and add to model list
			Model model = readCMDL(node.realDataOffset, diffEndian);
			cgdata.models[mapNode] = model;
		}
	}

	// Read and parse textures
	if (texCount > 0) {
		uint32_t dictOffset;
		memcpy(&dictOffset, data + dataOff + 0x14, 4);
		if (diffEndian) {
			endianSwap(dictOffset);
		}

		DICT texDict = readDICT(data + dataOff + 0x14 + dictOffset, diffEndian);
		if (texDict.nodes.size() < 1) {
			return false;
		}

		// Grab and parse each texture from the DICT
		std::vector<Node*> nodeList = {};
		nodeList.resize(texDict.nodes.size());
		for (const DICTNode& node : texDict.nodes) {
			// Parse node and add to local node list
			Node mapNode = nodeFromDICT(node, nodeList);
			nodeList.push_back(&mapNode);

			// Ignore root node
			if (node.refbit == 0xffffffff) {
				continue;
			}

			// Parse model and add to model list
			Texture texture = readTXOB(node.realDataOffset, diffEndian);
			cgdata.textures[mapNode] = texture;
		}
	}

	hasLoaded = true;
	return true;
}