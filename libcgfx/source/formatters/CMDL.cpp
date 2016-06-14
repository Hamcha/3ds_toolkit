#include <formats/CGFX.h>
#include <utils/exceptions.h>
#include <utils/endian.h>

using namespace cgfx;

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
	memcpy(&mdl.flags, data, 4);
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
	mdl.scale = Vector3::read(data + 0x30, diffEndian);
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
		readDictMap<Mesh>(data + 0xb8, diffEndian, Mesh::read, mdl.meshes);
	}

	//TODO Parse rest of the model (duh)

	return mdl;
}