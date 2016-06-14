#include <formats/CGFX.h>
#include <utils/exceptions.h>
#include <utils/endian.h>

using namespace cgfx;

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
	static const size_t entryOff = 0x0c;
	static const size_t entrySize = 0x10;
	for (uint32_t i = 0; i < out.entriesCount + 1; i++) {
		size_t nodeOffset = entryOff + entrySize * i;

		DICTNode node;
		memcpy(&node.refbit, data + nodeOffset, 4);
		memcpy(&node.left, data + nodeOffset + 0x04, 2);
		memcpy(&node.right, data + nodeOffset + 0x06, 2);
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

Node Node::fromDICT(const DICTNode& src, const std::vector<Node*>& nodes) {
	return Node { src.refbit, nodes.at(src.left), nodes.at(src.right), std::string((const char*)src.realNameOffset) };
}