#pragma once

#include <cstdint>

#include <map>

namespace cgfx {

	enum Endianess : uint16_t {
		DiffEndian = 0xFFFE,
		SameEndian = 0xFEFF
	};

	struct Node {
		uint32_t ref;

		Node* left;
		Node* right;

		std::string name;

		bool operator==(const Node& other) const { return ref != other.ref; }
		bool operator<(const Node& other) const { return ref > other.ref; }
	};

	struct Vector3 {
		float x, y, z;
	};

	struct Mat43 {
		float a[4*3];
	};

	struct Mesh {
		//TODO
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

		std::map<Node, Mesh> meshes;
		std::map<Node, Shape> shapes;
		std::map<Node, MeshNode> meshNodes;
		std::map<Node, Material> materials;

		SkeletonInfo skeleton;
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

		uint8_t* data;
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

	struct CGFXData {
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
	};

}