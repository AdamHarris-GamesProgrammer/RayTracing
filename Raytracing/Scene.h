#pragma once

#include<glm/glm.hpp>
#include<vector>

#include "Hittable.h"

#include "Ray.h"
#include <string>

struct Material {
	glm::vec3 albedo = glm::vec3(1.0f);
	float roughness = 1.0f;
	float metallic = 0.0f;
};

class Sphere : public Hittable {
public:
	glm::vec3 pos = glm::vec3(0.0f);
	float radius = 0.5f;

	int materialIndex = 0;


	// Inherited via Hittable
	virtual bool Hit(const Ray& r, float tMin, float tMax, HitPayload& rec) const override;

};


struct Scene {
	std::string name;

	std::vector<Sphere> spheres;
	std::vector<Material> materials;
};
