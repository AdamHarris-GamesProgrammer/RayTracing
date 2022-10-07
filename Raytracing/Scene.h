#pragma once

#include<glm/glm.hpp>
#include<vector>

struct Sphere {
	glm::vec3 pos = glm::vec3(0.0f);
	float radius = 0.5f;

	glm::vec3 albedo = glm::vec3(1.0f);
};

struct Scene {
	std::vector<Sphere> spheres;
};
