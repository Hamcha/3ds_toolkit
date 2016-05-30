#pragma once

#include <cstdint>

#include <vector>

#include "../formatTypes.h"

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

struct Texture {
	//TODO
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
	uint32_t blocksCount;

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