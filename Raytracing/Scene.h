#pragma once

#include<glm/glm.hpp>
#include<vector>

struct Sphere {
	glm::vec3 pos = glm::vec3(0.0f);
	float radius = 0.5f;

	glm::vec4 albedo = glm::vec4(1.0f);
};

struct Scene {
	std::vector<Sphere> spheres;
};
