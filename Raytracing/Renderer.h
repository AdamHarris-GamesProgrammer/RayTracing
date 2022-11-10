#pragma once

#include "Walnut/Image.h"
#include "glm/glm.hpp"
#include <memory>

#include "Scene.h"

#include "Ray.h"

#include "Hittable.h"

class Renderer
{
public:
	struct Settings {
		bool Accumulate = true;
	};


	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const class Camera& camera);

	Settings& GetSettings() { return _settings; }

	glm::vec3 GetLightDir() { return _lightDir; }
	void SetLightDir(glm::vec3 newDir) { _lightDir = newDir; }

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return _finalImage; }

	void ResetFrameIndex() { _frameIndex = 1; }

private:


	HitPayload TraceRay(const Ray& ray);

	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);

	HitPayload MissHit(const Ray& ray);

	//Invoked for every pixel we are rendering
	glm::vec4 PerPixel(uint32_t x, uint32_t y);

private:
	uint32_t* _imageData = nullptr;
	std::shared_ptr<Walnut::Image> _finalImage;

	Settings _settings;

	const Scene* _activeScene;
	const Camera* _activeCamera;


	glm::vec3 _lightDir = glm::vec3(-1.0f);


	glm::vec4* _accumulationData;

	uint32_t _frameIndex = 1;
};

