#pragma once

#include "glm/glm.hpp"
#include "Ray.h"

struct HitPayload {
	float HitDistance;
	glm::vec3 WorldPosition;
	glm::vec3 WorldNormal;

	uint32_t ObjectIndex;
};

class Hittable {
public: 
	virtual bool Hit(const Ray& r, float tMin, float tMax, HitPayload& rec) const = 0;
};