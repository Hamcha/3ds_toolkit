#pragma once

#include <cstdint>

#include <vector>

#include "../formatTypes.h"

enum Endianess: uint16_t {
	DiffEndian = 0xFFFE,
	SameEndian = 0xFEFF
};

namespace cgfx {

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
		uint32_t type;
		uint32_t revision;

		std::string name;

		Vector3 position;
		Vector3 rotation;
		Vector3 scale;

		Mat43 local;
		Mat43 world;

		std::vector<Mesh> meshes;
		std::vector<Shape> shapes;
		std::vector<MeshNode> meshNodes;
		std::vector<Material> materials;

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

		std::vector<Model> models;
		std::vector<Texture> textures;

		std::vector<LUT> lookupTables;
		std::vector<Fog> fogs;
		std::vector<Light> lights;
		std::vector<Shader> shaders;
		std::vector<Camera> cameras;
		std::vector<Emitter> emitters;
		std::vector<Particles> particles;
		std::vector<Environment> environments;

		std::vector<BoneAnimation> boneAnimations;
		std::vector<LightAnimation> lightAnimations;
		std::vector<CameraAnimation> cameraAnimations;
		std::vector<MaterialAnimation> materialAnimations;
		std::vector<VisibilityAnimation> visibilityAnimations;
	};

}