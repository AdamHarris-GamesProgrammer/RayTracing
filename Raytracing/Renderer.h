#pragma once

#include "Walnut/Image.h"
#include "glm/glm.hpp"
#include <memory>


#include "Ray.h"



class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const class Camera& camera);

	glm::vec3 GetLightDir() const { return _lightDir; }
	void SetLightDir(glm::vec3 newDir) { _lightDir = newDir; }

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return _finalImage; }

private:
	glm::vec4 TraceRay(const Ray& ray);


private:
	uint32_t* _imageData = nullptr;
	std::shared_ptr<Walnut::Image> _finalImage;

	glm::vec3 _lightDir = glm::vec3(-1.0f);

};

