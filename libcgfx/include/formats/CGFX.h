#pragma once

#include <cstdint>

#include <functional>
#include <map>
#include <vector>

namespace cgfx {
	struct DICTNode {
		uint32_t refbit;
		uint16_t left, right;
		uint32_t nameOffset;
		uint32_t dataOffset;

		uint8_t* realNameOffset;
		uint8_t* realDataOffset;
	};

	struct Node {
		uint32_t ref;

		Node* left;
		Node* right;

		std::string name;

		bool operator==(const Node& other) const { return ref != other.ref; }
		bool operator<(const Node& other) const { return ref > other.ref; }

		static Node fromDICT(const DICTNode& src, const std::vector<Node*>& nodes);
	};

	struct DICT {
		uint32_t entriesCount;
		std::vector<DICTNode> nodes;

		static DICT read(const uint8_t* data, const bool diffEndian);
		//TODO write(uint8_t** data, size_t* size);
	};

	enum Endianess : uint16_t {
		DiffEndian = 0xFFFE,
		SameEndian = 0xFEFF
	};

	struct Vector3 {
		float x, y, z;

		static Vector3 read(const uint8_t* data, const bool diffEndian);
		//TODO bool write(uint8_t** data, size_t* size)
	};

	struct Mat43 {
		float a[4*3];

		static Mat43 read(const uint8_t* data, const bool diffEndian);
		//TODO bool write(uint8_t** data, size_t* size)
	};

	struct Mesh {
		uint32_t flags;
		uint32_t revision;
		std::string name;
		std::string nodeName;

		Vector3 positionOffset;

		//TODO Face groups, Vertex groups

		uint32_t shapeIndex;
		uint32_t materialIndex;
		bool     visible;
		uint8_t  renderPriority;
		uint16_t meshNodeVisibilityIndex;

		static Mesh read(const uint8_t* data, const bool diffEndian);
		//TODO bool write(uint8_t** data, size_t* size)
	};

	struct Shape {
		//TODO
	};

	struct MeshNode {
		//TODO
	};

	struct Material {
		//TODO
	};

	struct SkeletonInfo {
		//TODO
	};

	struct Model {
		uint32_t flags;
		uint32_t revision;
		std::string name;

		Vector3 position;
		Vector3 rotation;
		Vector3 scale;

		Mat43 local;
		Mat43 world;

		std::vector<Mesh> meshes;
		std::vector<Shape> shapes;
		std::map<Node, MeshNode> meshNodes;
		std::map<Node, Material> materials;

		SkeletonInfo skeleton;

		static Model read(const uint8_t* data, const bool diffEndian);
		//TODO bool write(uint8_t** data, size_t* size)
	};

	enum TexFormat : uint32_t {
		TF_RGBA8 = 0x0,
		TF_RGB8 = 0x1,
		TF_RGBA5551 = 0x2,
		TF_RGB565 = 0x3,
		TF_RGBA4 = 0x4,
		TF_LA8 = 0x5,
		TF_HILO8 = 0x6,
		TF_L8 = 0x7,
		TF_A8 = 0x8,
		TF_LA4 = 0x9,
		TF_L4 = 0xa,
		TF_A4 = 0xb,
		TF_ETC1 = 0xc,
		TF_ETC1A4 = 0xd
	};

	struct Texture {
		uint32_t flags;
		uint32_t revision;
		std::string name;

		uint32_t width;
		uint32_t height;

		uint32_t mipmapsCount;

		TexFormat format;

		std::vector<uint8_t> data;

		static Texture read(const uint8_t* data, const bool diffEndian);
		//TODO bool write(uint8_t** data, size_t* size);
	};

	struct LUT {
		//TODO
	};

	struct Shader {
		//TODO
	};

	struct Camera {
		//TODO
	};

	struct Light {
		//TODO
	};

	struct Fog {
		//TODO
	};

	struct Environment {
		//TODO
	};

	struct Emitter {
		//TODO
	};

	struct Particles {
		//TODO
	};

	struct BoneAnimation {
		//TODO
	};

	struct LightAnimation {
		//TODO
	};

	struct CameraAnimation {
		//TODO
	};

	struct MaterialAnimation {
		//TODO
	};

	struct VisibilityAnimation {
		//TODO
	};

	struct CGFX {
		uint16_t endianess;
		uint16_t version;
		uint32_t blockCount;

		std::map<Node, Model> models;
		std::map<Node, Texture> textures;

		std::map<Node, LUT> lookupTables;
		std::map<Node, Fog> fogs;
		std::map<Node, Light> lights;
		std::map<Node, Shader> shaders;
		std::map<Node, Camera> cameras;
		std::map<Node, Emitter> emitters;
		std::map<Node, Particles> particles;
		std::map<Node, Environment> environments;

		std::map<Node, BoneAnimation> boneAnimations;
		std::map<Node, LightAnimation> lightAnimations;
		std::map<Node, CameraAnimation> cameraAnimations;
		std::map<Node, MaterialAnimation> materialAnimations;
		std::map<Node, VisibilityAnimation> visibilityAnimations;

		static CGFX read(const uint8_t* data);
		bool write(uint8_t** data, size_t* size);
	};

	template<typename T>
	void readDictMap(const uint8_t* data, bool diffEndian, std::function<T(const uint8_t*, bool)> readFn, std::map<Node, T>& map) {
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
			Node mapNode = Node::fromDICT(node, nodeList);
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
}
